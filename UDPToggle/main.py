#!/usr/bin/env python3
"""
VCU测试开关桩管理工具 - 主程序

一个专业的VCU测试开关桩管理工具，基于UDP通信协议实现对VCU设备的远程控制和监控。
"""

import sys
import os
import logging
import tkinter as tk
from pathlib import Path

# 添加项目根目录到sys.path
project_root = Path(__file__).parent
sys.path.insert(0, str(project_root))

from gui.main_window import VCUControllerApp
from core.config_manager import config


def setup_logging():
    """设置日志系统"""
    log_dir = Path("logs")
    log_dir.mkdir(exist_ok=True)
    
    # 配置日志格式
    log_format = "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
    date_format = "%Y-%m-%d %H:%M:%S"
    
    # 创建日志处理器
    file_handler = logging.FileHandler(
        log_dir / "vcu_controller.log", 
        encoding='utf-8'
    )
    file_handler.setFormatter(logging.Formatter(log_format, date_format))
    
    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setFormatter(logging.Formatter(log_format, date_format))
    
    # 设置根日志器
    root_logger = logging.getLogger()
    root_logger.setLevel(logging.INFO)
    root_logger.addHandler(file_handler)
    root_logger.addHandler(console_handler)
    
    # 设置第三方库日志级别
    logging.getLogger('matplotlib').setLevel(logging.WARNING)
    logging.getLogger('PIL').setLevel(logging.WARNING)


def main():
    """主函数"""
    try:
        # 设置日志
        setup_logging()
        logger = logging.getLogger(__name__)
        
        logger.info("="*50)
        logger.info("VCU测试开关桩管理工具启动")
        logger.info("="*50)
        
        # 验证配置
        if not config.validate():
            logger.error("配置验证失败，使用默认配置")
        
        # 创建主窗口
        root = tk.Tk()
        app = VCUControllerApp(root)
        
        logger.info("GUI应用程序已启动")
        
        # 启动主循环
        root.mainloop()
        
        logger.info("VCU测试开关桩管理工具退出")
        
    except KeyboardInterrupt:
        logger.info("收到中断信号，正在退出...")
    except Exception as e:
        logger.error(f"应用程序异常: {e}", exc_info=True)
        sys.exit(1)


if __name__ == "__main__":
    main() 