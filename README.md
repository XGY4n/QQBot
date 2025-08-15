# QQBot 

使用 C++ 编写的 QQ 机器人项目，支持通过 Python 3.9 进行任务调度与脚本扩展。

---

## 快速开始指南

### 1. 准备 Python 3.9 环境

请确保你已经在系统中安装并正确配置了 **Python 3.9**。(更高的版本未测试)
如未安装，可从 [Python 官网](https://www.python.org/downloads/release/python-390/) 下载并安装对应版本。
#### 1.1安装依赖

在您的项目根目录中，使用 `requirements.txt` 文件安装所有依赖库。

```bash
pip install -r requirements.txt
```
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
    [Py_load_0]              # 唯一名称 必填 任务名称，随意起名，如：Py_load_0, AI, GPT 等
    Py_Call_Head = test.     # 必填 触发机器人响应的命令前缀
    Py_Call_Path = ./script/ # 必填 Python 脚本文件所在的目录
    Py_Call_File = test      # 必填 Python 脚本文件名（不含 .py 后缀）
    Py_Call_Func = test      # 必填 Python 脚本中要调用的函数名
    Py_Return_type = str     # 必填 函数返回值类型，目前支持 str
    AUTO_Start = xxx         # 可选 随机器人自动启动时传参 不填写则不会自动启动
    isUnique = true          # 可选 false/true是否唯一 默认不填写为false 
    Py_Task_Type = short     # 可选 long/short 是否为短任务 默认不填写为长任务 短任务会在5s内把python脚本强制终止
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
        msg = bot.get_latest_message()#阻塞 接收后续所有#开头的QQ消息
        if msg:
            bot.print(f"主线程处理接收到的消息：{msg}")
            bot.print(f"消息发送者名称：{msg.username}")
            bot.print(f"消息发送者QQ号：{msg.email}")
            bot.print(f"时间：{msg.time}")
            bot.print(f"消息：{msg.content}")#'test'
            bot.postMessage("Get once")#回发QQ默认字符串
            bot.postMessage("some/path", "AUTO")#支持多类型见PythonTask.ini 返回类型说明
    return "long task end"
```
 对应的PythonTask.ini节配置:
```ini
    [Py_load_0]              # 任务名称，随意起名，如：Py_load_0, AI, GPT 等
    Py_Call_Head = test.     # 触发机器人响应的命令前缀
    Py_Call_Path = ./script/ # Python 脚本文件所在的目录
    Py_Call_File = test      # Python 脚本文件名（不含 .py 后缀）
    Py_Call_Func = test      # Python 脚本中要调用的函数名
    Py_Return_type = str     # 函数返回值类型
```
对应触发的QQ消息:
```bash
     #test.test
```
### 8.脚本DEBUG
QQbot SDK 内置了对 VS Code 的调试支持，可以轻松地在脚本中进行断点调试。
#### 8.1 配置 VS Code
需要使用 `attach` 模式来连接正在运行的脚本。在项目根目录的Python文件夹中创建Python的Debug配置文件。在 `.vscode` 文件夹中，创建或修改 `launch.json` 文件，添加一个 `attach` 配置。

**`.vscode/launch.json`**
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Python: Attach to bot",
            "type": "debugpy",
            "request": "attach",
            "connect": {
                "host": "localhost",
                "port": 58260  //固定配置
            },
            "justMyCode": false
        }
    ]
}

```
#### 8.2 修改脚本
 ```Python
import QQbot
import time
def test(data):
    bot = QQbot.BotSDK()
    bot.print(data)#{'Values': 'test'}

    bot.StartDebug() #增加这行将会阻塞 此时连接vscode的debug

    bot.print(data['Values'])#'test'
    while True:
        msg = bot.get_latest_message()#阻塞 接收后续所有#开头的QQ消息
        if msg:
            bot.print(f"主线程处理接收到的消息：{msg}")
            bot.print(f"消息发送者名称：{msg.username}")
            bot.print(f"消息发送者QQ号：{msg.email}")
            bot.print(f"时间：{msg.time}")
            bot.print(f"消息：{msg.content}")#'test'
            bot.postMessage("Get once")#回发QQ默认字符串
            bot.postMessage("some/path", "AUTO")#支持多类型见PythonTask.ini 返回类型说明
    return "long task end"
```
#### 8.3 启动调试

1. **启动脚本**：从QQ启动 Python 脚本。
2. **等待连接**：脚本会运行到 `bot.StartDebug()` 这一行并暂停，等待连接。
3. **连接调试器**：打开 VS Code，切换到 **运行和调试** 视图（快捷键 `Ctrl+Shift+D`）。
4. **选择配置**：在顶部的下拉菜单中选择 **"Python: Attach to bot"**。
5. **开始调试**：点击绿色的 **启动调试** 按钮。

一旦连接成功，您就可以设置断点、检查变量和进行单步调试了。

### 9.长任务 短任务 唯一性机制详解

在机器人系统中，任务的配置决定了其行为模式，主要涉及**任务类型（长/短任务）**和**唯一性**两个核心概念。理解这些配置项能帮助你更好地控制任务的生命周期和执行逻辑。

#### 任务类型：长任务与短任务

任务类型由 `Py_Task_Type` 参数决定，可选值为 `long` 或 `short`。

* **短任务 (`Py_Task_Type = short`)**：
    * **定义**：旨在执行快速、一次性的操作。
    * **生命周期**：任务会在启动后，无论是否完成，都将在**5秒后被 C++ 端强制终止**。这是一种安全机制，可以防止脚本因意外挂起而占用系统资源。
    * **适用场景**：非常适合用于那些只需要短暂执行、快速返回结果的脚本，例如查询、状态报告或简单的计算。
* **长任务 (`Py_Task_Type` 未配置或配置为 `long`)**：
    * **定义**：旨在执行持续性、需要长时间运行的操作。
    * **生命周期**：长任务不会被系统自动终止。它会一直运行，直到满足以下任一条件：
        1.  任务通过 `return` 返回。
        2.  任务**失去心跳**。
        3.  被用户或系统主动终止。
    * **适用场景**：适用于需要持续监控、后台运行或处理复杂流程的脚本，例如保持与外部服务的连接、长时间数据处理等。

---

#### 任务唯一性：仅适用于长任务

任务唯一性由 `isUnique` 参数决定，可选值为 `true` 或 `false`，默认值为 `false`。**此机制仅对长任务有效。**

* **唯一性长任务 (`isUnique = true`)**：
    * **定义**：在整个机器人生命周期内，同一时间只允许存在一个特定任务的实例。
    * **工作机制**：当一个被标记为唯一的长任务被拉起后，系统会生成一个**哈希值**来唯一标识它。如果此时再次尝试拉起相同的任务，系统会检测到哈希值冲突，并**拒绝新任务的启动**。
    * **哈希值构成**：哈希值通常由`任务名称`、`脚本路径`和`文件名`等关键信息组合而成。
    * **示例**：如果 `Py_load_0` 是一个唯一的长任务，第一次通过 `test.xxx` 启动后，再次输入 `test.yyy`或者`test.xxx` 试图启动它，系统会识别出这是同一个任务，并阻止新实例的创建。
* **非唯一性长任务 (`isUnique = false` 或未配置)**：
    * **定义**：允许同一任务在同一时刻存在多个独立的实例。
    * **工作机制**：每次拉起任务，即使是使用相同的任务不同的配置，系统都会将其视为一个全新的任务，并允许其独立运行。
    * **示例**：如果 `Py_load_0` 是非唯一的长任务，第一次通过 `test.xxx` 启动后，再次输入 `test.yyy` 同样会成功启动一个全新的 `Py_load_0` 实例，两者互不影响。但是再次输入`test.xxx`会被拒绝。
* **短任务与唯一性**：
    * **短任务**不具备唯一性。由于短任务生命周期极短，它们的执行是独立的，因此没有必要为其设置唯一性限制。

理解这些配置能帮助你根据任务的实际需求，灵活地配置和管理你的机器人任务。