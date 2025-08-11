#include <interface/ITaskRunner.h>

class PythonTaskRunner : public ITaskRunner {
public:
	std::map<std::string, PyReturnType> ReturnTypeMap = {
		{"str", STRING},
		{"wstr", WSTRING},
		{"file", FILE},
		{"QImage",QIMAGE},
		{"Auto", AUTO},
		{"undefine", UNKNOWN},
	};
public:
	PythonTaskRunner() ;
	~PythonTaskRunner() override ;
	std::optional<PythonTaskRunner::ServiceCallbackInfo> run(Task task) override;
private : 
	ITaskRunner::ServiceCallbackInfo BuildTCB(const char* TaskCallBackJsonstr, Task task);
private :
	std::wstring _pythonHomeW;
	std::string _pythonHomeA;

	WinInIWrapper* _PyenvCfg = nullptr;
	void ReadPythonEnvConfig();
	ITaskRunner::ServiceCallbackInfo _tcb;
};
