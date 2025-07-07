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
#include <interface/IQQMessageExporter.h>
#include <functional>
class Parser 
{
	public:
		Parser();
		Parser(//DI
			std::unique_ptr<IQMsgFormatter> formatter,
			std::unique_ptr<IUIAWindowController> windowController,
			std::unique_ptr<IMessageFetcher> fetcher,
			std::string Symbol = "#"
		);

		~Parser();

		using MessageCallback = std::function<void(const QMessage)>;

		std::string GetmarkSymbol() const { return _markSymbol; }
		void start();
		void stop();
		using MessageCallback = std::function<void(const QMessage)>;
		void SetMessageCallback(MessageCallback cb);
	private:
		bool SetParserMarkSymbol(std::string Symbols);
		bool SetParserMarkSymbol(char Symbol);
		void OnFetcherMessage(const QMessage msg);

	public:
	private:
		Botlog* _logger = Botlog::GetInstance();
		std::unique_ptr<IQMsgFormatter> _formatter;
		std::unique_ptr<IUIAWindowController> _windowController;
		std::unique_ptr<IMessageFetcher> _fetcher;
		std::string _markSymbol;
		MessageCallback _msgCallback;

};