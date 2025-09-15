#include <QQBot.h>

QQBot::QQBot() 
	: _groupName(""), _mainGroup(0), _msgCenter(0), _symbol(""), _botConfig(nullptr), _pool(4)
{
	initialize("./ini/BotSetting.ini");
	watchParserEventBus();
	_configWatcherThread = std::thread(&QQBot::watchConfigHotReload, this, "./ini", "BotSetting.ini");
}

QQBot::QQBot(std::string ConfigPath) 
	: _groupName(""), _mainGroup(0), _msgCenter(0), _symbol(""), _botConfig(nullptr), _pool(4)
{
	initialize(ConfigPath);
	watchParserEventBus();
	fs::path p(ConfigPath);
	std::string dir = p.parent_path().string();
	std::string filename = p.filename().string();
	_configWatcherThread = std::thread(&QQBot::watchConfigHotReload, this, dir, filename);
}

QQBot::~QQBot()
{
	_watchConfigStopFlag = true;
	if (_configWatcherThread.joinable())
		_configWatcherThread.join();
	_executor.reset();
	stop();
}

void QQBot::watchParserEventBus() 
{
	EventBusInstance::instance().setExecutor([this](std::function<void()> job) {
		_pool.enqueue(std::move(job));
	});

	EventBusInstance::instance().asyncSubscribe<WindowLostEvent>(
		[this](const WindowLostEvent& event) {
			LOG_ERROR_SELF("Window lost from: " + event.componentName);
			LOG_SUCCESS_SELF(" Async reinitialization task started.");
			waitGroup();
			_executor->SetHWDN(_mainGroup);
			LOG_SUCCESS_SELF("Async reinitialization task completed.");
		});

	EventBusInstance::instance().subscribe<FetchMessageEvent>(
		[this](const FetchMessageEvent& event) {
			if (_executor)
			{
				_executor->push(event.msg);
			}
			else
			{
				LOG_ERROR_SELF("Executor is not initialized.");
			}
		});
}

void QQBot::initialize(const std::string& configPath) {
	_botConfig = std::make_unique<WinInIWrapper>(configPath);
	if (!_botConfig->IsValid()) 
	{
		LOG_ERROR_SELF("BotConfig is not valid, please check the config file path.");
		exit(EXIT_FAILURE);
	}
	if (_executor == nullptr)
	{
		_executor = std::make_unique<Executor<QMessage>>();
		LOG_SUCCESS_SELF("Executor initialized.");
	}
	else
	{
		LOG_WARNING_SELF("Executor already initialized, skipping reinitialization.");
	}
	readBotConfig();
}

bool QQBot::waitGroup()
{
	std::string tempg;
	std::string tempc;

	LOG_SUCCESS_SELF("Attach QQGroup : " + _groupName);

	while (true)
	{
		bool mainGroupReady = false;
		bool msgCenterReady = false;
		_mainGroup = FindWindow(_T("TXGuiFoundation"), multi_Byte_To_Wide_Char(_groupName));
		_msgCenter = FindWindow(_T("TXGuiFoundation"), _T("消息管理器"));

		mainGroupReady = (_mainGroup != 0);
		tempg = mainGroupReady ? "Ready" : "Not Ready";

		msgCenterReady = (_msgCenter != 0);
		tempc = msgCenterReady ? "Ready" : "Not Ready";

		char buffer[128];
		snprintf(buffer, sizeof(buffer), "Group: %-7s|MessageCenter: %-7s", tempg.c_str(), tempc.c_str());
		Botlog::GetInstance()->cmdlog_inline(Botlog::LEVEL_SUCCESS, Botlog::OWNER_SELF, std::string(buffer));

		if (mainGroupReady && msgCenterReady)
		{
			SetForegroundWindow(_mainGroup);
			std::cout << std::endl;
			LOG_SUCCESS_SELF("---Bot online---");
			_sender = std::make_unique<QQMessageSender>(_mainGroup);
			return true;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return false;
}

void QQBot::setup()
{
	waitGroup();
}

void QQBot::callbackAction(PythonCallbackInfo& event)
{
	LOG_SUCCESS_SELF("send to QQ :" + event.HttpBody);
	_sender->sendMessageAsJson(event.HttpBody, event.callInfo);
}

void QQBot::watchConfigHotReload(const std::string& dir, const std::string& filename)
{
	std::wstring wdir = std::wstring(dir.begin(), dir.end());
	std::wstring wfile = std::wstring(filename.begin(), filename.end());

	HANDLE hDir = CreateFileW(
		wdir.c_str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		nullptr
	);

	if (hDir == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR_SELF("Failed to open directory handle for config watch.");
		return;
	}

	char buffer[1024];
	DWORD bytesReturned;

	while (!_watchConfigStopFlag)
	{
		if (ReadDirectoryChangesW(
			hDir,
			buffer,
			sizeof(buffer),
			FALSE,
			FILE_NOTIFY_CHANGE_LAST_WRITE,
			&bytesReturned,
			nullptr,
			nullptr))
		{
			FILE_NOTIFY_INFORMATION* notify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
			std::wstring changedFile(notify->FileName, notify->FileNameLength / sizeof(WCHAR));
			if (changedFile == wfile)
			{
				LOG_SUCCESS_SELF("HotReload");
				readBotConfig();
				waitGroup();
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}

	CloseHandle(hDir);
}


bool QQBot::readBotConfig()
{
	std::string gid = _botConfig->FindValueA<std::string>("Group", "name");
	_symbol = _botConfig->FindValueA<std::string>("mark", "Symbol");

	if (gid != std::string())
	{
		_groupName = gid; // Assign the actual string value
		LOG_SUCCESS_SELF("Successfully read Group name: " + _groupName);
		return true;
	}
	else
	{
		LOG_ERROR_SELF("Failed to read 'name' from 'Group' section. Config file might be missing or malformed.");
		_groupName = ""; // Or some default/empty string if it's crucial that it's initialized
		return false;
	}
}


