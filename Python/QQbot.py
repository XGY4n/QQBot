import os
import time
import json
import threading
import socket
from datetime import datetime
from queue import Queue
from http.server import BaseHTTPRequestHandler, HTTPServer
import report_bridge as cppHttp_bridge
import Botlog as logger
import re
from dataclasses import dataclass
import debugpy
import os
import sys

@dataclass
class ParsedMessage:
    time: str
    username: str
    email: str
    content: str
    
class BotSDK:
    def __init__(self):
        self.musicPath = "./resource/music/"
        self.imgPath = "./resource/picture/"
        self.log_path = "./log/" + time.strftime('%Y-%m-%d', time.localtime()) + ".log"
        self.running = True
        self.bridge = cppHttp_bridge.ReportBridge("http://127.0.0.1:11451/report")
        self.message_queue = Queue()
        self.DebugStatus = False
        self.start_broadcast_listener()  # 启动监听线程

    def print(self, string):
        logger.bot_print(string)

    def postMessage(self, message, RTtype="str"):
        report_data = {
            "task_uuid":"DONT NEED",
            "result": message,
            "status": "working",
            "timestamp": datetime.now().isoformat(),
            "return_type": RTtype
        }
        self.bridge.report_result(report_data)
        
    def postMessageAT(self, message, caller, QQnumber, RTtype="str"):
        report_data = {
            "Caller": caller,
            "QQnumber": QQnumber,
            "task_uuid":"DONT NEED",
            "result": message,
            "status": "working",
            "timestamp": datetime.now().isoformat(),
            "return_type": RTtype
        }
        self.bridge.report_result(report_data)
        
    def start_broadcast_listener(self):
        def run_server():
            
            udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            udp_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            udp_sock.bind(("", 19198))  # 监听 19198 端口

            self.print("Listening for UDP broadcast on port 19198...")
            while True:
                data, addr = udp_sock.recvfrom(4096)  # 收 4KB
                try:
                    msg_json = json.loads(data.decode("utf-8"))
                    self.print(f"[Broadcast Received from {addr}] {msg_json}")
                    msg = self.parse_message(msg_json["message"])
                    self.message_queue.put(msg)
                except Exception as e:
                    self.print(f"Error parsing message: {e}")
                    
        threading.Thread(target=run_server, daemon=True).start()

    def get_latest_message(self):
        try:
            return self.message_queue.get()
        except:
            return None
    def StartDebug(self):
        if self.DebugStatus:
            print("Debug server is already active.")
            return

        self.DebugStatus = True

        if not getattr(sys, "_dbgpy_started", False):
            DEBUG_PORT = int(os.environ.get("PY_DEBUG_PORT", "58260"))
            
            try:
                debugpy.listen(("0.0.0.0", DEBUG_PORT))
                self.print(f"[DEBUG] Debug server listening on port {DEBUG_PORT}")
                sys._dbgpy_started = True
            except Exception as e:
                self.print(f"Error starting debug server: {e}")
                self.DebugStatus = False 
                return

        self.print("[DEBUG] Waiting for VSCode debugger to attach...")
        try:
            debugpy.wait_for_client()
            self.print("[DEBUG] Debugger attached!")
        except Exception as e:
            self.print(f"[DEBUG] Failed to attach debugger: {e}")
            self.DebugStatus = False 
            
    def parse_message(self, raw_msg: str) -> ParsedMessage:
        pattern = r'\[(.*?)\]\s+(.*?)\s+\((.*?)\)\s+(.*)'
        match = re.match(pattern, raw_msg)
        if not match:
            return ParsedMessage(time="", username="", email="", content=raw_msg)

        time, username, email, content = match.groups()
        return ParsedMessage(time, username, email, content)
# === 测试 ===
if __name__ == "__main__":
    bot = BotSDK()

    while True:
        msg = bot.get_latest_message()
        if msg:
            bot.print(f"主线程处理接收到的消息：{msg}")
        time.sleep(0.1)
