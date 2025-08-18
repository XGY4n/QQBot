import importlib
import sys
import json
import report_bridge as cppHttp_bridge
from http.server import HTTPServer, BaseHTTPRequestHandler
import threading
import os
import datetime
import Botlog as logger
import requests
import signal

TIMEOUT_SECONDS = 10
heartbeat_timer = None

def write_log(message: str):
    logger.ipc_print(message)
    
sdk_path = "./QQbot.py" 

if sdk_path not in sys.path:
    sys.path.insert(0, sdk_path)
    
def kill_self(task_uuid):
    write_log(f"超过 10 秒未收到 C++ GET 请求，退出进程 uuid : {task_uuid}")
    os.kill(os.getpid(), signal.SIGTERM)

def reset_timer(task_uuid):
    global heartbeat_timer
    if heartbeat_timer:
        heartbeat_timer.cancel()
    heartbeat_timer = threading.Timer(TIMEOUT_SECONDS, kill_self, args=(task_uuid,))
    heartbeat_timer.start()
    
def start_health_server(port, task_uuid):
    class HealthHandler(BaseHTTPRequestHandler):
        def do_GET(self):
            try:
                reset_timer(task_uuid)
                payload = {'uuid': task_uuid}
                resp = requests.post("http://127.0.0.1:11451/health", json=payload)
                
                self.send_response(resp.status_code)
                self.send_header("Content-type", "text/plain")
                self.end_headers()
                self.wfile.write(resp.content)
            except Exception as e:
                self.send_response(500)
                self.end_headers()
                self.wfile.write(f"Error: {e}".encode('utf-8'))
        def log_message(self, format, *args):
            pass
        
    reset_timer(task_uuid)
    HTTPServer(("127.0.0.1", port), HealthHandler).serve_forever() 

def main():
    
    global TIMEOUT_SECONDS

    write_log("into ""worker.py main()")

    args_json = sys.argv[1]
    write_log(f"接收到的 sys.argv[1] JSON 参数: {args_json}")

    args = json.loads(args_json)
    port = args["heartbeat_port"]
    task_args = args.get("task_args", {})
    function_to_call = args.get("function_name", "run")
    return_type = args.get("return_type", "str")
    report_url_for_sdk = args.get("report_url", {})
    Debug = args.get("Debug", {})
    if Debug == True:
        TIMEOUT_SECONDS = 300
        write_log(f"DEBUG been setting TimeOut set 5 min")

    # --- 新增：获取 task_uuid ---
    task_uuid = args.get("task_uuid") 
    write_log(f"获取到的 task_uuid: {task_uuid}")
    # ---------------------------

    if report_url_for_sdk:
        # 修改：初始化 bridge 时，如果 report_bridge 支持接收 task_uuid，可以在这里传递
        # 这取决于 report_bridge.init_bridge 的实现
        bridge = cppHttp_bridge.ReportBridge(report_url_for_sdk)

        write_log(f"初始化 QQBotSDK，报告 URL: {report_url_for_sdk}")
    else:
        write_log("未提供 report_url，未初始化 QQBotSDK")

    threading.Thread(target=start_health_server, args=(port,task_uuid,), daemon=True).start()
    write_log(f"启动心跳服务线程，端口: {port}")

    script_dir = os.path.dirname(args.get("script_path", {}))
    module_name = args.get("module", {})

    if script_dir not in sys.path:
        sys.path.insert(0, script_dir)
        write_log(f"将用户脚本目录添加到 sys.path: {script_dir}")

    try:
        module = importlib.import_module(module_name)
        write_log(f"成功导入用户模块: {module_name}")

        if hasattr(module, function_to_call) and callable(getattr(module, function_to_call)):
            target_function = getattr(module, function_to_call)
            task_result = target_function(task_args)
            write_log(f"用户模块 {function_to_call}() 返回结果: {task_result}")

            # --- 关键修改：在 report_result 中包含 task_uuid ---
            # 这里假设 report_bridge.report_result 可以接受一个字典或多个参数
            # 包含 task_uuid 和 task_result
            report_data = {
                "task_uuid": task_uuid,
                "result": task_result,
                "status": "success", # 可以添加状态信息
                "timestamp": datetime.datetime.now().isoformat(), # 添加时间戳
                "return_type" : return_type
            }
            bridge.report_result(report_data)
            write_log(f"已调用 cppHttp_bridge.report_result 上报结果，包含 UUID: {task_uuid}")
            # -----------------------------------------------------

            write_log(f"用户脚本 ({module_name}.py) 中函数 {function_to_call}() 返回的结果是: {task_result}")
        else:
            error_message = f"用户模块 {module_name} 中未找到函数或属性 '{function_to_call}'，或者它不可调用。"
            write_log(error_message)
            raise AttributeError(error_message)

    except Exception as e:
        write_log(f"执行用户脚本出错: {e}")
        # --- 错误时也上报 UUID ---
        error_report_data = {
            "task_uuid": task_uuid,
            "result": "Error: " + str(e),
            "status": "failed",
            "timestamp": datetime.datetime.now().isoformat(),
            "return_type": return_type
        }
        bridge.report_result(error_report_data)
        write_log(f"已上报错误结果，包含 UUID: {task_uuid}")
        # ------------------------
        raise

if __name__ == "__main__":
    main()