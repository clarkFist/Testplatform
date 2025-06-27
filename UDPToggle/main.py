#!/usr/bin/env python3
"""
VCU测试开关桩管理工具 - 主程序

专业的VCU测试开关桩管理工具，基于UDP通信协议实现对VCU设备的远程控制和监控。
"""

import sys
import logging
import tkinter as tk
from pathlib import Path
import time

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
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
        datefmt="%Y-%m-%d %H:%M:%S",
        handlers=[
            logging.FileHandler(log_dir / "vcu_controller.log", encoding='utf-8'),
            logging.StreamHandler(sys.stdout)
        ]
    )
    
    # 设置第三方库日志级别
    logging.getLogger('matplotlib').setLevel(logging.WARNING)
    logging.getLogger('PIL').setLevel(logging.WARNING)


def auto_load_devices(app):
    """自动加载设备配置"""
    logger = logging.getLogger(__name__)
    
    try:
        # 首先尝试自动加载示例YAML配置
        if config.get("devices.auto_load_yaml_samples", False):
            logger.info("启动时自动加载示例YAML配置...")
            app.add_log_entry("🔄 正在自动加载示例YAML配置...", "INFO")
            
            try:
                success = config.load_sample_yaml_config() 
                if success:
                    logger.info("示例YAML配置加载成功")
                    app.add_log_entry("✅ 示例YAML配置已自动加载", "SUCCESS")
                    
                    # 显示加载的设备配置
                    device_config = config.get_device_config()
                    if device_config and 'devices' in device_config:
                        device_count = len(device_config['devices'])
                        app.add_log_entry(f"📱 已加载 {device_count} 个设备配置", "INFO")
                        
                        # 显示设备列表
                        for device_name in device_config['devices'].keys():
                            app.add_log_entry(f"  • {device_name}", "INFO")
                        
                        # 显示开关桩配置
                        if 'switches' in device_config:
                            switch_count = len(device_config['switches'])
                            app.add_log_entry(f"🔧 已加载 {switch_count} 个开关桩配置", "INFO")
                        
                        # 如果启用了自动连接，则尝试连接设备
                        if config.get("devices.auto_connect", False):
                            logger.info("开始自动连接设备...")
                            app.add_log_entry("🔗 开始自动连接设备...", "INFO")
                            
                            # 延迟连接，确保设备加载完成
                            app.root.after(2000, lambda: app.connect_all_devices())
                    else:
                        app.add_log_entry("⚠️ YAML配置中未找到设备信息", "WARNING")
                        
                else:
                    logger.warning("示例YAML配置加载失败，尝试加载默认设备配置")
                    app.add_log_entry("⚠️ 示例YAML配置加载失败，尝试默认配置", "WARNING")
                    load_default_device_config(app, logger)
                    
            except Exception as e:
                logger.error(f"YAML配置加载失败: {e}")
                app.add_log_entry(f"❌ YAML配置加载失败: {e}", "ERROR")
                load_default_device_config(app, logger)
        
        elif config.should_auto_load_devices():
            logger.info("启动时自动加载默认设备配置...")
            load_default_device_config(app, logger)
            
        else:
            logger.info("未启用设备自动加载")
            app.add_log_entry("ℹ️ 未启用设备自动加载", "INFO")
            
    except Exception as e:
        logger.error(f"自动加载设备失败: {e}", exc_info=True)
        app.add_log_entry(f"❌ 自动加载设备失败: {e}", "ERROR")


def load_default_device_config(app, logger):
    """加载默认设备配置"""
    device_list = config.get_default_device_list()
    if device_list:
        logger.info(f"准备加载默认设备: {device_list}")
        
        # 调用应用程序的加载配置方法
        try:
            app.load_default_configuration()
            logger.info("默认设备配置加载成功")
            app.add_log_entry("✅ 默认设备配置已自动加载", "SUCCESS")
            
            # 显示加载的设备列表
            for device_name in device_list:
                app.add_log_entry(f"📱 已加载设备: {device_name}", "INFO")
            
            # 如果启用了自动连接，则尝试连接设备
            if config.get("devices.auto_connect", False):
                logger.info("开始自动连接设备...")
                app.add_log_entry("🔗 开始自动连接设备...", "INFO")
                
                # 延迟连接，确保设备加载完成
                app.root.after(2000, lambda: app.connect_all_devices())
                
        except Exception as e:
            logger.warning(f"默认设备配置加载失败: {e}")
            app.add_log_entry("⚠️ 默认设备配置加载失败", "WARNING")
            
    else:
        logger.info("没有配置默认设备列表")
        app.add_log_entry("ℹ️ 没有配置默认设备列表", "INFO")


def check_and_setup_first_run(app):
    """检查并设置首次运行"""
    logger = logging.getLogger(__name__)
    
    try:
        if config.should_auto_load_sample():
            logger.info("检测到首次运行，准备自动加载示例项目...")
            
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
            
            result = msgbox.showinfo("欢迎", welcome_msg)
            
            # 自动加载示例项目
            sample_files = list(config.get_sample_project_path().glob("*.c"))
            sample_files.extend(list(config.get_sample_project_path().glob("*.h")))
            
            if sample_files:
                logger.info(f"找到 {len(sample_files)} 个示例文件")
                
                # 加载第一个示例文件
                sample_file = sample_files[0]
                logger.info(f"自动加载示例文件: {sample_file}")
                
                # 调用应用程序的加载项目方法
                success = app.load_sample_project(str(sample_file))
                
                if success:
                    logger.info("示例项目加载成功")
                    app.add_log_entry("✅ 示例项目已自动加载", "SUCCESS")
                    app.add_log_entry(f"📁 加载文件: {sample_file.name}", "INFO")
                    
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
                    app.add_log_entry("⚠️ 示例项目加载失败，使用默认配置", "WARNING")
            else:
                logger.warning("未找到示例项目文件")
                app.add_log_entry("⚠️ 未找到示例项目文件", "WARNING")
            
            # 标记首次运行完成
            config.mark_first_run_completed()
            
        else:
            logger.info("非首次运行，跳过自动加载示例项目")
            
    except Exception as e:
        logger.error(f"首次运行设置失败: {e}", exc_info=True)
        # 即使失败也要标记首次运行完成，避免重复尝试
        config.mark_first_run_completed()


def main():
    """主函数"""
    try:
        # 设置日志
        setup_logging()
        logger = logging.getLogger(__name__)
        
        logger.info("VCU测试开关桩管理工具启动")
        
        # 验证配置
        if not config.validate():
            logger.warning("配置验证失败，使用默认配置")
        
        # 创建主窗口
        root = tk.Tk()
        app = VCUControllerApp(root)
        
        logger.info("GUI应用程序已启动")
        
        # 检查首次运行并自动加载示例
        root.after(1000, lambda: check_and_setup_first_run(app))  # 延迟1秒执行，确保GUI完全加载
        
        # 自动加载设备配置
        root.after(1500, lambda: auto_load_devices(app))  # 延迟1.5秒执行，确保首次运行检查完成后再加载设备
        
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