# Testplatform

## 项目简介

该仓库包含用于VCU相关测试的脚本和工具，集成了日志管理、自动化测试、在线更新以及Wireshark协议解析等功能。主要代码位于 `udp_loglib.py` 与 `vcu_test.py`，提供了通过UDP与设备交互的能力，并支持日志收集及事件分析。

## 目录结构

- `Log_Log/`：测试日志与数据库文件存放位置。
- `Master/`：历史版本的脚本及配置，内部包含与根目录相似的 `testplatform` 结构。
- `scripts/`：批量推送、更新等辅助脚本，参见其中的 `README.md` 获取详细说明。
- `tests/`：基于 `unittest` 的单元测试，验证核心库的功能。
- `update_test/`：示例更新脚本及文件，用于部署新的测试环境。
- `wireshark/`：Wireshark 协议解析脚本及安装包，便于分析抓包数据。

## 主要功能

1. **UDP 日志库 (`udp_loglib.py`)**
   - 定义 `TestPlatForm` 类，用于发送测试指令、抓取dump、收集EventLog 等。
   - 提供 `Log` 类记录和存储日志，可保存到文件或数据库。
   - 实现 `Conf`、`Reply` 等消息格式解析与封装，便于与设备通信。
2. **日志管理界面 (`log_manger.pyw`)**
   - 基于 `tkinter` 的图形界面，可实时展示日志信息并与测试脚本交互。
3. **自动化测试入口 (`vcu_test.py`)**
   - 初始化测试环境，配置端口、Wireshark 抓包及在线更新等功能。
4. **辅助工具**
   - `scripts/batch_push.py`：按批次提交并推送代码，适合大文件或不稳定网络环境。
   - `update_test/update_test.py`：对比并更新测试目录的脚本，支持保留自定义配置区域。

## 使用方法

1. 安装 Python 3.10 环境（见 `pyvenv.cfg`）。
2. 执行 `vcu_test.py` 进入测试平台，根据需要调用各类接口发送指令或获取日志。
3. 如需查看或管理日志，可运行 `log_manger.pyw` 启动图形界面。
4. 更多脚本参数及推送流程，请参考 `scripts/README.md`。

## 贡献与测试

提交代码前建议运行 `tests/test_loglib.py`，确保核心库功能正常：

```bash
python -m unittest tests/test_loglib.py
```

如需批量推送，请使用 `scripts/batch_push.py` 按说明操作。

