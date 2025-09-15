#pragma once
#include <string>
//#include <UIACliWrapper.h>
#include <UIAQQMessageExporter.h>
#include <Botlog.h>
#include <tchar.h>
#include <ChineseString.h>
#include "QMsgFormatter.h"
#include "UIAWindowController.h"
#include "MessageFetcher.h"
#include <interface/IMessageExporter.h>
#include <functional>
class Parser 
{
	public:

		Parser(//DI
			std::unique_ptr<IMsgFormatter> formatter,
			std::unique_ptr<IUIAWindowController> windowController,
			std::unique_ptr<IMessageFetcher> fetcher,
			std::string Symbol = "#"
		);
		Parser(//DI
			std::unique_ptr<IMessageFetcher> fetcher,
			std::string Symbol = "#"
		);
		~Parser();

		std::string GetmarkSymbol() const { return _markSymbol; }
		void start();
		void stop();

	private:
		bool SetParserMarkSymbol(std::string Symbols);
		bool SetParserMarkSymbol(char Symbol);

	public:
	private:
		std::unique_ptr<IMsgFormatter> _formatter;
		std::unique_ptr<IUIAWindowController> _windowController;
		std::unique_ptr<IMessageFetcher> _fetcher;
		std::string _markSymbol;
};