#include <factories/ParserFactory.h>

std::unique_ptr<Parser> ParserFactory::Create(HWND qmsgWnd, std::string Symbol)
{
    auto formatter = std::make_unique<QMsgFormatter>();
    auto windowController = std::make_unique<UIAWindowController>(qmsgWnd);
    auto messageExporter = std::make_unique<UIAQQMessageExporter>(); 

    auto fetcher = std::make_unique<MessageFetcher>(
        std::move(formatter),
        std::move(windowController),
		std::move(messageExporter)
    );
    if (Symbol != "")
    {
        fetcher->SetParserMarkSymbol(Symbol);
        return std::make_unique<Parser>(
            std::move(fetcher), Symbol
        );
    }
    fetcher->SetParserMarkSymbol("#");
    return std::make_unique<Parser>(
        std::move(fetcher),
        Symbol
    );
}
