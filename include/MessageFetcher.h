#pragma once
#include <UIAQQMessageExporter.h>
#include <QMsgFormatter.h>
#include <thread>
#include <Botlog.h>
#include <UIAWindowController.h>
#include <interface/IMessageFetcher.h>

class MessageFetcher: public IMessageFetcher 
{
public:
    MessageFetcher(IQMsgFormatter* formatter, IUIAWindowController* windowController);
    MessageFetcher(std::unique_ptr<IQMsgFormatter> formatter, std::unique_ptr <IUIAWindowController> windowController, std::unique_ptr<IQQMessageExporter> MessageExporterm);
    ~MessageFetcher();

    void start();
    void stop();

    bool SetParserMarkSymbol(std::string Symbols);


private:
    bool MessageFetcher::isValidMessage(const QMessage& msg);
    void MessageFetcher::processMessage(QMessage& msg);
    void fetchLoop();
    void handleHeartbeat(const std::string& current, const std::string& previous, int& timer);
    bool tryParse(std::string& raw, QMessage& QQMessage);
    void Refresh();

private:
    std::unique_ptr<IQQMessageExporter> _msgExporter;
    std::unique_ptr<IQMsgFormatter> _formatter;
    std::unique_ptr<IUIAWindowController> _windowController;

    //log
    Botlog* _logger = Botlog::GetInstance();

    //thread
    std::thread __fetcherThread;
    std::thread __refThread;
    std::atomic<bool> _fetcherRunning{ false };   

    //else
    HWND _automationWindowHandle; 
    std::string _symbol;
};