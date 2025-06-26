"""
开关桩工具核心模块

提供VCU控制、UDP通信、开关桩管理等核心功能。
"""

from .vcu_controller import VCUController
from .udp_client import UDPClient
from .switch_manager import SwitchManager
from .config_manager import ConfigManager

__version__ = "2.0.0"
__author__ = "VCU Test Team"

__all__ = [
    "VCUController",
    "UDPClient", 
    "SwitchManager",
    "ConfigManager"
] 