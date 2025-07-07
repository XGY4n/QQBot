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
	ITaskRunner::ServiceCallbackInfo GetRunningTaskCallBack();
private : 
	ITaskRunner::ServiceCallbackInfo BuildTCB(const char* TaskCallBackJsonstr);
private :
	Botlog* _logger = Botlog::GetInstance();

	WinInIWrapper* _PyenvCfg = nullptr;
	void ReadPythonEnvConfig();
	ITaskRunner::ServiceCallbackInfo _tcb;
};
