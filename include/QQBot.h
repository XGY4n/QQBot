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
#include <interface/ICppy.h>
#include <MessageFetcher.h>
class QQBot : public ICPPy<QMessage>
{
public:
	QQBot();
    QQBot(std::string Configpath);
    ~QQBot();
	std::unique_ptr<IMessageFetcher> createMessageFetcher()
	{
        auto formatter = std::make_unique<QMsgFormatter>();
        auto windowController = std::make_unique<UIAWindowController>(_msgCenter);
        auto messageExporter = std::make_unique<UIAQQMessageExporter>();

        auto fetcher = std::make_unique<MessageFetcher>(
            std::move(formatter),
            std::move(windowController),
            std::move(messageExporter)
        );
		fetcher->SetParserMarkSymbol(_symbol);
        return fetcher;
	}
private:
    bool waitGroup();
    bool readBotConfig();
    void watchParserEventBus();
    void initialize(const std::string& configPath);
    void watchConfigHotReload(const std::string& dir, const std::string& filename);
    void setup() override;
    void callbackAction(PythonCallbackInfo& event) override;
private:
    std::string _groupName;
    HWND _mainGroup;
	HWND _msgCenter;
    std::string _symbol;
    std::unique_ptr<WinInIWrapper> _botConfig;
    std::unique_ptr<QQMessageSender> _sender;
    ThreadPool _pool;
    std::atomic<bool> _watchConfigStopFlag{ false };
    std::thread _configWatcherThread;
};