#!/usr/bin/env python3
"""
VCU测试开关桩管理工具 - 配置检查和修复工具

用于诊断和修复自动加载配置问题
"""

import json
import os
from pathlib import Path
import sys


class ConfigChecker:
    """配置检查器"""
    
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.config_dir = self.project_root / "config"
        self.issues = []
        self.fixes = []
    
    def check_all(self):
        """执行所有检查"""
        print("🔍 开始检查 VCU测试开关桩管理工具配置...")
        print("=" * 50)
        
        self.check_config_directory()
        self.check_default_config()
        self.check_devices_config()
        self.check_sample_project()
        self.check_first_run_marker()
        
        self.print_summary()
        
        if self.issues:
            self.offer_fixes()
    
    def check_config_directory(self):
        """检查配置目录"""
        print("📁 检查配置目录...")
        
        if not self.config_dir.exists():
            self.issues.append("配置目录不存在")
            self.fixes.append(("create_config_dir", "创建配置目录"))
        else:
            print("✅ 配置目录存在")
    
    def check_default_config(self):
        """检查默认配置文件"""
        print("⚙️ 检查默认配置...")
        
        default_config_path = self.config_dir / "default.json"
        
        if not default_config_path.exists():
            self.issues.append("默认配置文件不存在")
            self.fixes.append(("create_default_config", "创建默认配置文件"))
            return
        
        try:
            with open(default_config_path, 'r', encoding='utf-8') as f:
                config = json.load(f)
            
            # 检查自动加载配置
            startup_auto_load = config.get("devices", {}).get("startup_auto_load", False)
            auto_connect = config.get("devices", {}).get("auto_connect", False)
            default_device_list = config.get("devices", {}).get("default_device_list", [])
            
            if not startup_auto_load:
                self.issues.append("启动时自动加载被禁用")
                self.fixes.append(("enable_auto_load", "启用自动加载"))
            else:
                print("✅ 启动时自动加载已启用")
            
            if not auto_connect:
                print("⚠️ 自动连接被禁用（可选功能）")
            else:
                print("✅ 自动连接已启用")
            
            if not default_device_list:
                self.issues.append("默认设备列表为空")
                self.fixes.append(("set_default_devices", "设置默认设备列表"))
            else:
                print(f"✅ 默认设备列表: {', '.join(default_device_list)}")
                
        except Exception as e:
            self.issues.append(f"默认配置文件格式错误: {e}")
            self.fixes.append(("fix_default_config", "修复默认配置文件"))
    
    def check_devices_config(self):
        """检查设备配置文件"""
        print("🖥️ 检查设备配置...")
        
        devices_config_path = self.config_dir / "devices.json"
        
        if not devices_config_path.exists():
            self.issues.append("设备配置文件不存在")
            self.fixes.append(("create_devices_config", "创建设备配置文件"))
            return
        
        try:
            with open(devices_config_path, 'r', encoding='utf-8') as f:
                config = json.load(f)
            
            devices = config.get("devices", {})
            if not devices:
                self.issues.append("设备配置为空")
                self.fixes.append(("add_sample_devices", "添加示例设备"))
            else:
                print(f"✅ 找到 {len(devices)} 个设备配置")
                for device_id, device_info in devices.items():
                    enabled = device_info.get("enabled", True)
                    status = "启用" if enabled else "禁用"
                    print(f"   - {device_id}: {device_info.get('name', '未命名')} ({status})")
                    
        except Exception as e:
            self.issues.append(f"设备配置文件格式错误: {e}")
            self.fixes.append(("fix_devices_config", "修复设备配置文件"))
    
    def check_sample_project(self):
        """检查示例项目"""
        print("📄 检查示例项目...")
        
        sample_dir = self.project_root / "sample_project"
        sample_files = []
        
        if sample_dir.exists():
            c_extensions = {'.c', '.h', '.cpp', '.hpp'}
            sample_files = [f for f in sample_dir.glob('*') 
                           if f.suffix.lower() in c_extensions]
        
        if not sample_files:
            self.issues.append("示例项目文件不存在")
            self.fixes.append(("create_sample_project", "创建示例项目"))
        else:
            print(f"✅ 找到 {len(sample_files)} 个示例文件")
            for file in sample_files:
                print(f"   - {file.name}")
    
    def check_first_run_marker(self):
        """检查首次运行标记"""
        print("🚀 检查首次运行状态...")
        
        marker_path = self.config_dir / ".first_run_completed"
        
        if marker_path.exists():
            print("ℹ️ 首次运行已完成（不会自动加载示例项目）")
            print("   如需重新体验首次运行，可删除此标记文件")
        else:
            print("✅ 首次运行标记不存在（将触发示例项目自动加载）")
    
    def print_summary(self):
        """打印检查摘要"""
        print("\n" + "=" * 50)
        print("📊 检查结果摘要")
        print("=" * 50)
        
        if not self.issues:
            print("🎉 恭喜！所有配置都正常，自动加载功能应该正常工作。")
            print("\n💡 如果仍然遇到问题，请检查：")
            print("   1. Python版本是否为 3.8+")
            print("   2. 是否有权限读写配置文件")
            print("   3. 程序启动时是否有错误日志")
        else:
            print(f"❌ 发现 {len(self.issues)} 个问题：")
            for i, issue in enumerate(self.issues, 1):
                print(f"   {i}. {issue}")
    
    def offer_fixes(self):
        """提供修复选项"""
        print("\n🔧 可用的修复选项：")
        print("=" * 50)
        
        for i, (fix_id, description) in enumerate(self.fixes, 1):
            print(f"   {i}. {description}")
        
        print("\n选择修复选项：")
        print("A - 自动修复所有问题")
        print("数字 - 修复特定问题")
        print("S - 跳过修复")
        
        choice = input("\n请输入选择 (A/数字/S): ").strip().upper()
        
        if choice == 'A':
            self.apply_all_fixes()
        elif choice == 'S':
            print("跳过修复，程序退出。")
        elif choice.isdigit():
            index = int(choice) - 1
            if 0 <= index < len(self.fixes):
                fix_id, description = self.fixes[index]
                print(f"正在执行: {description}")
                self.apply_fix(fix_id)
            else:
                print("无效的选择。")
    
    def apply_all_fixes(self):
        """应用所有修复"""
        print("🔧 正在自动修复所有问题...")
        
        for fix_id, description in self.fixes:
            print(f"   正在执行: {description}")
            self.apply_fix(fix_id)
        
        print("✅ 所有修复已完成！")
        print("💡 请重新启动应用程序以体验自动加载功能。")
    
    def apply_fix(self, fix_id):
        """应用特定修复"""
        if fix_id == "create_config_dir":
            self.config_dir.mkdir(parents=True, exist_ok=True)
            
        elif fix_id == "create_default_config":
            default_config = {
                "version": "2.0.0",
                "app": {
                    "name": "VCU测试开关桩管理工具",
                    "window_title": "VCU测试开关桩管理工具 v2.0.0"
                },
                "devices": {
                    "startup_auto_load": True,
                    "auto_connect": True,
                    "default_device_list": ["MVCU1_2", "MVCU1_16", "SVCU1_3"]
                },
                "vcu": {
                    "udp_port": 18125,
                    "timeout": 5.0
                }
            }
            with open(self.config_dir / "default.json", 'w', encoding='utf-8') as f:
                json.dump(default_config, f, indent=2, ensure_ascii=False)
                
        elif fix_id == "enable_auto_load":
            config_path = self.config_dir / "default.json"
            with open(config_path, 'r', encoding='utf-8') as f:
                config = json.load(f)
            
            if "devices" not in config:
                config["devices"] = {}
            config["devices"]["startup_auto_load"] = True
            config["devices"]["auto_connect"] = True
            
            with open(config_path, 'w', encoding='utf-8') as f:
                json.dump(config, f, indent=2, ensure_ascii=False)
                
        elif fix_id == "set_default_devices":
            config_path = self.config_dir / "default.json"
            with open(config_path, 'r', encoding='utf-8') as f:
                config = json.load(f)
            
            if "devices" not in config:
                config["devices"] = {}
            config["devices"]["default_device_list"] = ["MVCU1_2", "MVCU1_16", "SVCU1_3"]
            
            with open(config_path, 'w', encoding='utf-8') as f:
                json.dump(config, f, indent=2, ensure_ascii=False)
                
        elif fix_id == "create_devices_config":
            devices_config = {
                "description": "VCU设备配置文件",
                "devices": {
                    "MVCU1_2": {
                        "slot_id": "2",
                        "name": "主控单元MVCU1_2",
                        "ip": "20.2.1.10",
                        "local_ip": "20.2.1.100",
                        "port": 18125,
                        "enabled": True
                    },
                    "MVCU1_16": {
                        "slot_id": "16",
                        "name": "主控单元MVCU1_16",
                        "ip": "20.1.1.10",
                        "local_ip": "20.1.1.100",
                        "port": 18125,
                        "enabled": True
                    },
                    "SVCU1_3": {
                        "slot_id": "3",
                        "name": "从控单元SVCU1_3",
                        "ip": "20.2.1.10",
                        "local_ip": "20.2.1.101",
                        "port": 18125,
                        "enabled": True
                    }
                },
                "switch_groups": {
                    "system_flags": {
                        "name": "系统标志",
                        "switches": [
                            {
                                "name": "SYSTEM_INIT_COMPLETE_FLAG",
                                "type": "macro",
                                "description": "系统初始化完成标志"
                            }
                        ]
                    }
                }
            }
            with open(self.config_dir / "devices.json", 'w', encoding='utf-8') as f:
                json.dump(devices_config, f, indent=2, ensure_ascii=False)
                
        elif fix_id == "create_sample_project":
            sample_dir = self.project_root / "sample_project"
            sample_dir.mkdir(exist_ok=True)
            
            sample_content = '''/**
 * @file system_flags.c
 * @brief 系统标志和开关桩定义 - 示例文件
 */

// 系统初始化标志
#define SYSTEM_INIT_COMPLETE_FLAG    1    // 系统初始化完成标志
#define SYSTEM_RUNNING_FLAG          1    // 系统运行状态标志
#define SYSTEM_ERROR_FLAG            0    // 系统错误标志

// 网络通信开关
#define UDP_COMM_ENABLE             1     // UDP通信启用
#define TCP_COMM_ENABLE             0     // TCP通信启用

// 数据处理开关
#define DATA_CRC_CHECK_ENABLE       1     // CRC校验启用
#define DEBUG_LOG_ENABLE            0     // 调试日志启用
'''
            
            with open(sample_dir / "system_flags.c", 'w', encoding='utf-8') as f:
                f.write(sample_content)


def main():
    """主函数"""
    print("VCU测试开关桩管理工具 - 配置检查器")
    print("版本: 1.0.0")
    print()
    
    checker = ConfigChecker()
    checker.check_all()
    
    print("\n" + "=" * 50)
    print("检查完成！")
    print("如有问题，可以重新运行此工具进行修复。")
    
    input("\n按回车键退出...")


if __name__ == "__main__":
    main() 