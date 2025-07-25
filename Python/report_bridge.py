import requests
import json
import os
import Botlog as logger
        
class ReportBridge:
    def __init__(self, report_url):
        self._report_url = report_url
        logger.ipc_print("bridge done")
        
    def report_result(self, result_dict):
        if not self._report_url:
            raise RuntimeError("Bridge not initialized.")
        try:
            res = requests.post(self._report_url, json=result_dict)
            logger.ipc_print("Result sent:" + str(res.status_code))
        except Exception as e:
            logger.ipc_print("Failed to send result: " + str(e))