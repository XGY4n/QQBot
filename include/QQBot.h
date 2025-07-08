#pragma once
#include <WinInIWrapper.h>
#include <string>
#include <Botlog.h>
#include <ChineseString.h>
#include <optional>
#include <Parser.h>
#include <factories/ParserFactory.h>
#include <Executor.h>
#include <Bus/EventBusInstance.h>
class QQBot {
public:
    QQBot();
    QQBot(std::string Configpath);
    ~QQBot();
    void run(); 
    void stop(); 
    WinInIWrapper *BotConfig;
    HWND GetMainGroup() const { return _mainGroup; }

private:
    bool WaitGroup();
    void setupMessageProcessingPipeline(); 
    void setupExecutorPipeline();
    bool ReadBotConfig();
    void watchEventBus();

private:
    Botlog* _logger = Botlog::GetInstance();
    std::string _groupName;
    HWND _mainGroup;
	HWND _msgCenter;
    std::string _symbol;
    std::unique_ptr<Parser> _parser;
	std::unique_ptr<Executor<QMessage>> _executor;
};