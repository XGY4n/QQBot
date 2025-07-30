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
#include <future>
#include <ThreadPool.h>

class QQBot {
public:
    QQBot();
    QQBot(std::string Configpath);
    ~QQBot();
    void run(); 
    void stop(); 
    HWND GetMainGroup() const { return _mainGroup; }

private:
    bool waitGroup();
    void setupMessageProcessingPipeline(); 
    void setupExecutorPipeline();
    bool readBotConfig();
    void watchParserEventBus();
    void initialize(const std::string& configPath);
    void watchConfigHotReload(const std::string& dir, const std::string& filename);

private:
    Botlog* _logger = Botlog::GetInstance();
    std::string _groupName;
    HWND _mainGroup;
	HWND _msgCenter;
    std::string _symbol;
    std::unique_ptr<Parser> _parser;
	std::unique_ptr<Executor<QMessage>> _executor;
    std::unique_ptr<WinInIWrapper> _botConfig;
    ThreadPool _pool;
    std::atomic<bool> _watchConfigStopFlag{ false };
    std::thread _configWatcherThread;
};