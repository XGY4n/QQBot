#pragma once
#include <UIAQQMessageExporter.h>
#include <QMsgFormatter.h>
#include <thread>
#include <Botlog.h>
#include <UIAWindowController.h>
#include <interface/IMessageFetcher.h>
class MessageFetcher: public IMessageFetcher {
public:
    MessageFetcher(IQMsgFormatter* formatter, IUIAWindowController* windowController);
    MessageFetcher(IQMsgFormatter* formatter, IUIAWindowController* windowController, std::unique_ptr<IQQMessageExporter> messageExporter); 

    ~MessageFetcher();

    void start();
    void stop();

    bool SetParserMarkSymbol(std::string Symbols);


    void SetMessageCallback(MessageCallback cb) override {
        _callback = std::move(cb);
    }
    void OnNewMessage(const QMessage msg);
private:
    bool shouldLogAndContinue(std::string& raw, std::string& lastRaw);
    void fetchLoop();
    void handleHeartbeat(const std::string& current, const std::string& previous, int& timer);
    bool tryParse(std::string& raw, std::string& out, std::string& lastTime);
    void Refresh();

private:
    std::unique_ptr<IQQMessageExporter> _msgExporter;  // �洢����Ȩ
    IQMsgFormatter* _formatter;
    IUIAWindowController* _windowController;

    Botlog* _logger = Botlog::GetInstance();

    std::thread __fetcherThread;
    std::thread __refThread;

    std::atomic<bool> _fetcherRunning{ false };
    MessageCallback _callback;
    HWND _hwnd; // ���ھ��
	QMessage _QQMessage; 
    std::string _symbol;
};