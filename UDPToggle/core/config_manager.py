"""
配置管理器

负责加载、验证和管理系统配置。
"""

import json
import os
from pathlib import Path
from typing import Dict, Any, Optional
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
        self._load_default_config()
    
    def _load_default_config(self) -> None:
        """加载默认配置"""
        default_config_path = self.config_dir / "default.json"
        
        if default_config_path.exists():
            try:
                with open(default_config_path, 'r', encoding='utf-8') as f:
                    self._config = json.load(f)
                logger.info(f"已加载默认配置: {default_config_path}")
            except Exception as e:
                logger.error(f"加载默认配置失败: {e}")
                self._config = self._get_fallback_config()
        else:
            logger.warning("默认配置文件不存在，使用内置配置")
            self._config = self._get_fallback_config()
    
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
            with open(filepath, 'r', encoding='utf-8') as f:
                config = json.load(f)
            
            # 合并配置
            self._merge_config(config)
            logger.info(f"已加载配置文件: {filepath}")
            
        except Exception as e:
            logger.error(f"加载配置文件失败: {filepath}, 错误: {e}")
            raise
    
    def save_to_file(self, filepath: str) -> None:
        """
        保存配置到文件
        
        Args:
            filepath: 配置文件路径
        """
        try:
            # 确保目录存在
            Path(filepath).parent.mkdir(parents=True, exist_ok=True)
            
            with open(filepath, 'w', encoding='utf-8') as f:
                json.dump(self._config, f, indent=2, ensure_ascii=False)
            
            logger.info(f"配置已保存到: {filepath}")
            
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


# 全局配置管理器实例
config = ConfigManager() 