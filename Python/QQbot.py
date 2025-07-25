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

@dataclass
class ParsedMessage:
    time: str
    username: str
    email: str
    content: str
    
class BroadcastHandler(BaseHTTPRequestHandler):
    bot_instance = None  # 绑定 BotSDK 实例

    def do_POST(self):
        if self.path == "/broadcast":
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            try:
                data = json.loads(post_data.decode('utf-8'))
            except json.JSONDecodeError:
                self.send_response(400)
                self.end_headers()
                return

            if BroadcastHandler.bot_instance:
                BroadcastHandler.bot_instance.print(f"[Broadcast Received] {data}")
                msg = self.parse_message(data["message"])
                BroadcastHandler.bot_instance.message_queue.put(msg)

            self.send_response(200)
            self.end_headers()
            self.wfile.write(b"OK")
        else:
            self.send_response(404)
            self.end_headers()

    def log_message(self, format, *args):
        # 防止打印 HTTP 请求日志
        return
        
    def parse_message(self, raw_msg: str) -> ParsedMessage:
        pattern = r'\[(.*?)\]\s+(.*?)\s+\((.*?)\)\s+(.*)'
        match = re.match(pattern, raw_msg)
        if not match:
            return ParsedMessage(time="", username="", email="", content=raw_msg)

        time, username, email, content = match.groups()
        return ParsedMessage(time, username, email, content)

class BotSDK:
    def __init__(self):
        self.musicPath = "./resource/music/"
        self.imgPath = "./resource/picture/"
        self.log_path = "./log/" + time.strftime('%Y-%m-%d', time.localtime()) + ".log"
        self.running = True
        self.bridge = cppHttp_bridge.ReportBridge("http://127.0.0.1:11451/report")
        self.message_queue = Queue()

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
            BroadcastHandler.bot_instance = self
            server = HTTPServer(("127.0.0.1", 19198), BroadcastHandler)
            self.print("Listening for broadcast POSTs on http://127.0.0.1:19198/broadcast")
            server.serve_forever()

        threading.Thread(target=run_server, daemon=True).start()

    def get_latest_message(self):
        try:
            return self.message_queue.get(timeout=1)  # 等待 1 秒，避免空转
        except:
            return None


# === 测试 ===
if __name__ == "__main__":
    bot = BotSDK()

    while True:
        msg = bot.get_latest_message()
        if msg:
            bot.print(f"主线程处理接收到的消息：{msg}")
        time.sleep(0.1)
