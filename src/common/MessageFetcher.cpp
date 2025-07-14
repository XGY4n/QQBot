#include <MessageFetcher.h>

MessageFetcher::MessageFetcher(IQMsgFormatter* formatter, IUIAWindowController* windowController)
    : _formatter(formatter), _windowController(windowController)
{
	//_uia = new UIAQQMessageExporter();
    _hwnd = FindWindow(_T("TXGuiFoundation"), _T("消息管理器"));

    if (_msgExporter == NULL || !_hwnd || !_formatter) {
        _logger->LOG_ERROR_SELF("MessageFetcher init error");
    }

}

bool MessageFetcher::SetParserMarkSymbol(std::string Symbols)
{
    _symbol = Symbols;
    return true;
}

//MessageFetcher::MessageFetcher(IQMsgFormatter* formatter, IUIAWindowController* windowController, std::unique_ptr<IQQMessageExporter> MessageExporterm)
MessageFetcher::MessageFetcher(std::unique_ptr<IQMsgFormatter> formatter, std::unique_ptr <IUIAWindowController> windowController, std::unique_ptr<IQQMessageExporter> MessageExporterm)
	: _formatter(std::move(formatter)), _windowController(std::move(windowController)), _msgExporter(std::move(MessageExporterm))
{
    _hwnd = FindWindow(_T("TXGuiFoundation"), _T("消息管理器"));

    if (!_msgExporter || !_hwnd || !_formatter) {
        _logger->LOG_ERROR_SELF("MessageFetcher init error");
    }

}

MessageFetcher::~MessageFetcher() 
{
    stop();
}

void MessageFetcher::start()
{
    if (_fetcherRunning) return;
    _fetcherRunning = true;
    __refThread = std::thread(&MessageFetcher::Refresh, this);
    //__refThread.detach();
    __fetcherThread = std::thread(&MessageFetcher::fetchLoop, this);
    //__fetcherThread.detach();
    _logger->LOG_SUCCESS_SELF("MessageFetcher start...");

}

void MessageFetcher::stop()
{
    if (_fetcherRunning)
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

void MessageFetcher::Refresh()
{
    while (_fetcherRunning)
    {
        if (!_windowController->RefreshMsg())
        {
            EventBusInstance::instance().publish(WindowLostEvent{ std::to_string(int(_hwnd)) });
            break;
        }
        _msgExporter->GetQQMessages();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
        timer = 0;

    if (timer == 1000)
        _logger->filelog(Botlog::LEVEL_SUCCESS, Botlog::OWNER_SELF, "Heart Beat");
    else if (timer > 1000)
        timer = 1000;
    //Sleep(timer);
}

bool MessageFetcher::tryParse(std::string& raw, std::string& out, std::string& lastTime)
{
    try 
    {
        _QQMessage = _formatter->parse(raw);
        out = "[" + _QQMessage.DataTime + "]" + _QQMessage.name + "(" + _QQMessage.QQNumber + ")" + _QQMessage.message;
        if (!_QQMessage.message.empty() && !_symbol.empty() && _QQMessage.message[0] == _symbol[0]) 
        {
            _logger->Record(Botlog::Type_Message, Botlog::LEVEL_SUCCESS, Botlog::OWNER_USERCALL, _QQMessage.toString());

            _QQMessage.message = _QQMessage.message.substr(1);
            OnNewMessage(_QQMessage);
            return true;
        }
        _logger->Record(Botlog::Type_Message, Botlog::LEVEL_SUCCESS, Botlog::OWNER_SELF, _QQMessage.toString());

        return true;
    }
    catch (const std::exception& e) 
    {
        out = raw;
        if (raw != lastTime) {
            lastTime = raw;
            _logger->filelog(Botlog::LEVEL_WARNIGN, Botlog::OWNER_SELF, e.what());
        }
        return false;
    }
}
bool MessageFetcher::shouldLogAndContinue(std::string& raw, std::string& lastRaw)
{
    if (raw != lastRaw)
    {
        return true; 
    }
    return false;
}
void MessageFetcher::fetchLoop()
{
    std::string ans;
    std::string lastRaw;
    std::string lastTime;
    std::vector<std::string > msgWindows;
    int timer = 0;
    while (_fetcherRunning) 
    {
        std::string raw = _msgExporter->GetQQMessage();
        if (raw == "N/A")
        {
            continue;
        }
        //handleHeartbeat(raw, lastRaw, timer);
        if (!shouldLogAndContinue(raw, lastRaw))
            continue;
        std::string msg;

        if (!tryParse(raw, msg, lastTime)) 
        {
			_logger->LOG_WARNING_SELF("Failed to parse message: " + raw + "Fallback parse used");
        }

        lastRaw = raw;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}