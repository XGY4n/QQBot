import os
import time
from datetime import datetime
import inspect

musicPath = "./resource/music/"
imgPath = "./resource/picture/"
LogPath = "./log/"+time.strftime('%Y-%m-%d',time.localtime(time.time())) + ".log"
def Pmessage(string): 
    frame = inspect.stack()[2]
    caller_filename = os.path.basename(frame.filename)
    caller_line = frame.lineno
    caller_func = frame.function

    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
    table_message = f"[Python][{timestamp}][{caller_filename}] {string}"#+:{caller_func}:{caller_line}
    print(table_message)
    return table_message

def bot_print(string):
    with open(LogPath, mode='a+', encoding='utf-8') as file_obj:
        file_obj.write(Pmessage(string) + "\n")   
    file_obj.close()
    
def ipc_print(string):
    with open(LogPath, mode='a+', encoding='utf-8') as file_obj:
        file_obj.write(Pmessage(string) + "\n")   #f"[{module}]" 
    file_obj.close()
    