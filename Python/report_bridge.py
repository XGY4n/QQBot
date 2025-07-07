import requests
import json
import os
import QQbot as bot
_report_url = None

def init_bridge(report_url):
    global _report_url
    _report_url = report_url

def report_result(result_dict):
    if not _report_url:
        raise RuntimeError("Bridge not initialized. Call init_bridge first.")
    try:
        res = requests.post(_report_url, json=result_dict)
        bot.ipc_print("Result sent:" + str(res.status_code))
    except Exception as e:
        bot.ipc_print("Failed to send result:", e)
