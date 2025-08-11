#include <Parser.h>

//Parser::Parser()
//{
//    _qmsgwind = FindWindow(_T("TXGuiFoundation"), _T("消息管理器"));
//    if (_qmsgwind == 0) {
//        if (_logger) {
//            LOG_ERROR_SELF("Failed to find message window. Aborting Parser construction.");
//        }
//        throw std::runtime_error("Parser: Message window not found.");
//    }
//    if (_logger) {
//        LOG_SUCCESS_SELF("Message window handle: " + std::to_string(reinterpret_cast<uintptr_t>(_qmsgwind)));
//    }
//
//    _markSymbol = "#";
//
//    try {
//        _formatter = std::make_unique<QMsgFormatter>();
//        _windowController = std::make_unique<UIAWindowController>(_qmsgwind);
//        _fetcher = std::make_unique<MessageFetcher>(_formatter.get(), _windowController.get());
//
//    }
//    catch (const std::bad_alloc& e) {
//        if (_logger) {
//            LOG_ERROR_SELF("Failed to allocate memory for a component: " + std::string(e.what()));
//        }
//        throw std::runtime_error("Parser: Memory allocation failed during component initialization.");
//    }
//    catch (const std::exception& e) {
//        if (_logger) {
//            LOG_ERROR_SELF("Error during component initialization: " + std::string(e.what()));
//        }
//        throw;
//    }
//
//    if (_windowController) {
//        _windowController->MoveWindowOffScreen();
//    }
//    else {
//        if (_logger) {
//            LOG_ERROR_SELF("UIAWindowController was unexpectedly null after initialization.");
//        }
//        throw std::runtime_error("Parser: UIAWindowController uninitialized.");
//    }
//
//    if (_logger) {
//        LOG_SUCCESS_SELF("Parser constructed successfully.");
//    }
//}
Parser::Parser(//DI
    std::unique_ptr<IMessageFetcher> fetcher,
    std::string Symbol 
)  :  _fetcher(std::move(fetcher))
{
    if (!_fetcher) {
        throw std::invalid_argument("Parser: IMessageFetcher is null.");
    }

    // 检查 Symbol 是否为空
    if (_markSymbol.empty()) {
        LOG_ERROR_SELF("Parser: Symbol is empty.");
        return;
    }
    LOG_SUCCESS_SELF("Parser constructed successfully (DI).");

}


Parser::Parser(
    std::unique_ptr<IQMsgFormatter> formatter,
    std::unique_ptr<IUIAWindowController> windowController,
    std::unique_ptr<IMessageFetcher> fetcher,
	std::string Symbol 
)
    : _formatter(std::move(formatter)),
    _windowController(std::move(windowController)),
    _fetcher(std::move(fetcher)),
	_markSymbol(Symbol)
{
    // 检查依赖项是否为 nullptr
    if (!_formatter) {
        throw std::invalid_argument("Parser: IQMsgFormatter is null.");
    }

    if (!_windowController) {
        LOG_ERROR_SELF("UIAWindowController is null after DI initialization.");
        throw std::invalid_argument("Parser: IUIAWindowController is null.");
    }

    if (!_fetcher) {
        throw std::invalid_argument("Parser: IMessageFetcher is null.");
    }

    // 检查 Symbol 是否为空
    if (_markSymbol.empty()) {
        LOG_ERROR_SELF("Parser: Symbol is empty.");
		throw std::invalid_argument("Parser: Symbol is empty.");
    }

    try {
#ifndef _DEBUG
        _windowController->MoveWindowOffScreen();
#endif    
    }
    catch (const std::exception& e) {
        LOG_ERROR_SELF("Exception in MoveWindowOffScreen: %s", e.what());
        throw; 
    }

    LOG_SUCCESS_SELF("Parser constructed successfully (DI).");

}

Parser::~Parser()
{
    stop();
}

bool Parser::SetParserMarkSymbol(std::string Symbols)
{
	return true;
}

bool Parser::SetParserMarkSymbol(char Symbol)
{
	return true;
}

void Parser::start()
{

    if (!_fetcher)
    {
        LOG_ERROR_SELF("Parser::start() called with uninitialized _fetcher. Aborting start operation.");
        throw std::runtime_error("Parser::start(): MessageFetcher is not initialized.");
    }

    try 
    {
        _fetcher->start();
        LOG_SUCCESS_SELF("Message fetching started successfully.");
    }
    catch (const std::exception& e)
    {
        LOG_ERROR_SELF("Error starting message fetcher: " + std::string(e.what()));
        throw; 
    }
    catch (...) 
    {
        LOG_ERROR_SELF("An unknown error occurred while starting message fetcher.");
        throw;
    }
}

void Parser::stop()
{
    _fetcher->stop();
    LOG_SUCCESS_SELF("fetcher stop done");
}

