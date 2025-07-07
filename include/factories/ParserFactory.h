#pragma once
#include <memory>
#include <Parser.h>
#include <QMsgFormatter.h>
#include <UIAWindowController.h>
#include <MessageFetcher.h>
#include <UIAQQMessageExporter.h>

class ParserFactory 
{
public:
    static std::unique_ptr<Parser> Create(HWND qmsgWnd, std::string Symbol);
};