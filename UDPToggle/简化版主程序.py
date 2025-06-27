#!/usr/bin/env python3
"""
VCU测试开关桩管理工具 - 简化版主程序

专门用于测试自动加载功能，去除所有可能导致问题的emoji字符
"""

import sys
import logging
import tkinter as tk
from pathlib import Path
import time

# 添加项目根目录到sys.path
project_root = Path(__file__).parent
sys.path.insert(0, str(project_root))

from core.config_manager import config


def setup_logging():
    """设置日志系统"""
    log_dir = Path("logs")
    log_dir.mkdir(exist_ok=True)
    
    # 配置日志格式
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
        datefmt="%Y-%m-%d %H:%M:%S",
        handlers=[
            logging.FileHandler(log_dir / "vcu_controller.log", encoding='utf-8'),
            logging.StreamHandler(sys.stdout)
        ]
    )


class SimpleTestApp:
    """简化的测试应用程序"""
    
    def __init__(self, root):
        self.root = root
        self.root.title("VCU配置自动加载测试")
        self.root.geometry("800x600")
        
        # 创建简单的文本显示区域
        self.text_area = tk.Text(root, wrap=tk.WORD, font=('Consolas', 10))
        self.text_area.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        self.log("应用程序启动完成")
    
    def log(self, message):
        """添加日志信息"""
        timestamp = time.strftime("%H:%M:%S")
        log_entry = f"[{timestamp}] {message}\n"
        self.text_area.insert(tk.END, log_entry)
        self.text_area.see(tk.END)
        print(f"[{timestamp}] {message}")
    
    def load_configuration(self):
        """加载配置（模拟方法）"""
        self.log("开始加载设备配置...")
        
        try:
            # 检查设备配置文件
            devices_config_path = Path("config/devices.json")
            if devices_config_path.exists():
                import json
                with open(devices_config_path, 'r', encoding='utf-8') as f:
                    devices_config = json.load(f)
                
                devices = devices_config.get("devices", {})
                self.log(f"成功加载 {len(devices)} 个设备配置:")
                
                for device_id, device_info in devices.items():
                    enabled = device_info.get("enabled", True)
                    status = "启用" if enabled else "禁用"
                    self.log(f"  - {device_id}: {device_info.get('name', '未命名')} ({status})")
                
                return True
            else:
                self.log("设备配置文件不存在")
                return False
                
        except Exception as e:
            self.log(f"加载配置失败: {e}")
            return False
    
    def load_sample_project(self, file_path):
        """加载示例项目（模拟方法）"""
        self.log(f"开始加载示例项目: {file_path}")
        
        try:
            sample_file = Path(file_path)
            if sample_file.exists():
                self.log(f"成功找到示例文件: {sample_file.name}")
                self.log(f"文件大小: {sample_file.stat().st_size} 字节")
                
                # 模拟解析开关桩
                self.log("正在解析C文件中的开关桩定义...")
                self.log("找到开关桩: SYSTEM_INIT_COMPLETE_FLAG")
                self.log("找到开关桩: UDP_COMM_ENABLE") 
                self.log("找到开关桩: DATA_CRC_CHECK_ENABLE")
                self.log("示例项目加载成功！")
                
                return True
            else:
                self.log(f"示例文件不存在: {file_path}")
                return False
                
        except Exception as e:
            self.log(f"加载示例项目失败: {e}")
            return False
    
    def connect_all_devices(self):
        """连接所有设备（模拟方法）"""
        self.log("开始连接所有设备...")
        self.log("模拟连接 MVCU1_2...")
        self.log("模拟连接 MVCU1_16...")
        self.log("模拟连接 SVCU1_3...")
        self.log("所有设备连接完成")
    
    def add_log_entry(self, message, level="INFO"):
        """添加日志条目"""
        self.log(f"{level}: {message}")


def auto_load_devices(app):
    """自动加载设备配置"""
    logger = logging.getLogger(__name__)
    
    try:
        if config.should_auto_load_devices():
            logger.info("启动时自动加载设备配置...")
            app.log("检测到启用了设备自动加载")
            
            device_list = config.get_default_device_list()
            if device_list:
                logger.info(f"准备加载默认设备: {device_list}")
                app.log(f"默认设备列表: {', '.join(device_list)}")
                
                # 调用应用程序的加载配置方法
                success = app.load_configuration()
                
                if success:
                    logger.info("设备配置加载成功")
                    app.add_log_entry("设备配置已自动加载", "SUCCESS")
                    
                    # 显示加载的设备列表
                    for device_name in device_list:
                        app.add_log_entry(f"已加载设备: {device_name}", "INFO")
                    
                    # 如果启用了自动连接，则尝试连接设备
                    if config.get("devices.auto_connect", False):
                        logger.info("开始自动连接设备...")
                        app.add_log_entry("开始自动连接设备...", "INFO")
                        
                        # 延迟连接，确保设备加载完成
                        app.root.after(2000, lambda: app.connect_all_devices())
                        
                else:
                    logger.warning("设备配置加载失败")
                    app.add_log_entry("设备配置加载失败", "WARNING")
                    
            else:
                logger.info("没有配置默认设备列表")
                app.add_log_entry("没有配置默认设备列表", "INFO")
                
        else:
            logger.info("未启用设备自动加载")
            app.log("设备自动加载被禁用")
            
    except Exception as e:
        logger.error(f"自动加载设备失败: {e}", exc_info=True)
        app.add_log_entry(f"自动加载设备失败: {e}", "ERROR")


def check_and_setup_first_run(app):
    """检查并设置首次运行"""
    logger = logging.getLogger(__name__)
    
    try:
        if config.should_auto_load_sample():
            logger.info("检测到首次运行，准备自动加载示例项目...")
            app.log("检测到首次运行，将自动加载示例项目")
            
            # 显示欢迎信息
            import tkinter.messagebox as msgbox
            welcome_msg = (
                "欢迎使用VCU测试开关桩管理工具！\n\n"
                "检测到这是您的首次运行，系统将自动加载示例项目\n"
                "以便您快速了解工具的功能和使用方法。\n\n"
                "示例项目包含：\n"
                "• 预配置的VCU设备 (MVCU1_2, MVCU1_16, SVCU1_3)\n"
                "• 丰富的开关桩示例\n"
                "• 不同类型的配置演示\n\n"
                "点击确定继续..."
            )
            
            app.log("显示欢迎对话框...")
            result = msgbox.showinfo("欢迎", welcome_msg)
            
            # 自动加载示例项目
            sample_files = list(config.get_sample_project_path().glob("*.c"))
            sample_files.extend(list(config.get_sample_project_path().glob("*.h")))
            
            if sample_files:
                logger.info(f"找到 {len(sample_files)} 个示例文件")
                app.log(f"找到 {len(sample_files)} 个示例文件")
                
                # 加载第一个示例文件
                sample_file = sample_files[0]
                logger.info(f"自动加载示例文件: {sample_file}")
                
                # 调用应用程序的加载项目方法
                success = app.load_sample_project(str(sample_file))
                
                if success:
                    logger.info("示例项目加载成功")
                    app.add_log_entry("示例项目已自动加载", "SUCCESS")
                    app.add_log_entry(f"加载文件: {sample_file.name}", "INFO")
                    
                    # 显示成功消息
                    success_msg = (
                        "示例项目加载成功！\n\n"
                        f"已从 {sample_file.name} 解析出开关桩配置。\n"
                        "您现在可以：\n"
                        "• 查看右侧的开关桩列表\n"
                        "• 尝试连接VCU设备\n"
                        "• 体验开关桩的操作功能\n\n"
                        "如需加载其他项目文件，请使用菜单中的'加载项目'功能。"
                    )
                    msgbox.showinfo("加载成功", success_msg)
                else:
                    logger.warning("示例项目加载失败，使用默认配置")
                    app.add_log_entry("示例项目加载失败，使用默认配置", "WARNING")
            else:
                logger.warning("未找到示例项目文件")
                app.add_log_entry("未找到示例项目文件", "WARNING")
            
            # 标记首次运行完成
            config.mark_first_run_completed()
            app.log("首次运行标记已设置")
            
        else:
            logger.info("非首次运行，跳过自动加载示例项目")
            app.log("非首次运行，跳过示例项目自动加载")
            
    except Exception as e:
        logger.error(f"首次运行设置失败: {e}", exc_info=True)
        app.add_log_entry(f"首次运行设置失败: {e}", "ERROR")
        # 即使失败也要标记首次运行完成，避免重复尝试
        config.mark_first_run_completed()


def main():
    """主函数"""
    try:
        # 设置日志
        setup_logging()
        logger = logging.getLogger(__name__)
        
        logger.info("VCU测试开关桩管理工具启动 - 简化版")
        
        # 验证配置
        if not config.validate():
            logger.warning("配置验证失败，使用默认配置")
        
        # 创建主窗口
        root = tk.Tk()
        app = SimpleTestApp(root)
        
        logger.info("GUI应用程序已启动")
        app.log("GUI应用程序已启动")
        
        # 检查首次运行并自动加载示例
        app.log("准备检查首次运行状态...")
        root.after(1000, lambda: check_and_setup_first_run(app))  # 延迟1秒执行
        
        # 自动加载设备配置
        app.log("准备自动加载设备配置...")
        root.after(1500, lambda: auto_load_devices(app))  # 延迟1.5秒执行
        
        # 启动主循环
        root.mainloop()
        
        logger.info("VCU测试开关桩管理工具退出")
        
    except KeyboardInterrupt:
        logger.info("收到中断信号，正在退出...")
    except Exception as e:
        logger.error(f"应用程序异常: {e}", exc_info=True)
        print(f"ERROR: {e}")
        input("按回车键退出...")
        sys.exit(1)


if __name__ == "__main__":
    main() 