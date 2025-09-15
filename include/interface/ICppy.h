#pragma once
#include <string>
#include <memory>
#include "IMessageFetcher.h"

template<typename MsgT>
class ICPPy
{
public:
    virtual ~ICPPy() = default;

    virtual void start()
    {
        setup();  
        run(); 
    }
    virtual std::unique_ptr<IMessageFetcher> createMessageFetcher() = 0;
    virtual void setup() {}
	virtual void callbackAction(PythonCallbackInfo &event) {}
protected:
    void watchBus()
    {
        EventBusInstance::instance().subscribe<PythonCallbackInfo>(
            [this](const PythonCallbackInfo& event) {
                callbackAction(event);
            });
    }

    virtual void run()
    {
        setupPipeline();
        startPipeline();
    }

    void stop()
    {
        if (_fetcher) {
            _fetcher->stop();
        }
    }

protected:
    std::unique_ptr<IMessageFetcher> _fetcher;
    std::unique_ptr<Executor<MsgT>> _executor;

    void setupPipeline()
    {
        _fetcher = createMessageFetcher();
        if (!_executor)
        {
			_executor = std::make_unique<Executor<MsgT>>();
            _executor->start();
        }
        else
        {
            LOG_WARNING_SELF("Executor already initialized, skipping reinitialization.");
        }
    }

    void startPipeline()
    {
        if (_fetcher) {
            _fetcher->start();
        }
        if (_executor) {
            _executor->start();
        }
    }
};
