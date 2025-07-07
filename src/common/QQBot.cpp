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
	std::string tempg = "GroupWindow";
	std::string tempc = "MessageCenter";

	while (true)
	{
		_logger->LOG_SUCCESS_SELF("Attch QQGroup : " + _groupName);

		_mainGroup = FindWindow(_T("TXGuiFoundation"), multi_Byte_To_Wide_Char(_groupName));
		if (_mainGroup != 0)
		{
			tempg = "";
		}

		_msgCenter = FindWindow(_T("TXGuiFoundation"), _T("消息管理器"));
		if (_msgCenter != 0)
		{
			tempc = "";
		}

		if (_mainGroup != 0 && _msgCenter != 0)
		{
			SetForegroundWindow(_mainGroup);
			_logger->LOG_SUCCESS_SELF(std::string("---Bot online---"));
			Sleep(1000);
			return true;
		}

		_logger->LOG_SUCCESS_SELF("waiting - " + tempg + " " + tempc);
		Sleep(1000);
		system("cls");
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
