"""
配置管理器

负责加载、验证和管理系统配置。支持YAML格式配置文件。
"""

import json
import yaml
import os
import time
from pathlib import Path
from typing import Dict, Any, Optional, List
import logging

logger = logging.getLogger(__name__)


class ConfigManager:
    """配置管理器类"""
    
    def __init__(self, config_dir: str = "config"):
        """
        初始化配置管理器
        
        Args:
            config_dir: 配置文件目录
        """
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        
        self._config: Dict[str, Any] = {}
        self._device_config: Dict[str, Any] = {}
        
        # 记录配置加载状态
        self._config_loaded = False
        self._config_source = ""
        
        # 自动加载配置
        self._auto_load_configurations()
    
    def _auto_load_configurations(self) -> None:
        """自动加载所有配置"""
        logger.info("🔧 开始加载系统配置...")
        
        # 加载系统配置
        self._load_default_config()
        
        # 加载设备配置
        self._load_device_config()
        
        # 验证配置
        if self.validate():
            logger.info(f"✅ 配置系统初始化完成，配置来源: {self._config_source}")
        else:
            logger.warning("⚠️ 配置验证失败，可能影响系统功能")
    
    def _load_default_config(self) -> None:
        """加载默认配置"""
        # 优先加载YAML配置
        yaml_config_path = self.config_dir / "default.yaml"
        json_config_path = self.config_dir / "default.json"
        
        # 尝试加载YAML配置
        if yaml_config_path.exists():
            try:
                logger.info(f"📄 正在加载YAML配置文件: {yaml_config_path}")
                with open(yaml_config_path, 'r', encoding='utf-8') as f:
                    self._config = yaml.safe_load(f)
                    
                if not self._config:
                    raise ValueError("配置文件为空或格式错误")
                
                self._config_loaded = True
                self._config_source = f"YAML配置文件 ({yaml_config_path})"
                logger.info(f"✅ YAML配置加载成功，版本: {self.get('version', '未知')}")
                
                # 显示关键配置信息
                self._log_key_config_info()
                return
                
            except Exception as e:
                logger.error(f"❌ 加载YAML配置失败: {e}")
                self._config = {}
        
        # 如果YAML不存在或加载失败，尝试加载JSON配置
        if json_config_path.exists():
            try:
                logger.info(f"📄 正在加载JSON配置文件: {json_config_path}")
                with open(json_config_path, 'r', encoding='utf-8') as f:
                    self._config = json.load(f)
                    
                self._config_loaded = True
                self._config_source = f"JSON配置文件 ({json_config_path})"
                logger.info(f"✅ JSON配置加载成功，版本: {self.get('version', '未知')}")
                self._log_key_config_info()
                return
                
            except Exception as e:
                logger.error(f"❌ 加载JSON配置失败: {e}")
        
        # 如果都失败了，创建默认配置文件
        logger.warning("⚠️ 未找到有效的配置文件，正在创建默认配置...")
        self._create_default_config_file()
    
    def _create_default_config_file(self) -> None:
        """创建默认配置文件"""
        try:
            self._config = self._get_fallback_config()
            
            # 创建默认的YAML配置文件
            default_yaml_path = self.config_dir / "default.yaml"
            with open(default_yaml_path, 'w', encoding='utf-8') as f:
                yaml.dump(self._config, f, default_flow_style=False, 
                         allow_unicode=True, indent=2, sort_keys=False)
            
            self._config_loaded = True
            self._config_source = f"自动创建的默认配置 ({default_yaml_path})"
            logger.info(f"✅ 默认配置文件已创建: {default_yaml_path}")
            self._log_key_config_info()
            
        except Exception as e:
            logger.error(f"❌ 创建默认配置文件失败: {e}")
            # 使用内存中的后备配置
            self._config = self._get_fallback_config()
            self._config_loaded = True
            self._config_source = "内置后备配置"
            logger.warning("⚠️ 使用内置后备配置")
    
    def _log_key_config_info(self) -> None:
        """记录关键配置信息"""
        try:
            app_name = self.get('app.name', '未知应用')
            udp_port = self.get('vcu.udp_port', 'N/A')
            server_port = self.get('server.port', 'N/A')
            
            logger.info(f"🏷️ 应用名称: {app_name}")
            logger.info(f"🌐 UDP端口: {udp_port}")
            logger.info(f"🖥️ 服务器端口: {server_port}")
            
            # 显示设备配置信息
            default_devices = self.get('devices.default_device_list', [])
            if default_devices:
                logger.info(f"📱 默认设备: {', '.join(default_devices)}")
            
            # 显示功能开关状态
            auto_connect = self.get('devices.auto_connect', False)
            auto_load_samples = self.get('devices.auto_load_yaml_samples', False)
            logger.info(f"🔗 自动连接设备: {'启用' if auto_connect else '禁用'}")
            logger.info(f"📂 自动加载示例: {'启用' if auto_load_samples else '禁用'}")
            
        except Exception as e:
            logger.warning(f"记录配置信息时出错: {e}")
    
    def _load_device_config(self) -> None:
        """加载设备配置"""
        # 优先加载YAML设备配置
        yaml_device_path = self.config_dir / "devices.yaml"
        json_device_path = self.config_dir / "devices.json"
        
        if yaml_device_path.exists():
            try:
                logger.info(f"📱 正在加载设备YAML配置: {yaml_device_path}")
                with open(yaml_device_path, 'r', encoding='utf-8') as f:
                    self._device_config = yaml.safe_load(f)
                logger.info("✅ 设备YAML配置加载成功")
                return
            except Exception as e:
                logger.error(f"❌ 加载设备YAML配置失败: {e}")
        
        # 如果YAML不存在，尝试加载JSON配置
        if json_device_path.exists():
            try:
                logger.info(f"📱 正在加载设备JSON配置: {json_device_path}")
                with open(json_device_path, 'r', encoding='utf-8') as f:
                    self._device_config = json.load(f)
                logger.info("✅ 设备JSON配置加载成功")
                return
            except Exception as e:
                logger.error(f"❌ 加载设备JSON配置失败: {e}")
        
        logger.info("ℹ️ 未找到设备配置文件，将使用空配置")
        self._device_config = {}

    def get_config_status(self) -> Dict[str, Any]:
        """
        获取配置状态信息
        
        Returns:
            配置状态字典
        """
        return {
            "loaded": self._config_loaded,
            "source": self._config_source,
            "version": self.get('version', '未知'),
            "config_file_exists": (self.config_dir / "default.yaml").exists(),
            "device_config_exists": bool(self._device_config),
            "validation_passed": self.validate()
        }
    
    def _get_fallback_config(self) -> Dict[str, Any]:
        """获取后备配置"""
        return {
            "version": "2.0.0",
            "server": {
                "host": "0.0.0.0",
                "port": 8080,
                "debug": True
            },
            "vcu": {
                "udp_port": 18125,
                "timeout": 5.0,
                "retry_count": 3,
                "heartbeat_interval": 10.0
            },
            "devices": {
                "slot_ip": {
                    "2": "20.2.1.10",
                    "16": "20.1.1.10"
                },
                "local_ip": {
                    "2": "20.2.1.100", 
                    "16": "20.1.1.100"
                }
            },
            "logging": {
                "level": "INFO",
                "max_file_size": "10MB",
                "backup_count": 5,
                "log_dir": "logs"
            },
            "features": {
                "auto_reconnect": True,
                "web_ui": True,
                "rest_api": True,
                "serial_support": True,
                "wireshark_integration": False
            }
        }
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        获取配置值
        
        Args:
            key: 配置键，支持点号分隔的嵌套键
            default: 默认值
            
        Returns:
            配置值
        """
        keys = key.split('.')
        value = self._config
        
        try:
            for k in keys:
                value = value[k]
            return value
        except (KeyError, TypeError):
            return default
    
    def set(self, key: str, value: Any) -> None:
        """
        设置配置值
        
        Args:
            key: 配置键，支持点号分隔的嵌套键
            value: 配置值
        """
        keys = key.split('.')
        config = self._config
        
        # 导航到目标位置
        for k in keys[:-1]:
            if k not in config:
                config[k] = {}
            config = config[k]
        
        # 设置值
        config[keys[-1]] = value
        logger.debug(f"配置已更新: {key} = {value}")
    
    def load_from_file(self, filepath: str) -> None:
        """
        从文件加载配置
        
        Args:
            filepath: 配置文件路径
        """
        try:
            file_path = Path(filepath)
            
            if file_path.suffix.lower() in ['.yaml', '.yml']:
                # 加载YAML文件
                with open(filepath, 'r', encoding='utf-8') as f:
                    config = yaml.safe_load(f)
            elif file_path.suffix.lower() == '.json':
                # 加载JSON文件
                with open(filepath, 'r', encoding='utf-8') as f:
                    config = json.load(f)
            else:
                # 默认尝试YAML格式
                with open(filepath, 'r', encoding='utf-8') as f:
                    config = yaml.safe_load(f)
            
            # 合并配置
            if 'devices' in config or 'switches' in config:
                # 这是设备配置文件
                self._device_config = config
            else:
                # 这是系统配置文件
                self._merge_config(config)
            
            logger.info(f"已加载配置文件: {filepath}")
            
        except Exception as e:
            logger.error(f"加载配置文件失败: {filepath}, 错误: {e}")
            raise
    
    def save_to_file(self, filepath: str, config_type: str = "system") -> None:
        """
        保存配置到文件
        
        Args:
            filepath: 配置文件路径
            config_type: 配置类型 ("system" 或 "device")
        """
        try:
            # 确保目录存在
            Path(filepath).parent.mkdir(parents=True, exist_ok=True)
            
            file_path = Path(filepath)
            config_data = self._config if config_type == "system" else self._device_config
            
            if file_path.suffix.lower() in ['.yaml', '.yml']:
                # 保存为YAML文件
                with open(filepath, 'w', encoding='utf-8') as f:
                    yaml.dump(config_data, f, default_flow_style=False, 
                             allow_unicode=True, indent=2, sort_keys=False)
            elif file_path.suffix.lower() == '.json':
                # 保存为JSON文件
                with open(filepath, 'w', encoding='utf-8') as f:
                    json.dump(config_data, f, indent=2, ensure_ascii=False)
            else:
                # 默认保存为YAML格式
                with open(filepath, 'w', encoding='utf-8') as f:
                    yaml.dump(config_data, f, default_flow_style=False, 
                             allow_unicode=True, indent=2, sort_keys=False)
            
            logger.info(f"{config_type}配置已保存到: {filepath}")
            
        except Exception as e:
            logger.error(f"保存配置文件失败: {filepath}, 错误: {e}")
            raise
    
    def _merge_config(self, new_config: Dict[str, Any]) -> None:
        """
        合并配置
        
        Args:
            new_config: 新配置
        """
        def merge_dict(base: Dict, update: Dict) -> Dict:
            """递归合并字典"""
            for key, value in update.items():
                if key in base and isinstance(base[key], dict) and isinstance(value, dict):
                    merge_dict(base[key], value)
                else:
                    base[key] = value
            return base
        
        self._config = merge_dict(self._config, new_config)
    
    def validate(self) -> bool:
        """
        验证配置是否有效
        
        Returns:
            是否有效
        """
        required_keys = [
            "server.host",
            "server.port", 
            "vcu.udp_port",
            "devices.slot_ip",
            "devices.local_ip"
        ]
        
        for key in required_keys:
            if self.get(key) is None:
                logger.error(f"必需的配置项缺失: {key}")
                return False
        
        # 验证端口范围
        server_port = self.get("server.port")
        if not (1024 <= server_port <= 65535):
            logger.error(f"服务器端口无效: {server_port}")
            return False
        
        udp_port = self.get("vcu.udp_port")
        if not (1024 <= udp_port <= 65535):
            logger.error(f"UDP端口无效: {udp_port}")
            return False
        
        logger.info("配置验证通过")
        return True
    
    def get_all(self) -> Dict[str, Any]:
        """
        获取所有配置
        
        Returns:
            配置字典
        """
        return self._config.copy()
    
    def reload(self) -> None:
        """重新加载配置"""
        self._load_default_config()
        logger.info("配置已重新加载")
    
    def should_auto_load_devices(self) -> bool:
        """
        检查是否应该自动加载设备配置
        
        Returns:
            是否应该自动加载
        """
        return self.get("devices.startup_auto_load", False)
    
    def get_default_device_list(self) -> List[str]:
        """
        获取默认设备列表
        
        Returns:
            设备名称列表
        """
        return self.get("devices.default_device_list", [])
    
    def is_first_run(self) -> bool:
        """检测是否是首次运行"""
        # 检查是否存在运行标记文件
        run_marker = self.config_dir / ".first_run_completed"
        first_run_config = self.get("app.first_run", True)
        
        # 只有当配置中标记为首次运行且没有完成标记文件时才认为是首次运行
        return first_run_config and not run_marker.exists()
    
    def mark_first_run_completed(self) -> None:
        """标记首次运行已完成"""
        run_marker = self.config_dir / ".first_run_completed"
        try:
            with open(run_marker, 'w', encoding='utf-8') as f:
                f.write(f"首次运行完成时间: {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
            
            # 同时更新配置文件中的首次运行标记
            self.set("app.first_run", False)
            try:
                default_yaml_path = self.config_dir / "default.yaml"
                if default_yaml_path.exists():
                    self.save_to_file(str(default_yaml_path), "system")
            except Exception as save_e:
                logger.warning(f"更新配置文件中的首次运行标记失败: {save_e}")
            
            logger.info("首次运行已标记为完成")
        except Exception as e:
            logger.error(f"标记首次运行完成失败: {e}")
    
    def get_sample_project_path(self) -> Path:
        """获取示例项目路径"""
        return Path("sample_project")
    
    def should_auto_load_sample(self) -> bool:
        """判断是否应该自动加载示例项目"""
        # 如果是首次运行，并且示例项目存在，则自动加载
        sample_path = self.get_sample_project_path()
        return self.is_first_run() and sample_path.exists()
    
    def get_device_config(self) -> Dict[str, Any]:
        """
        获取设备配置
        
        Returns:
            设备配置字典
        """
        # 如果有独立的设备配置文件，优先使用
        if self._device_config:
            return self._device_config.copy()
        
        # 否则从主配置文件中构建设备配置
        return self.build_device_config_from_main()
    
    def build_device_config_from_main(self) -> Dict[str, Any]:
        """
        从主配置文件构建设备配置
        
        Returns:
            设备配置字典
        """
        devices_config = {"devices": {}}
        
        # 获取设备定义
        device_definitions = self.get('device_definitions', {})
        default_device_list = self.get('devices.default_device_list', [])
        
        logger.info(f"开始从主配置构建设备配置，设备列表: {default_device_list}")
        logger.info(f"可用设备定义: {list(device_definitions.keys())}")
        
        # 为默认设备列表中的每个设备创建配置
        for device_name in default_device_list:
            if device_name in device_definitions:
                # 如果有完整定义，直接使用
                devices_config["devices"][device_name] = device_definitions[device_name].copy()
                logger.info(f"✅ 使用设备定义: {device_name}")
            else:
                # 否则尝试从旧格式构建
                slot_id = self._extract_slot_id_from_name(device_name)
                if slot_id:
                    slot_ip = self.get(f'devices.slot_ip.{slot_id}')
                    local_ip = self.get(f'devices.local_ip.{slot_id}')
                    
                    if slot_ip:
                        devices_config["devices"][device_name] = {
                            "slot_id": slot_id,
                            "name": self._generate_device_name(device_name, slot_id),
                            "ip": slot_ip,
                            "local_ip": local_ip or slot_ip,
                            "port": 18125,
                            "enabled": True,
                            "description": f"VCU设备，槽位{slot_id}"
                        }
                        logger.info(f"✅ 从槽位配置构建设备: {device_name} (槽位{slot_id})")
                    else:
                        logger.warning(f"⚠️ 设备 {device_name} 找不到槽位{slot_id}的IP配置")
                else:
                    logger.warning(f"⚠️ 无法从设备名称 {device_name} 提取槽位ID")
        
        logger.info(f"从主配置构建了 {len(devices_config['devices'])} 个设备配置")
        return devices_config
    
    def _extract_slot_id_from_name(self, device_name: str) -> Optional[str]:
        """
        从设备名称中提取槽位ID
        
        Args:
            device_name: 设备名称，如 "MVCU1_2"
            
        Returns:
            槽位ID或None
        """
        # 支持多种命名格式
        import re
        
        # 匹配 MVCU1_2, SVCU1_3 等格式
        match = re.search(r'[MS]VCU\d+_(\d+)', device_name)
        if match:
            return match.group(1)
        
        # 匹配 VCU_2, VCU2 等格式  
        match = re.search(r'VCU[_]?(\d+)', device_name)
        if match:
            return match.group(1)
        
        # 匹配末尾的数字
        match = re.search(r'_(\d+)$', device_name)
        if match:
            return match.group(1)
        
        return None
    
    def _generate_device_name(self, device_id: str, slot_id: str) -> str:
        """
        生成设备显示名称
        
        Args:
            device_id: 设备ID
            slot_id: 槽位ID
            
        Returns:
            设备显示名称
        """
        if device_id.startswith('MVCU'):
            return f"主系VCU-槽位{slot_id}"
        elif device_id.startswith('SVCU'):
            return f"安全VCU-槽位{slot_id}"
        else:
            return f"VCU设备-槽位{slot_id}"
    
    def set_device_config(self, config: Dict[str, Any]) -> None:
        """
        设置设备配置
        
        Args:
            config: 设备配置字典
        """
        self._device_config = config
        logger.info("设备配置已更新")
    
    def get_sample_yaml_files(self) -> List[Path]:
        """
        获取示例YAML文件列表
        
        Returns:
            YAML文件路径列表
        """
        yaml_files = []
        
        # 检查config目录中的示例文件
        config_patterns = [
            "devices_example.yaml",
            "devices_sample.yaml", 
            "example_*.yaml",
            "sample_*.yaml"
        ]
        
        for pattern in config_patterns:
            yaml_files.extend(self.config_dir.glob(pattern))
        
        # 检查示例项目目录
        sample_dir = self.get_sample_project_path()
        if sample_dir.exists():
            yaml_files.extend(sample_dir.glob("*.yaml"))
            yaml_files.extend(sample_dir.glob("*.yml"))
        
        return sorted(yaml_files)
    
    def load_sample_yaml_config(self) -> bool:
        """
        自动加载示例YAML配置
        
        Returns:
            是否加载成功
        """
        sample_files = self.get_sample_yaml_files()
        
        if not sample_files:
            logger.warning("未找到示例YAML配置文件")
            # 如果没有找到示例文件，尝试使用内置的设备配置
            logger.info("尝试使用内置设备配置创建示例")
            try:
                device_config = self.build_device_config_from_main()
                if device_config and device_config.get('devices'):
                    self._device_config = device_config
                    logger.info(f"✅ 使用内置配置创建了 {len(device_config['devices'])} 个设备")
                    return True
            except Exception as e:
                logger.error(f"创建内置设备配置失败: {e}")
            return False
        
        # 优先加载devices_example.yaml或devices_sample.yaml
        priority_files = [
            "devices_example.yaml",
            "devices_sample.yaml",
            "example_devices.yaml",
            "sample_devices.yaml"
        ]
        
        target_file = None
        for priority_name in priority_files:
            for file_path in sample_files:
                if file_path.name == priority_name:
                    target_file = file_path
                    break
            if target_file:
                break
        
        # 如果没有找到优先文件，使用第一个找到的文件
        if not target_file:
            target_file = sample_files[0]
        
        try:
            logger.info(f"正在加载示例YAML配置: {target_file}")
            self.load_from_file(str(target_file))
            logger.info(f"✅ 示例YAML配置加载成功: {target_file}")
            return True
        except Exception as e:
            logger.error(f"加载示例YAML配置失败: {e}")
            # 如果YAML文件加载失败，回退到内置配置
            try:
                device_config = self.build_device_config_from_main()
                if device_config and device_config.get('devices'):
                    self._device_config = device_config
                    logger.info(f"⚠️ 回退到内置配置: {len(device_config['devices'])} 个设备")
                    return True
            except Exception as fallback_e:
                logger.error(f"回退到内置配置也失败: {fallback_e}")
            return False


# 全局配置管理器实例
config = ConfigManager() 