#include <Parser.h>

Parser::Parser(//DI
    std::unique_ptr<IMessageFetcher> fetcher,
    std::string Symbol 
) : _fetcher(std::move(fetcher)), _markSymbol(Symbol)
{
    if (!_fetcher) {
        throw std::invalid_argument("Parser: IMessageFetcher is null.");
    }
    if (_markSymbol.empty()) {
        LOG_ERROR_SELF("Parser: Symbol is empty.");
        throw std::invalid_argument("Parser: Symbol is empty.");
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

