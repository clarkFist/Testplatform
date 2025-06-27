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
        self._load_default_config()
        self._load_device_config()
    
    def _load_default_config(self) -> None:
        """加载默认配置"""
        # 优先加载YAML配置
        yaml_config_path = self.config_dir / "default.yaml"
        json_config_path = self.config_dir / "default.json"
        
        if yaml_config_path.exists():
            try:
                with open(yaml_config_path, 'r', encoding='utf-8') as f:
                    self._config = yaml.safe_load(f)
                logger.info(f"已加载默认YAML配置: {yaml_config_path}")
                return
            except Exception as e:
                logger.error(f"加载默认YAML配置失败: {e}")
        
        # 如果YAML不存在，尝试加载JSON配置
        if json_config_path.exists():
            try:
                with open(json_config_path, 'r', encoding='utf-8') as f:
                    self._config = json.load(f)
                logger.info(f"已加载默认JSON配置: {json_config_path}")
                return
            except Exception as e:
                logger.error(f"加载默认JSON配置失败: {e}")
        
        logger.warning("默认配置文件不存在，使用内置配置")
        self._config = self._get_fallback_config()
    
    def _load_device_config(self) -> None:
        """加载设备配置"""
        # 优先加载YAML设备配置
        yaml_device_path = self.config_dir / "devices.yaml"
        json_device_path = self.config_dir / "devices.json"
        
        if yaml_device_path.exists():
            try:
                with open(yaml_device_path, 'r', encoding='utf-8') as f:
                    self._device_config = yaml.safe_load(f)
                logger.info(f"已加载设备YAML配置: {yaml_device_path}")
                return
            except Exception as e:
                logger.error(f"加载设备YAML配置失败: {e}")
        
        # 如果YAML不存在，尝试加载JSON配置
        if json_device_path.exists():
            try:
                with open(json_device_path, 'r', encoding='utf-8') as f:
                    self._device_config = json.load(f)
                logger.info(f"已加载设备JSON配置: {json_device_path}")
                return
            except Exception as e:
                logger.error(f"加载设备JSON配置失败: {e}")
        
        logger.warning("设备配置文件不存在，使用空配置")
        self._device_config = {}
    
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
        return not run_marker.exists()
    
    def mark_first_run_completed(self) -> None:
        """标记首次运行已完成"""
        run_marker = self.config_dir / ".first_run_completed"
        try:
            with open(run_marker, 'w', encoding='utf-8') as f:
                f.write(f"首次运行完成时间: {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
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
        return self._device_config.copy()
    
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
            return True
        except Exception as e:
            logger.error(f"加载示例YAML配置失败: {e}")
            return False


# 全局配置管理器实例
config = ConfigManager() 