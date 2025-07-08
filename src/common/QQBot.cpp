#include <QQBot.h>


QQBot::QQBot()
	: _groupName(""), _mainGroup(0), _msgCenter(0), _symbol("")
{
	BotConfig = new WinInIWrapper("./ini/BotSetting.ini");
	if (!BotConfig->IsValid())
	{
		_logger->LOG_ERROR_SELF("BotConfig is not valid, please check the config file path.");
		exit(EXIT_FAILURE);
	}
	_executor = std::make_unique<Executor<QMessage>>();
	ReadBotConfig();
}

QQBot::QQBot(std::string Configpath)
	: _groupName(""), _mainGroup(0), _msgCenter(0), _symbol("")
{
	BotConfig = new WinInIWrapper(Configpath);
	if (!BotConfig->IsValid())
	{
		_logger->LOG_ERROR_SELF( "BotConfig is not valid, please check the config file path.");
		exit(EXIT_FAILURE);
	}
	ReadBotConfig();

}

bool QQBot::WaitGroup()
{
	std::string tempg = "waiting";
	std::string tempc = "waiting";

	_logger->LOG_SUCCESS_SELF("Attach QQGroup : " + _groupName);

	while (true)
	{
		bool mainGroupReady = false;
		bool msgCenterReady = false;

		if (_mainGroup != 0)
		{
			mainGroupReady = true;
			tempg = "Ready";
		}

		_msgCenter = FindWindow(_T("TXGuiFoundation"), _T("消息管理器"));
		if (_msgCenter != 0)
		{
			msgCenterReady = true;
			tempc = "Ready";
		}

		char buffer[128];
		snprintf(buffer, sizeof(buffer), "Group: %-7s|MessageCenter: %-7s", tempg.c_str(), tempc.c_str());
		_logger->cmdlog_inline(Botlog::LEVEL_SUCCESS, Botlog::OWNER_USERCALL, std::string(buffer));
		_mainGroup = FindWindow(_T("TXGuiFoundation"), multi_Byte_To_Wide_Char(_groupName));

		if (mainGroupReady && msgCenterReady)
		{
			SetForegroundWindow(_mainGroup);
			std::cout << std::endl;
			_logger->LOG_SUCCESS_SELF("---Bot online---");
			return true;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return false;
}


void QQBot::run()
{
	setupMessageProcessingPipeline();
	setupExecutorPipeline();
}

QQBot::~QQBot()
{

}

void QQBot::stop()
{
	_logger->LOG_SUCCESS_SELF("Stopping QQBot...");
	if (BotConfig) {
		delete BotConfig;
		BotConfig = nullptr;
	}
	_logger->LOG_SUCCESS_SELF("QQBot stopped.");
	exit(0);
}

bool QQBot::ReadBotConfig()
{
	std::string gid = BotConfig->FindValueA<std::string>("Group", "name");
	_symbol = BotConfig->FindValueA<std::string>("mark", "Symbol");

	if (gid != std::string())
	{
		_groupName = gid; // Assign the actual string value
		_logger->LOG_SUCCESS_SELF("Successfully read Group name: " + _groupName);
		return true;
	}
	else
	{
		_logger->LOG_ERROR_SELF("Failed to read 'name' from 'Group' section. Config file might be missing or malformed.");
		_groupName = ""; // Or some default/empty string if it's crucial that it's initialized
		return false;
	}
}

void QQBot::setupMessageProcessingPipeline()
{
	if (WaitGroup())
	{
		_msgParser = ParserFactory::Create(_msgCenter, _symbol);
		_msgParser->SetMessageCallback([this](const QMessage msg)
		{
			if (_executor) 
			{
				_executor->push(msg);
			}
			else 
			{
				_logger->LOG_ERROR_SELF("Executor is not initialized.");
			}
		});
		_msgParser->start();
	}
}

void QQBot::setupExecutorPipeline()
{
	if (_executor) 
	{
		_executor->SetHWDN(_mainGroup);
		_executor->start();
	}
	else 
	{
		_logger->LOG_ERROR_SELF("Executor is not initialized.");
	}
}
