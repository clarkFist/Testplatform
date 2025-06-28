#!/usr/bin/env python3
"""
VCU测试开关桩管理工具 - 主程序

专业的VCU测试开关桩管理工具，基于UDP通信协议实现对VCU设备的远程控制和监控。

主要特性：
- 支持YAML格式的配置文件管理
- 自动加载设备配置和开关桩配置
- 支持首次运行示例配置自动加载
- 灵活的配置验证和错误处理
- 现代化的日志系统

配置文件格式：
- config/default.yaml: 应用默认配置
- config/devices_example.yaml: 设备配置示例
- config/devices.yaml: 用户自定义设备配置
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
from core.vcu_controller import vcu_controller


def setup_logging():
    """设置日志系统"""
    log_dir = Path("logs")
    log_dir.mkdir(exist_ok=True)
    
    # 获取日志级别配置
    log_level = config.get("logging.level", "INFO")
    console_output = config.get("logging.console_output", True)
    file_output = config.get("logging.file_output", True)
    
    # 设置日志级别
    level_map = {
        "DEBUG": logging.DEBUG,
        "INFO": logging.INFO,
        "WARNING": logging.WARNING,
        "ERROR": logging.ERROR,
        "CRITICAL": logging.CRITICAL
    }
    level = level_map.get(log_level.upper(), logging.INFO)
    
    # 配置处理器列表
    handlers = []
    
    # 文件处理器
    if file_output:
        file_handler = logging.FileHandler(
            log_dir / "vcu_controller.log", 
            encoding='utf-8'
        )
        file_handler.setLevel(level)
        handlers.append(file_handler)
    
    # 控制台处理器
    if console_output:
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(level)
        handlers.append(console_handler)
    
    # 配置日志格式
    logging.basicConfig(
        level=level,
        format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
        datefmt="%Y-%m-%d %H:%M:%S",
        handlers=handlers,
        force=True  # 强制重新配置
    )
    
    # 设置第三方库日志级别
    logging.getLogger('matplotlib').setLevel(logging.WARNING)
    logging.getLogger('PIL').setLevel(logging.WARNING)
    
    # 记录日志系统初始化信息
    logger = logging.getLogger(__name__)
    logger.info(f"📝 日志系统已初始化，级别: {log_level}")
    logger.info(f"📁 日志目录: {log_dir.absolute()}")
    if file_output:
        logger.info(f"📄 日志文件: {log_dir / 'vcu_controller.log'}")
    if console_output:
        logger.info("🖥️ 控制台输出: 已启用")


def auto_load_devices(app):
    """自动加载设备配置 - 支持YAML格式"""
    logger = logging.getLogger(__name__)
    
    try:
        # 首先尝试自动加载YAML配置
        if config.get("devices.auto_load_yaml_samples", False) or config.get("devices.auto_load_yaml", True):
            logger.info("启动时自动加载YAML配置...")
            app.add_log_entry("🔄 正在自动加载YAML配置...", "INFO")
            
            try:
                # 优先加载示例YAML配置
                success = False
                if hasattr(config, 'load_sample_yaml_config'):
                    success = config.load_sample_yaml_config()
                
                # 如果示例配置加载失败，尝试加载默认YAML配置
                if not success and hasattr(config, 'load_yaml_config'):
                    success = config.load_yaml_config()
                
                # 如果都没有专门的YAML加载方法，使用通用加载方法
                if not success and hasattr(config, 'load_config'):
                    # 尝试加载devices_example.yaml或default.yaml
                    yaml_files = [
                        "config/devices_example.yaml",
                        "config/default.yaml",
                        "config/devices.yaml"
                    ]
                    for yaml_file in yaml_files:
                        try:
                            success = config.load_config(yaml_file)
                            if success:
                                logger.info(f"成功加载YAML配置文件: {yaml_file}")
                                break
                        except Exception as e:
                            logger.debug(f"尝试加载{yaml_file}失败: {e}")
                            continue
                
                if success:
                    logger.info("YAML配置加载成功")
                    app.add_log_entry("✅ YAML配置已自动加载", "SUCCESS")
                    
                    # 显示加载的设备配置
                    device_config = config.get_device_config() if hasattr(config, 'get_device_config') else None
                    if not device_config:
                        # 尝试从主配置中获取设备信息
                        device_config = {"devices": config.get("devices", {})}
                    
                    if device_config and 'devices' in device_config and device_config['devices']:
                        device_count = len(device_config['devices'])
                        app.add_log_entry(f"📱 已加载 {device_count} 个设备配置", "INFO")
                        
                        # 显示设备列表
                        for device_name in device_config['devices'].keys():
                            device_info = device_config['devices'][device_name]
                            slot_id = device_info.get('slot_id', 'N/A')
                            ip = device_info.get('ip', 'N/A')
                            app.add_log_entry(f"  • {device_name} (Slot: {slot_id}, IP: {ip})", "INFO")
                        
                        # 显示开关桩配置
                        switches = device_config.get('switches', config.get('switches', {}))
                        if switches:
                            switch_count = len(switches)
                            app.add_log_entry(f"🔧 已加载 {switch_count} 个开关桩配置", "INFO")
                        
                        # 显示分组配置
                        switch_groups = device_config.get('switch_groups', config.get('switch_groups', {}))
                        if switch_groups:
                            group_count = len(switch_groups)
                            app.add_log_entry(f"📂 已加载 {group_count} 个开关桩分组", "INFO")
                        
                        # 检查设备是否已经加载
                        existing_devices = vcu_controller.get_all_devices()
                        
                        if len(existing_devices) == 0:
                            # 只有在没有设备时才加载
                            app.add_log_entry("🔄 正在将设备添加到VCU控制器...", "INFO")
                            if hasattr(app, 'load_default_configuration'):
                                app.load_default_configuration()
                            elif hasattr(app, 'load_devices_from_config'):
                                app.load_devices_from_config()
                        else:
                            app.add_log_entry(f"ℹ️ 设备已存在，跳过重复加载 ({len(existing_devices)} 个设备)", "INFO")
                        
                        # 如果启用了自动连接，则尝试连接设备
                        if config.get("devices.auto_connect", False):
                            logger.info("开始自动连接设备...")
                            app.add_log_entry("🔗 开始自动连接设备...", "INFO")
                            
                            # 延迟连接，确保设备加载完成
                            app.root.after(2000, lambda: app.connect_all_devices())
                    else:
                        app.add_log_entry("⚠️ YAML配置中未找到设备信息", "WARNING")
                        logger.warning("YAML配置中未找到设备信息，尝试加载默认配置")
                        load_default_device_config(app, logger)
                        
                else:
                    logger.warning("YAML配置加载失败，尝试加载默认设备配置")
                    app.add_log_entry("⚠️ YAML配置加载失败，尝试默认配置", "WARNING")
                    load_default_device_config(app, logger)
                    
            except Exception as e:
                logger.error(f"YAML配置加载失败: {e}")
                app.add_log_entry(f"❌ YAML配置加载失败: {e}", "ERROR")
                load_default_device_config(app, logger)
        
        elif config.should_auto_load_devices() if hasattr(config, 'should_auto_load_devices') else config.get("devices.auto_load", False):
            logger.info("启动时自动加载默认设备配置...")
            app.add_log_entry("🔄 正在自动加载默认设备配置...", "INFO")
            load_default_device_config(app, logger)
            
        else:
            logger.info("未启用设备自动加载")
            app.add_log_entry("ℹ️ 未启用设备自动加载", "INFO")
            app.add_log_entry("💡 您可以通过菜单加载YAML设备配置文件或项目文件", "INFO")
            
    except Exception as e:
        logger.error(f"自动加载设备失败: {e}", exc_info=True)
        app.add_log_entry(f"❌ 自动加载设备失败: {e}", "ERROR")


def load_default_device_config(app, logger):
    """加载默认设备配置 - 支持YAML格式"""
    # 首先尝试从YAML配置中获取设备列表
    device_list = None
    if hasattr(config, 'get_default_device_list'):
        device_list = config.get_default_device_list()
    
    # 如果没有专门的方法，尝试从配置中直接获取
    if not device_list:
        devices_config = config.get("devices", {})
        if devices_config:
            device_list = list(devices_config.keys())
    
    if device_list:
        logger.info(f"准备加载默认设备: {device_list}")
        
        # 调用应用程序的加载配置方法
        try:
            if hasattr(app, 'load_default_configuration'):
                app.load_default_configuration()
            elif hasattr(app, 'load_devices_from_config'):
                app.load_devices_from_config()
            elif hasattr(app, 'load_device_list_yaml'):
                app.load_device_list_yaml()
            else:
                logger.warning("应用程序没有可用的配置加载方法")
                app.add_log_entry("⚠️ 应用程序没有可用的配置加载方法", "WARNING")
                return
                
            logger.info("默认设备配置加载成功")
            app.add_log_entry("✅ 默认设备配置已自动加载", "SUCCESS")
            
            # 显示加载的设备列表
            for device_name in device_list:
                # 尝试获取设备详细信息
                device_info = config.get(f"devices.{device_name}", {})
                if device_info:
                    slot_id = device_info.get('slot_id', 'N/A')
                    ip = device_info.get('ip', 'N/A')
                    app.add_log_entry(f"📱 已加载设备: {device_name} (Slot: {slot_id}, IP: {ip})", "INFO")
                else:
                    app.add_log_entry(f"📱 已加载设备: {device_name}", "INFO")
            
            # 如果启用了自动连接，则尝试连接设备
            if config.get("devices.auto_connect", False):
                logger.info("开始自动连接设备...")
                app.add_log_entry("🔗 开始自动连接设备...", "INFO")
                
                # 延迟连接，确保设备加载完成
                app.root.after(2000, lambda: app.connect_all_devices())
                
        except Exception as e:
            logger.warning(f"默认设备配置加载失败: {e}")
            app.add_log_entry(f"⚠️ 默认设备配置加载失败: {e}", "WARNING")
            
    else:
        logger.info("没有配置默认设备列表")
        app.add_log_entry("ℹ️ 没有配置默认设备列表", "INFO")
        app.add_log_entry("💡 请通过菜单加载YAML设备配置文件或项目文件", "INFO")


def check_and_setup_first_run(app):
    """检查并设置首次运行 - 支持YAML配置"""
    logger = logging.getLogger(__name__)
    
    try:
        # 检查是否应该自动加载示例
        should_load_sample = False
        if hasattr(config, 'should_auto_load_sample'):
            should_load_sample = config.should_auto_load_sample()
        else:
            should_load_sample = config.get("app.first_run", True) or config.get("app.auto_load_sample", False)
        
        if should_load_sample:
            logger.info("检测到首次运行，准备自动加载示例项目...")
            
            # 显示欢迎信息
            import tkinter.messagebox as msgbox
            welcome_msg = (
                "欢迎使用VCU测试开关桩管理工具！\n\n"
                "检测到这是您的首次运行，系统将自动加载示例配置\n"
                "以便您快速了解工具的功能和使用方法。\n\n"
                "示例配置包含：\n"
                "• 预配置的VCU设备 (MVCU1_2, MVCU1_16, SVCU1_3)\n"
                "• 丰富的开关桩示例\n"
                "• YAML格式的配置文件\n"
                "• 不同类型的配置演示\n\n"
                "点击确定继续..."
            )
            
            result = msgbox.showinfo("欢迎", welcome_msg)
            
            # 首先尝试加载YAML示例配置
            yaml_loaded = False
            try:
                if hasattr(config, 'load_sample_yaml_config'):
                    yaml_loaded = config.load_sample_yaml_config()
                    if yaml_loaded:
                        logger.info("示例YAML配置加载成功")
                        app.add_log_entry("✅ 示例YAML配置已自动加载", "SUCCESS")
                        
                        # 调用应用程序的配置加载方法
                        if hasattr(app, 'load_default_configuration'):
                            app.load_default_configuration()
                        
                        success_msg = (
                            "示例YAML配置加载成功！\n\n"
                            "已加载预配置的VCU设备和开关桩配置。\n"
                            "您现在可以：\n"
                            "• 查看左侧的设备列表\n"
                            "• 查看右侧的开关桩列表\n"
                            "• 尝试连接VCU设备\n"
                            "• 体验开关桩的操作功能\n\n"
                            "如需加载其他配置文件，请使用菜单中的相关功能。"
                        )
                        msgbox.showinfo("配置加载成功", success_msg)
            except Exception as e:
                logger.warning(f"YAML配置加载失败: {e}")
                app.add_log_entry(f"⚠️ YAML配置加载失败: {e}", "WARNING")
            
            # 如果YAML配置加载失败，尝试加载示例项目文件
            if not yaml_loaded:
                logger.info("尝试加载示例项目文件...")
                sample_project_loaded = False
                
                try:
                    if hasattr(config, 'get_sample_project_path'):
                        sample_path = config.get_sample_project_path()
                        sample_files = list(sample_path.glob("*.c"))
                        sample_files.extend(list(sample_path.glob("*.h")))
                        
                        if sample_files:
                            logger.info(f"找到 {len(sample_files)} 个示例文件")
                            
                            # 加载第一个示例文件
                            sample_file = sample_files[0]
                            logger.info(f"自动加载示例文件: {sample_file}")
                            
                            # 调用应用程序的加载项目方法
                            if hasattr(app, 'load_sample_project'):
                                success = app.load_sample_project(str(sample_file))
                                
                                if success:
                                    sample_project_loaded = True
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
                            logger.warning("未找到示例项目文件")
                            app.add_log_entry("⚠️ 未找到示例项目文件", "WARNING")
                except Exception as e:
                    logger.warning(f"示例项目文件加载失败: {e}")
                    app.add_log_entry(f"⚠️ 示例项目文件加载失败: {e}", "WARNING")
                
                if not sample_project_loaded:
                    logger.warning("所有示例加载尝试都失败，使用默认配置")
                    app.add_log_entry("⚠️ 示例加载失败，使用默认配置", "WARNING")
            
            # 标记首次运行完成
            if hasattr(config, 'mark_first_run_completed'):
                config.mark_first_run_completed()
            else:
                # 如果没有专门的方法，尝试设置配置
                config.set("app.first_run", False)
                if hasattr(config, 'save_config'):
                    config.save_config()
            
        else:
            logger.info("非首次运行，跳过自动加载示例项目")
            
    except Exception as e:
        logger.error(f"首次运行设置失败: {e}", exc_info=True)
        # 即使失败也要标记首次运行完成，避免重复尝试
        if hasattr(config, 'mark_first_run_completed'):
            config.mark_first_run_completed()
        else:
            config.set("app.first_run", False)


def main():
    """主函数"""
    try:
        # 设置日志
        setup_logging()
        logger = logging.getLogger(__name__)
        
        logger.info("🚀 VCU测试开关桩管理工具正在启动...")
        
        # 显示配置加载状态
        try:
            config_status = None
            if hasattr(config, 'get_config_status'):
                config_status = config.get_config_status()
            
            logger.info("=" * 60)
            logger.info("📋 配置系统状态检查 (YAML格式):")
            
            if config_status:
                logger.info(f"  配置加载状态: {'✅ 成功' if config_status.get('loaded', False) else '❌ 失败'}")
                logger.info(f"  配置来源: {config_status.get('source', '未知')}")
                logger.info(f"  系统版本: {config_status.get('version', '未知')}")
                logger.info(f"  配置验证: {'✅ 通过' if config_status.get('validation_passed', False) else '❌ 失败'}")
                
                # 显示YAML配置特定信息
                if 'yaml_files' in config_status:
                    yaml_files = config_status['yaml_files']
                    logger.info(f"  YAML文件: {', '.join(yaml_files) if yaml_files else '无'}")
            else:
                # 如果没有状态信息，尝试获取基本配置信息
                logger.info("  配置管理器: ✅ 已初始化")
                logger.info(f"  应用名称: {config.get('app.name', 'VCU测试开关桩管理工具')}")
                logger.info(f"  应用版本: {config.get('app.version', config.get('version', '2.0.0'))}")
                logger.info("  配置格式: YAML")
                
                # 检查是否有设备配置
                devices = config.get('devices', {})
                logger.info(f"  预配置设备: {len(devices)} 个")
                
                # 检查是否有开关桩配置
                switches = config.get('switches', {})
                logger.info(f"  预配置开关桩: {len(switches)} 个")
            
            logger.info("=" * 60)
        except Exception as e:
            logger.warning(f"获取配置状态失败: {e}")
            logger.info("=" * 60)
            logger.info("📋 配置系统状态检查:")
            logger.info("  配置管理器: ✅ 已初始化")
            logger.info("  配置格式: YAML")
            logger.info("=" * 60)
        
        # 验证配置
        config_valid = True
        try:
            if hasattr(config, 'validate'):
                config_valid = config.validate()
            if not config_valid:
                logger.warning("⚠️ 配置验证失败，使用默认配置继续运行")
        except Exception as e:
            logger.warning(f"⚠️ 配置验证过程出错: {e}")
        
        # 显示YAML配置文件信息
        logger.info("📄 YAML配置文件状态:")
        yaml_config_files = [
            ("默认配置", "config/default.yaml"),
            ("设备配置", "config/devices_example.yaml"),
            ("设备配置", "config/devices.yaml")
        ]
        
        for desc, file_path in yaml_config_files:
            from pathlib import Path
            yaml_file = Path(file_path)
            if yaml_file.exists():
                logger.info(f"  ✅ {desc}: {file_path}")
            else:
                logger.debug(f"  ❌ {desc}: {file_path} (不存在)")
        
        logger.info("💡 提示: 系统已切换到YAML配置格式，支持更灵活的配置管理")
        
        # 创建主窗口
        root = tk.Tk()
        app = VCUControllerApp(root)
        
        logger.info("🖥️ GUI应用程序已启动")
        
        # 延迟执行自动加载功能，确保GUI完全初始化
        # 检查首次运行并自动加载示例
        root.after(1000, lambda: check_and_setup_first_run(app))  # 延迟1秒执行，确保GUI完全加载
        
        # 自动加载设备配置
        root.after(1500, lambda: auto_load_devices(app))  # 延迟1.5秒执行，确保首次运行检查完成后再加载设备
        
        # 显示启动完成信息
        logger.info("✅ VCU测试开关桩管理工具启动完成!")
        logger.info(f"🎯 请在界面中查看系统状态和操作日志")
        
        # 启动主循环
        root.mainloop()
        
        logger.info("🛑 VCU测试开关桩管理工具退出")
        
    except KeyboardInterrupt:
        logger.info("⏹️ 收到中断信号，正在退出...")
    except Exception as e:
        logger.error(f"💥 应用程序异常: {e}", exc_info=True)
        sys.exit(1)


if __name__ == "__main__":
    main() 