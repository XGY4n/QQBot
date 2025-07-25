#include <iostream>
#include <string>
#include <QQBot.h>
#include <QQMessageSender.h>
int main(int argc, char* argv[])
{

    QQBot* bot = nullptr;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " ./ini/BotSetting.ini" << std::endl;
        bot = new QQBot(); 
    }
    else {
        std::string config_file = argv[1];
        try {
            bot = new QQBot(config_file); 
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to initialize QQBot with config: " << e.what() << std::endl;
            return 1;
        } 
    }

    if (bot) {
        bot->run();
        delete bot;
    }

    return 0;
}
