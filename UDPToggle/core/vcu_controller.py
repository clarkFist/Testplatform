"""
VCU控制器

负责VCU设备的控制和监控功能。
"""

import logging
import time
import threading
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass
from enum import Enum

from .udp_client import UDPClient, DataType, UDPFrame, udp_manager

logger = logging.getLogger(__name__)


class VCUState(Enum):
    """VCU状态枚举"""
    UNKNOWN = "unknown"
    ONLINE = "online"
    OFFLINE = "offline"
    ERROR = "error"
    MAINTENANCE = "maintenance"


class VCUCommand(Enum):
    """VCU命令类型"""
    SWITCH_TAG = 0x01       # 开关桩命令
    SET_CONFIG = 0x02       # 设置配置
    GET_STATUS = 0x03       # 获取状态
    RESET = 0x04           # 重置设备
    MAINTENANCE = 0x05      # 维护模式
    HEARTBEAT = 0x06       # 心跳


@dataclass
class VCUDevice:
    """VCU设备信息"""
    slot_id: str
    name: str
    ip: str
    local_ip: str
    port: int
    enabled: bool = True
    description: str = ""
    state: VCUState = VCUState.UNKNOWN
    last_seen: Optional[float] = None
    stats: Dict[str, Any] = None
    
    def __post_init__(self):
        if self.stats is None:
            self.stats = {
                'commands_sent': 0,
                'responses_received': 0,
                'errors': 0,
                'uptime': 0
            }


class VCUController:
    """VCU控制器"""
    
    def __init__(self):
        """初始化VCU控制器"""
        self._devices: Dict[str, VCUDevice] = {}
        self._clients: Dict[str, UDPClient] = {}
        
        # 回调函数
        self._status_callbacks: List[Callable[[str, VCUState], None]] = []
        self._message_callbacks: List[Callable[[str, Dict], None]] = []
        
        # 心跳和监控
        self._heartbeat_interval = 10.0  # 心跳间隔(秒)
        self._monitor_thread: Optional[threading.Thread] = None
        self._running = False
        
        self._lock = threading.Lock()
    
    def add_device(self, device: VCUDevice) -> bool:
        """
        添加VCU设备
        
        Args:
            device: VCU设备信息
            
        Returns:
            是否添加成功
        """
        try:
            with self._lock:
                self._devices[device.slot_id] = device
            
            # 创建UDP客户端
            client = udp_manager.create_client(
                name=f"vcu_{device.slot_id}",
                local_addr=(device.local_ip, device.port),
                remote_addr=(device.ip, device.port)
            )
            
            # 添加回调
            client.add_message_callback(
                lambda frame, slot=device.slot_id: self._on_message_received(slot, frame)
            )
            client.add_connect_callback(
                lambda connected, slot=device.slot_id: self._on_connection_changed(slot, connected)
            )
            
            self._clients[device.slot_id] = client
            
            logger.info(f"已添加VCU设备: {device.name} (Slot {device.slot_id})")
            return True
            
        except Exception as e:
            logger.error(f"添加VCU设备失败: {e}")
            return False
    
    def remove_device(self, slot_id: str) -> bool:
        """
        移除VCU设备
        
        Args:
            slot_id: 设备槽位ID
            
        Returns:
            是否移除成功
        """
        try:
            with self._lock:
                if slot_id in self._devices:
                    del self._devices[slot_id]
                
                if slot_id in self._clients:
                    self._clients[slot_id].disconnect()
                    del self._clients[slot_id]
            
            udp_manager.remove_client(f"vcu_{slot_id}")
            
            logger.info(f"已移除VCU设备: Slot {slot_id}")
            return True
            
        except Exception as e:
            logger.error(f"移除VCU设备失败: {e}")
            return False
    
    def connect_device(self, slot_id: str) -> bool:
        """
        连接VCU设备
        
        Args:
            slot_id: 设备槽位ID
            
        Returns:
            是否连接成功
        """
        client = self._clients.get(slot_id)
        if not client:
            logger.error(f"VCU设备客户端不存在: Slot {slot_id}")
            return False
        
        if client.connect():
            self._update_device_state(slot_id, VCUState.ONLINE)
            logger.info(f"VCU设备已连接: Slot {slot_id}")
            return True
        else:
            self._update_device_state(slot_id, VCUState.ERROR)
            return False
    
    def disconnect_device(self, slot_id: str) -> None:
        """
        断开VCU设备连接
        
        Args:
            slot_id: 设备槽位ID
        """
        client = self._clients.get(slot_id)
        if client:
            client.disconnect()
            self._update_device_state(slot_id, VCUState.OFFLINE)
            logger.info(f"VCU设备已断开: Slot {slot_id}")
    
    def send_switch_command(self, slot_id: str, switch_name: str, 
                          switch_value: bool) -> bool:
        """
        发送开关桩命令
        
        Args:
            slot_id: 设备槽位ID
            switch_name: 开关名称
            switch_value: 开关值 (True=打开, False=关闭)
            
        Returns:
            是否发送成功
        """
        client = self._clients.get(slot_id)
        if not client or not client.is_connected():
            logger.error(f"VCU设备未连接: Slot {slot_id}")
            return False
        
        try:
            # 构造开关命令数据
            command_data = self._build_switch_command(switch_name, switch_value)
            
            success = client.send_command(DataType.TAG, command_data)
            
            if success:
                # 更新统计
                with self._lock:
                    if slot_id in self._devices:
                        self._devices[slot_id].stats['commands_sent'] += 1
                
                logger.info(f"开关桩命令已发送: Slot {slot_id}, {switch_name}={switch_value}")
            
            return success
            
        except Exception as e:
            logger.error(f"发送开关桩命令失败: {e}")
            return False
    
    def send_config_command(self, slot_id: str, config_data: Dict[str, Any]) -> bool:
        """
        发送配置命令
        
        Args:
            slot_id: 设备槽位ID
            config_data: 配置数据
            
        Returns:
            是否发送成功
        """
        client = self._clients.get(slot_id)
        if not client or not client.is_connected():
            logger.error(f"VCU设备未连接: Slot {slot_id}")
            return False
        
        try:
            command_data = self._build_config_command(config_data)
            success = client.send_command(DataType.CONF, command_data)
            
            if success:
                with self._lock:
                    if slot_id in self._devices:
                        self._devices[slot_id].stats['commands_sent'] += 1
                
                logger.info(f"配置命令已发送: Slot {slot_id}")
            
            return success
            
        except Exception as e:
            logger.error(f"发送配置命令失败: {e}")
            return False
    
    def get_device_status(self, slot_id: str) -> Optional[VCUDevice]:
        """
        获取设备状态
        
        Args:
            slot_id: 设备槽位ID
            
        Returns:
            设备信息或None
        """
        with self._lock:
            return self._devices.get(slot_id)
    
    def get_all_devices(self) -> Dict[str, VCUDevice]:
        """
        获取所有设备信息
        
        Returns:
            设备字典
        """
        with self._lock:
            return self._devices.copy()
    
    def start_monitoring(self) -> None:
        """启动设备监控"""
        if self._running:
            return
        
        self._running = True
        self._monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self._monitor_thread.start()
        
        logger.info("VCU监控已启动")
    
    def stop_monitoring(self) -> None:
        """停止设备监控"""
        self._running = False
        
        if self._monitor_thread and self._monitor_thread.is_alive():
            self._monitor_thread.join(timeout=2.0)
        
        logger.info("VCU监控已停止")
    
    def add_status_callback(self, callback: Callable[[str, VCUState], None]) -> None:
        """
        添加状态变化回调
        
        Args:
            callback: 回调函数
        """
        self._status_callbacks.append(callback)
    
    def add_message_callback(self, callback: Callable[[str, Dict], None]) -> None:
        """
        添加消息接收回调
        
        Args:
            callback: 回调函数
        """
        self._message_callbacks.append(callback)
    
    def _build_switch_command(self, switch_name: str, switch_value: bool) -> bytes:
        """构造开关命令数据"""
        # 简化的命令格式：命令类型(1字节) + 开关名长度(1字节) + 开关名 + 开关值(1字节)
        name_bytes = switch_name.encode('utf-8')
        command_data = bytes([VCUCommand.SWITCH_TAG.value]) + \
                      bytes([len(name_bytes)]) + \
                      name_bytes + \
                      bytes([1 if switch_value else 0])
        return command_data
    
    def _build_config_command(self, config_data: Dict[str, Any]) -> bytes:
        """构造配置命令数据"""
        import json
        config_json = json.dumps(config_data).encode('utf-8')
        command_data = bytes([VCUCommand.SET_CONFIG.value]) + \
                      len(config_json).to_bytes(2, 'big') + \
                      config_json
        return command_data
    
    def _update_device_state(self, slot_id: str, state: VCUState) -> None:
        """更新设备状态"""
        with self._lock:
            if slot_id in self._devices:
                old_state = self._devices[slot_id].state
                self._devices[slot_id].state = state
                self._devices[slot_id].last_seen = time.time()
                
                # 触发状态回调
                if old_state != state:
                    for callback in self._status_callbacks:
                        try:
                            callback(slot_id, state)
                        except Exception as e:
                            logger.error(f"状态回调执行错误: {e}")
    
    def _on_message_received(self, slot_id: str, frame: UDPFrame) -> None:
        """处理接收到的消息"""
        try:
            with self._lock:
                if slot_id in self._devices:
                    self._devices[slot_id].stats['responses_received'] += 1
                    self._devices[slot_id].last_seen = time.time()
            
            # 解析消息内容
            message_data = {
                'slot_id': slot_id,
                'data_type': frame.data_type.name,
                'data': frame.data,
                'timestamp': time.time()
            }
            
            # 触发消息回调
            for callback in self._message_callbacks:
                try:
                    callback(slot_id, message_data)
                except Exception as e:
                    logger.error(f"消息回调执行错误: {e}")
            
            logger.debug(f"收到VCU消息: Slot {slot_id}, Type {frame.data_type.name}")
            
        except Exception as e:
            logger.error(f"处理VCU消息失败: {e}")
    
    def _on_connection_changed(self, slot_id: str, connected: bool) -> None:
        """处理连接状态变化"""
        state = VCUState.ONLINE if connected else VCUState.OFFLINE
        self._update_device_state(slot_id, state)
        
        logger.info(f"VCU连接状态变化: Slot {slot_id}, Connected={connected}")
    
    def _monitor_loop(self) -> None:
        """监控循环"""
        logger.info("VCU监控循环已启动")
        
        while self._running:
            try:
                current_time = time.time()
                
                with self._lock:
                    for slot_id, device in self._devices.items():
                        # 检查设备超时
                        if (device.last_seen and 
                            current_time - device.last_seen > 30.0):  # 30秒超时
                            if device.state == VCUState.ONLINE:
                                device.state = VCUState.ERROR
                                logger.warning(f"VCU设备超时: Slot {slot_id}")
                        
                        # 发送心跳
                        client = self._clients.get(slot_id)
                        if client and client.is_connected():
                            try:
                                heartbeat_data = bytes([VCUCommand.HEARTBEAT.value])
                                client.send_command(DataType.MAINT, heartbeat_data)
                            except Exception as e:
                                logger.debug(f"发送心跳失败: Slot {slot_id}, {e}")
                
                time.sleep(self._heartbeat_interval)
                
            except Exception as e:
                logger.error(f"监控循环异常: {e}")
                time.sleep(1.0)
        
        logger.info("VCU监控循环已停止")
    
    def __enter__(self):
        """上下文管理器入口"""
        self.start_monitoring()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """上下文管理器出口"""
        self.stop_monitoring()
        
        # 断开所有设备连接
        for client in self._clients.values():
            client.disconnect()


# 全局VCU控制器实例
vcu_controller = VCUController() 