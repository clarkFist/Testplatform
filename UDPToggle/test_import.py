#!/usr/bin/env python3
"""
测试模块导入
"""

import sys
import traceback
from pathlib import Path

print("Python版本:", sys.version)
print("当前工作目录:", Path.cwd())

# 添加项目根目录到sys.path
project_root = Path(__file__).parent
sys.path.insert(0, str(project_root))
print("项目根目录:", project_root)
print("sys.path:", sys.path[:3])  # 显示前3个路径

try:
    print("尝试导入 core.config_manager...")
    from core.config_manager import config
    print("✅ core.config_manager 导入成功")
    
    print("尝试导入 gui.main_window...")
    from gui.main_window import VCUControllerApp
    print("✅ gui.main_window 导入成功")
    
    print("尝试导入 tkinter...")
    import tkinter as tk
    print("✅ tkinter 导入成功")
    
    print("所有模块导入测试通过！")
    
except Exception as e:
    print("❌ 导入失败:")
    print(f"错误类型: {type(e).__name__}")
    print(f"错误信息: {e}")
    print("详细错误:")
    traceback.print_exc() 