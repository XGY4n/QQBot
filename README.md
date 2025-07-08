# QQBot Project

This is a QQ bot project written in C++, which uses Python 3.9 for task scheduling and script support.

---

## Quick Start Guide

### 1. Prepare Python 3.9 Environment

Make sure you have Python 3.9 installed and properly configured on your system.  
If not, download and install it from the [official Python website](https://www.python.org/downloads/release/python-390/).

---

### 2. Configure INI Files

The project uses several INI configuration files that need to be set up:

- **PythonEnv.ini**  
  Configure the Python 3.9 environment path and related settings.

- **BotSetting.ini**  
  Fill in the QQ group name and identifier that the bot will connect to.

- **PythonTask.ini**  
  Specify the Python tasks to be executed by the bot.

---

### 3. Build and Run the Project

- Use CMake to build the project (Visual Studio or command line is recommended):

  ```bash
  mkdir build
  cd build
  cmake .. -G "Visual Studio 17 2022" -A x64

