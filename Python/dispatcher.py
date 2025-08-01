import sys
import subprocess
import json
import random
import urllib.parse
import socket
import datetime
import uuid
import Botlog as logger

def write_log(message: str):
    logger.ipc_print(message)

def is_port_available(port: int) -> bool:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            s.bind(("127.0.0.1", port))
            return True
        except OSError:
            return False



def find_available_port(min_port: int, max_port: int, exclude_ports: list = None) -> int:

    if exclude_ports is None:
        exclude_ports = []
    attempts = 0
    max_attempts = (max_port - min_port + 1) * 2
    while attempts < max_attempts:
        port = random.randint(min_port, max_port)
        if port not in exclude_ports and is_port_available(port):
            return port
        attempts += 1
    raise RuntimeError(f"在 {min_port}-{max_port} 范围内找不到可用端口。")

def main(jsmsg, py_home):
    write_log("into dispatcher.py main()")
    write_log(jsmsg)
    write_log("Python" + py_home)
    # 解析传入的JSON字符串
    try:
        js_data = json.loads(jsmsg)
    except json.JSONDecodeError as e:
        raise ValueError(f"Invalid JSON in jsmsg: {e}")

    # 从JSON中提取用户脚本和任务参数
    user_script = js_data.get("script_path")
    if not user_script:
        raise ValueError("Missing 'script_path' in jsmsg")
    
    # 获取输入参数（默认为空字典）
    input_task_args = js_data.get("input_args", {})
    
    # 获取模块信息
    module = js_data.get("Module", {})
    write_log(f"module: {module}")

    # 端口设置
    #heartbeat_port = 10712  # 明确指定一个固定端口
    heartbeat_port = find_available_port(8000, 9000)


    report_port = 11451  # 保持固定端口

    # 构造报告URL
    base_report_url = input_task_args.get("report_url", "http://127.0.0.1/report")
    parsed_base_url = urllib.parse.urlparse(base_report_url)
    report_url_for_worker = parsed_base_url._replace(
        netloc=f"{parsed_base_url.hostname or '127.0.0.1'}:{report_port}"
    ).geturl()
    #input_task_args["report_url"] = report_url_for_worker
    input_task_args["Values"] = js_data.get("value", {})  
    function_name =  js_data.get("Function", "run")
    return_type = js_data.get("ReturnType", "str")
    now = datetime.datetime.now()
    timestamp = now.strftime('%Y%m%d%H%M%S') + f"{int(now.microsecond / 1000):03d}"
    taskname = f"{user_script}{module}.{function_name}_{timestamp}"
    # 构建整合所有信息的单一JSON对象
    task_uuid = str(uuid.uuid4())

    combined_payload = {
        "task_uuid": task_uuid,
        "function_name": function_name,
        "script_path": user_script,
        "module": module,
        "heartbeat_port": heartbeat_port,
        "report_url" : report_url_for_worker,
        "task_args": input_task_args,
        "return_type": return_type,
    }
    
    write_log(f"整合后的参数: {json.dumps(combined_payload, indent=2)}")

    # 启动工作进程 - 现在只传递一个JSON参数
    cmd = [
        py_home + "\python.exe", "./Python/worker.py",
        json.dumps(combined_payload)  # 只传递一个参数
    ]
    
    write_log(f"启动命令: {' '.join(cmd)}")
    proc = subprocess.Popen(cmd, start_new_session=True)

    # 返回进程信息
    output_info = {
        "pid": proc.pid,
        "heartbeat_port": heartbeat_port,
        "report_port": report_url_for_worker,
        "task_uuid": task_uuid,
        "return_type": return_type,
    }
    finallymsg = json.dumps(output_info)
    write_log(f"输出信息: {finallymsg}")

    return finallymsg

if __name__ == "__main__":
    main(a)
