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
		unsigned int pId;
		unsigned int heartbeat_port;
		std::string task_uuid;
		short status;
		std::string reportUrl;
		PyReturnType returnType;
		QMessage callInfo;
	}ServiceCallbackInfo;
	//typedef ServiceCallbackInfo PSCBI;
public:
    virtual ~ITaskRunner() = default;
    virtual std::optional<ServiceCallbackInfo> run(Task task) = 0;
};