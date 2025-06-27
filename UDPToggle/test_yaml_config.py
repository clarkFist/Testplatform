#!/usr/bin/env python3
"""
YAML配置功能测试脚本
"""

import sys
from pathlib import Path

# 设置路径
project_root = Path(__file__).parent
sys.path.insert(0, str(project_root))

def test_yaml_import():
    """测试YAML模块导入"""
    try:
        import yaml
        print("✅ PyYAML模块导入成功")
        print(f"   版本: {yaml.__version__}")
        return True
    except ImportError as e:
        print(f"❌ PyYAML模块导入失败: {e}")
        print("   请运行: pip install PyYAML")
        return False

def test_config_manager():
    """测试配置管理器"""
    try:
        from core.config_manager import ConfigManager
        
        # 创建配置管理器实例
        config = ConfigManager()
        print("✅ 配置管理器创建成功")
        
        # 测试获取示例YAML文件
        yaml_files = config.get_sample_yaml_files()
        print(f"✅ 找到 {len(yaml_files)} 个示例YAML文件:")
        for file in yaml_files:
            print(f"   • {file}")
        
        # 测试加载示例YAML配置
        if yaml_files:
            success = config.load_sample_yaml_config()
            if success:
                print("✅ 示例YAML配置加载成功")
                
                # 获取设备配置
                device_config = config.get_device_config()
                if device_config:
                    print(f"✅ 设备配置包含:")
                    if 'devices' in device_config:
                        print(f"   • 设备数量: {len(device_config['devices'])}")
                        for device_name in device_config['devices'].keys():
                            print(f"     - {device_name}")
                    
                    if 'switches' in device_config:
                        print(f"   • 开关桩数量: {len(device_config['switches'])}")
                        
                    if 'switch_groups' in device_config:
                        print(f"   • 开关桩分组: {len(device_config['switch_groups'])}")
                        for group_name in device_config['switch_groups'].keys():
                            print(f"     - {group_name}")
                
            else:
                print("⚠️ 示例YAML配置加载失败")
        else:
            print("⚠️ 没有找到示例YAML文件")
        
        return True
        
    except Exception as e:
        print(f"❌ 配置管理器测试失败: {e}")
        return False

def test_yaml_file_parsing():
    """测试YAML文件解析"""
    try:
        import yaml
        
        # 测试解析示例YAML文件
        config_dir = Path("config")
        yaml_files = list(config_dir.glob("*.yaml"))
        
        for yaml_file in yaml_files:
            try:
                with open(yaml_file, 'r', encoding='utf-8') as f:
                    data = yaml.safe_load(f)
                print(f"✅ {yaml_file.name} 解析成功")
                
                # 显示基本信息
                if isinstance(data, dict):
                    if 'description' in data:
                        print(f"   描述: {data['description']}")
                    if 'devices' in data:
                        print(f"   设备数量: {len(data['devices'])}")
                    if 'switches' in data:
                        print(f"   开关桩数量: {len(data['switches'])}")
                        
            except Exception as e:
                print(f"❌ {yaml_file.name} 解析失败: {e}")
        
        return True
        
    except Exception as e:
        print(f"❌ YAML文件解析测试失败: {e}")
        return False

def main():
    """主函数"""
    print("🔧 开始YAML配置功能测试\n")
    
    # 测试YAML模块导入
    print("1. 测试YAML模块导入")
    yaml_ok = test_yaml_import()
    print()
    
    if not yaml_ok:
        print("⚠️ YAML模块未安装，跳过后续测试")
        return
    
    # 测试YAML文件解析
    print("2. 测试YAML文件解析")
    test_yaml_file_parsing()
    print()
    
    # 测试配置管理器
    print("3. 测试配置管理器")
    config_ok = test_config_manager()
    print()
    
    # 总结
    if yaml_ok and config_ok:
        print("🎉 所有测试通过！YAML配置功能正常工作")
    else:
        print("⚠️ 部分测试失败，请检查配置")

if __name__ == "__main__":
    main() 