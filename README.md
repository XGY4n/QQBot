# QQBot 

使用 C++ 编写的 QQ 机器人项目，支持通过 Python 3.9 进行任务调度与脚本扩展。

---

## 快速开始指南

### 1. 准备 Python 3.9 环境

请确保你已经在系统中安装并正确配置了 **Python 3.9**。(更高的版本未测试)
如未安装，可从 [Python 官网](https://www.python.org/downloads/release/python-390/) 下载并安装对应版本。

---

### 2. 配置 INI 文件

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
    name = test

    [mark]
    Symbol = #
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

---

### 3. 编译并运行项目
- 编译
推荐使用 CMake（可通过 Visual Studio 或命令行）进行构建：

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 
```

- 运行：

 1.启动 QQ

 2.打开目标群聊

 3.打开该群聊的 消息管理器 窗口（