#include <MessageFetcher.h>

MessageFetcher::MessageFetcher(IQMsgFormatter* formatter, IUIAWindowController* windowController)
    : _formatter(formatter), _windowController(windowController)
{
    _automationWindowHandle = FindWindow(_T("TXGuiFoundation"), _T("消息管理器"));

    if (_msgExporter == NULL || !_automationWindowHandle || !_formatter) {
        _logger->LOG_ERROR_SELF("MessageFetcher init error");
    }

}

bool MessageFetcher::SetParserMarkSymbol(std::string Symbols)
{
    _symbol = Symbols;
    return true;
}

MessageFetcher::MessageFetcher(std::unique_ptr<IQMsgFormatter> formatter, std::unique_ptr <IUIAWindowController> windowController, 
    std::unique_ptr<IQQMessageExporter> MessageExporterm)
	: _formatter(std::move(formatter)), _windowController(std::move(windowController)), _msgExporter(std::move(MessageExporterm))
{
    _automationWindowHandle = FindWindow(_T("TXGuiFoundation"), _T("消息管理器"));

    if (!_msgExporter || !_automationWindowHandle || !_formatter) {
        _logger->LOG_ERROR_SELF("MessageFetcher init error");
    }

}

MessageFetcher::~MessageFetcher() 
{
    stop();
}

void MessageFetcher::start()
{
    if (_fetcherRunning.load()) return;
    _fetcherRunning = true;
    __refThread = std::thread(&MessageFetcher::Refresh, this);
    __fetcherThread = std::thread(&MessageFetcher::fetchLoop, this);
    _logger->LOG_SUCCESS_SELF("MessageFetcher start...");

}

void MessageFetcher::stop()
{
    if (_fetcherRunning.load())
    {
        _fetcherRunning = false;
        if (__fetcherThread.joinable()) {
            __fetcherThread.join();
        }
        if (__refThread.joinable()) {
            __refThread.join();
        }        
        _formatter.reset();
        _windowController.reset();
        _msgExporter.reset();
        _logger->LOG_SUCCESS_SELF("MessageFetcher stop");
    }
}
 
void MessageFetcher::OnNewMessage(const QMessage msg) {
    if (_callback) {
        _callback(msg); 
    }
}

void MessageFetcher::handleHeartbeat(const std::string& current, const std::string& previous, int& timer) 
{
    if (current == previous)
        timer += 100;
    else
        timer = 300;

    if (timer == 800)
        _logger->filelog(Botlog::LEVEL_SUCCESS, Botlog::OWNER_SELF, "Heart Beat");
    else if (timer > 800)
        timer = 800;
    std::this_thread::sleep_for(std::chrono::milliseconds(timer));
}

bool MessageFetcher::tryParse(std::string& raw, QMessage& QQMessage)
{
    try 
    {
        QQMessage = _formatter->parse(raw);
        return true;
    }
    catch (const std::exception& e) 
    {
        _logger->Record(Botlog::LEVEL_WARNIGN, Botlog::OWNER_SELF, e.what());
        return false;
    }
}

bool MessageFetcher::isValidMessage(const QMessage& msg)
{
    return !msg.message.empty() && !_symbol.empty() && msg.message[0] == _symbol[0];
}

void MessageFetcher::processMessage(QMessage& msg)
{
    if (isValidMessage(msg))
    {
        msg.message = std::move(msg.message.substr(1));
        OnNewMessage(msg);
        _logger->Record(Botlog::Type_Message, Botlog::LEVEL_SUCCESS, Botlog::OWNER_USERCALL, msg.toString());
        return;
    }
    _logger->Record(Botlog::Type_Message, Botlog::LEVEL_SUCCESS, Botlog::OWNER_SELF, msg.toString());
}

void MessageFetcher::fetchLoop()
{
    while (_fetcherRunning.load())
    {
        QMessage QQMessage;
        std::string raw = std::move(_msgExporter->GetQQMessage());
        if (!tryParse(raw, QQMessage))
        {
			_logger->LOG_WARNING_SELF("Failed to parse message: " + raw + "Fallback parse used");
			continue;
        }
        processMessage(QQMessage);
    }
}

void MessageFetcher::Refresh()
{
    int t = 0;
    while (_fetcherRunning.load())
    {
        if (!_windowController->RefreshMsg())
        {
            EventBusInstance::instance().publish(WindowLostEvent{ std::to_string(int(_automationWindowHandle)) });
            break;
        }
        _msgExporter->GetQQMessages();
        
        handleHeartbeat("", "", t);
    }
}
