# Testplatform

## 项目简介

该项目为 VCU（Vehicle Control Unit） 测试提供统一的脚本及工具集合，涵盖日志收集、协议解析、自动化执行等功能。核心脚本 `udp_loglib.py` 与 `vcu_test.py` 提供通过 UDP 与设备交互的能力，并集成 Wireshark 协议解析以及日志管理界面，方便开发人员快速搭建测试环境。

## 项目作用

- **统一测试流程**：通过封装常用指令、日志处理和事件解析，简化 VCU 调试与验证工作。
- **易于扩展**：脚本使用 Python 编写，可根据实际需求二次开发或集成到现有系统。
- **辅助定位问题**：结合 Wireshark 插件与图形化日志界面，便于实时观察通信数据与错误信息。

## 目录结构

- `Log_Log/`：测试日志及数据库文件的默认存放目录。
- `Master/`：存放历史版本脚本及配置，结构与当前项目相似，主要用于备份参考。
- `scripts/`：提供批量推送、环境更新等辅助脚本，详见 `scripts/README.md`。
- `rssp1/`：包含 RSSP1 协议模拟器及相关构建文件，常用于联调测试。
- `tests/`：基于 `unittest` 的单元测试代码。
- `update_test/`：示例更新脚本及文件，用于部署新的测试目录。
- `wireshark/`：Wireshark 协议解析脚本及安装包，用于分析抓包数据。
- 其他文件：如 `log_manger.pyw`（日志管理界面）、`lru_print.py`（LRU 信息转换），以及多种批处理脚本（`.bat`）等。

## 如何运行

1. **准备环境**：建议使用 Python 3.10（参考 `pyvenv.cfg`）。
2. **安装依赖**：进入虚拟环境后自行安装 `gevent`、`tkinter` 等必要库。
3. **启动测试平台**：
   ```bash
   python vcu_test.py
   ```
   脚本默认监听 UDP 18125 端口并根据配置与设备通信。
4. **查看日志**：若需图形化查看或管理日志，可执行：
   ```bash
   python log_manger.pyw
   ```
5. **更新测试目录**（可选）：
   ```bash
   python update_test/update_test.py
   ```
6. **运行单元测试**：
   ```bash
   python -m unittest tests/test_loglib.py
   ```

## 贡献与测试

欢迎提交 Issue 与 PR。在提交代码前建议运行上方的单元测试以确保核心功能正常；如需分批推送，可使用 `scripts/batch_push.py`。
