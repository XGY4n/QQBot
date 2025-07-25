# QQBot 

使用 C++ 编写的 QQ 机器人项目，支持通过 Python 3.9 进行任务调度与脚本扩展。

---

## 快速开始指南

### 1. 准备 Python 3.9 环境

请确保你已经在系统中安装并正确配置了 **Python 3.9**。(更高的版本未测试)
如未安装，可从 [Python 官网](https://www.python.org/downloads/release/python-390/) 下载并安装对应版本。

---
### 2. 编译项目
- 编译
推荐使用 CMake（可通过 Visual Studio 或命令行）进行构建：
- 需要C++17
```bash
mkdir build
cd build
cmake -DPYTHON_INCLUDE_DIR="your/python/39/include/path" .. -G "Visual Studio 17 2022" -A x64
```
### 3. 配置 INI 文件
推荐编译后将ini模板文件移动到build/ 和 .exe的输出目录下再编辑

项目使用多个 INI 配置文件，请按如下说明进行设置：

* **PythonEnv.ini**  
  配置 Python 3.9 的路径及相关设置。当前不会检查系统环境，请手动指定。
    ```ini
    [PYTHONHOME]
    HOME = D:\Python\Python39

    [PYTHONLIB]
    LIB = D:\Python\Python39\libs
    ```
* **BotSetting.ini**
  设置机器人要连接的 QQ 群名称和响应标识符。
    ```ini
    [Group]
    name = test #群名称

    [mark]
    Symbol = #

    [@]
    isAtback = false #回发QQ的字符串是否 @发送者
    ```
* **PythonTask.ini**
    配置机器人将执行的 Python 任务列表。

    示例配置：

    ```ini
    [Py_load_0]              # 任务名称，随意起名，如：Py_load_0, AI, GPT 等
    Py_Call_Head = test.     # 触发机器人响应的命令前缀
    Py_Call_Path = ./script/ # Python 脚本文件所在的目录
    Py_Call_File = test      # Python 脚本文件名（不含 .py 后缀）
    Py_Call_Func = test      # Python 脚本中要调用的函数名
    Py_Return_type = str      # 函数返回值类型，目前支持 str
    ```

    **说明：**
    当机器人收到以 `#test.` 开头的消息时，它会执行以下操作：

    1.  调用 `./script/test.py` 文件中的 `test()` 函数。

    2.  将 `test.` 后面的内容作为参数传入该函数。

    3.  函数的返回值（如果 `Py_Return_type` 设置为 `str`）会以字符串形式发送回 QQ 群聊。

> **提示**：请将模板 `ini/` 文件夹拷贝到 `/build` 和 `/build/Debug` 或 `/build/Release` 目录下。再去编辑复制过去的.ini

  ### 4.PythonTask.ini 返回类型说明

  `Py_Return_type` 用来指定 Python 函数的返回值类型，支持以下几种枚举值：

  | 类型名  | CPP枚举值   | 说明 |
  |--------|----------|------|
  | STRING | 0   | Python 函数返回的是 UTF-8 编码的普通字符串 (`str`)，机器人将其作为文本消息发送。 |
  | WSTRING| 1  | Python 函数返回的是宽字符字符串（Unicode），适合包含多语言字符的文本。 |
  | FILE   | 2     | 返回一个文件路径，机器人会读取该文件并以文件形式发送到聊天窗口。 |
  | QIMAGE | 3   | 返回的是一个图像文件(.bmp .jpg .gif .png)的路径，机器人会将该图像以QQ图片的形式发送。 |
  | AUTO   | 4     | 自动检测返回值类型，根据 Python 返回内容智能选择合适的处理方式。 |
  | UNKNOWN| 5  | 未知类型，通常表示返回值没有指定类型或类型未识别，机器人默认以字符串处理。 |

  ---
### 5. 日志文件
  在执行目录的/log下写入日志文件
  其中名字带有[]的.log是C++端的文件日志, 没有带[]的是python的日志
  可以通过cmakelist的宏
  ```bash
  add_definitions(-D_CLASS_LOG_)
  ```
  来配置C++端日志的详细性

### 6. 运行：
  推荐使用TIM或老版本QQ否则可能找不到 消息管理器
 
 1.启动 QQ/TIM

 2.打开目标群聊

 3.打开该群聊的 消息管理器 窗口

### 7. python脚本编写规则：
 对于单次立即返回的功能:
 ```Python
 #./script/test.py
import QQbot
import time
def test(data):
    bot = QQbot.BotSDK()
    bot.print(data)#{'Values': 'test'}
    bot.print(data['Values'])#'test'
    return "test end"#回发testend
 ```

对于长期的持久功能
 ```Python
import QQbot
import time
def test(data):
    bot = QQbot.BotSDK()
    bot.print(data)#{'Values': 'test'}
    bot.print(data['Values'])#'test'
    while True:
        msg = bot.get_latest_message()
        if msg:
            bot.print(f"主线程处理接收到的消息：{msg}")
            bot.print(f"消息发送者名称：{msg.username}")
            bot.print(f"消息发送者QQ号：{msg.email}")
            bot.print(f"时间：{msg.time}")
            bot.print(f"消息：{msg.content}")#'test'
            bot.postMessage("Get once")#回发QQ默认字符串
            bot.postMessage("some/path", "AUTO")#支持多类型见PythonTask.ini 返回类型说明
        time.sleep(0.1)
    return "long task end"
```
 对应的PythonTask.ini节配置:
```ini
    [Py_load_0]              # 任务名称，随意起名，如：Py_load_0, AI, GPT 等
    Py_Call_Head = test.     # 触发机器人响应的命令前缀
    Py_Call_Path = ./script/ # Python 脚本文件所在的目录
    Py_Call_File = test      # Python 脚本文件名（不含 .py 后缀）
    Py_Call_Func = test      # Python 脚本中要调用的函数名
    Py_Return_type = str      # 函数返回值类型，目前支持 str
```
对应触发的QQ消息:
```bash
     #test.test
```

