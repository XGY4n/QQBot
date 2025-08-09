#include <TaskBuilder.h>

class ITaskRunner 
{
public:
	enum PyReturnType
	{
		STRING,
		WSTRING,
		FILE,
		QIMAGE,
		AUTO,
		UNKNOWN
	};
public :
	typedef struct PythonServiceCallbackInfo
	{
		unsigned int pId = 0;
		unsigned int heartbeat_port = 11451;
		std::string task_uuid;
		short status = 0;
		std::string reportUrl;
		PyReturnType returnType;
		QMessage callInfo;
		Task taskcallback;
	}ServiceCallbackInfo;
public:
    virtual ~ITaskRunner() = default;
    virtual std::optional<ServiceCallbackInfo> run(Task task) = 0;
};