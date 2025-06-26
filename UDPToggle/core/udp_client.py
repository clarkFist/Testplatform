"""
UDP通信客户端

负责与VCU设备的UDP通信，包括消息发送、接收和解析。
"""

import socket
import struct
import time
import threading
import asyncio
import logging
from typing import Dict, List, Optional, Callable, Tuple, Any
from dataclasses import dataclass
from enum import Enum

logger = logging.getLogger(__name__)


class DataType(Enum):
    """数据类型枚举"""
    TAG = 0x00
    CONF = 0x02
    LOG = 0x0E
    MAINT = 0x04


class UDPFrameType(Enum):
    """UDP帧类型"""
    REQUEST = 0x01
    RESPONSE = 0x02
    ACK = 0x03


@dataclass
class UDPFrame:
    """UDP数据帧结构"""
    dst_node_id: int = 0xFF
    src_node_id: int = 0xFF
    data_type: DataType = DataType.LOG
    data_len: int = 0
    data: bytes = b''
    
    def pack(self) -> bytes:
        """打包数据帧"""
        header = struct.pack('BBBBBxxx', 
                           self.dst_node_id,
                           self.src_node_id, 
                           self.data_type.value,
                           self.data_len,
                           len(self.data))
        return header + self.data
    
    @classmethod
    def unpack(cls, data: bytes) -> 'UDPFrame':
        """解包数据帧"""
        if len(data) < 8:
            raise ValueError("数据长度不足")
        
        header = struct.unpack('BBBBBxxx', data[:8])
        frame = cls()
        frame.dst_node_id = header[0]
        frame.src_node_id = header[1]
        
        try:
            frame.data_type = DataType(header[2])
        except ValueError:
            logger.warning(f"未知数据类型: {header[2]}")
            frame.data_type = DataType.LOG
        
        frame.data_len = header[3]
        frame.data = data[8:8+frame.data_len] if frame.data_len > 0 else b''
        
        return frame


class UDPClient:
    """UDP通信客户端"""
    
    def __init__(self, local_addr: Tuple[str, int], remote_addr: Tuple[str, int]):
        """
        初始化UDP客户端
        
        Args:
            local_addr: 本地地址 (IP, 端口)
            remote_addr: 远程地址 (IP, 端口)
        """
        self.local_addr = local_addr
        self.remote_addr = remote_addr
        
        self._socket: Optional[socket.socket] = None
        self._running = False
        self._recv_thread: Optional[threading.Thread] = None
        
        # 回调函数
        self._message_callbacks: List[Callable[[UDPFrame], None]] = []
        self._connect_callbacks: List[Callable[[bool], None]] = []
        
        # 统计信息
        self._stats = {
            'sent_count': 0,
            'recv_count': 0,
            'error_count': 0,
            'last_activity': None
        }
        
        self._lock = threading.Lock()
    
    def add_message_callback(self, callback: Callable[[UDPFrame], None]) -> None:
        """
        添加消息接收回调
        
        Args:
            callback: 回调函数
        """
        self._message_callbacks.append(callback)
    
    def add_connect_callback(self, callback: Callable[[bool], None]) -> None:
        """
        添加连接状态回调
        
        Args:
            callback: 回调函数，参数为连接状态
        """
        self._connect_callbacks.append(callback)
    
    def connect(self) -> bool:
        """
        连接到远程设备
        
        Returns:
            是否连接成功
        """
        try:
            # 创建UDP socket
            self._socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self._socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self._socket.settimeout(1.0)  # 设置超时
            
            # 绑定本地地址
            self._socket.bind(self.local_addr)
            
            # 启动接收线程
            self._running = True
            self._recv_thread = threading.Thread(target=self._recv_loop, daemon=True)
            self._recv_thread.start()
            
            logger.info(f"UDP客户端已连接: {self.local_addr} -> {self.remote_addr}")
            
            # 通知连接状态
            for callback in self._connect_callbacks:
                try:
                    callback(True)
                except Exception as e:
                    logger.error(f"连接回调执行错误: {e}")
            
            return True
            
        except Exception as e:
            logger.error(f"UDP连接失败: {e}")
            self._stats['error_count'] += 1
            return False
    
    def disconnect(self) -> None:
        """断开连接"""
        self._running = False
        
        if self._recv_thread and self._recv_thread.is_alive():
            self._recv_thread.join(timeout=2.0)
        
        if self._socket:
            try:
                self._socket.close()
            except:
                pass
            self._socket = None
        
        # 通知连接状态
        for callback in self._connect_callbacks:
            try:
                callback(False)
            except Exception as e:
                logger.error(f"断开回调执行错误: {e}")
        
        logger.info("UDP客户端已断开连接")
    
    def send_frame(self, frame: UDPFrame) -> bool:
        """
        发送数据帧
        
        Args:
            frame: 数据帧
            
        Returns:
            是否发送成功
        """
        if not self._socket or not self._running:
            logger.error("UDP客户端未连接")
            return False
        
        try:
            data = frame.pack()
            sent_bytes = self._socket.sendto(data, self.remote_addr)
            
            with self._lock:
                self._stats['sent_count'] += 1
                self._stats['last_activity'] = time.time()
            
            logger.debug(f"已发送 {sent_bytes} 字节到 {self.remote_addr}")
            return True
            
        except Exception as e:
            logger.error(f"发送数据失败: {e}")
            self._stats['error_count'] += 1
            return False
    
    def send_command(self, data_type: DataType, data: bytes, 
                    dst_node_id: int = 0xFF) -> bool:
        """
        发送命令
        
        Args:
            data_type: 数据类型
            data: 数据内容
            dst_node_id: 目标节点ID
            
        Returns:
            是否发送成功
        """
        frame = UDPFrame(
            dst_node_id=dst_node_id,
            src_node_id=0xFF,  # 本地节点ID
            data_type=data_type,
            data_len=len(data),
            data=data
        )
        
        return self.send_frame(frame)
    
    def _recv_loop(self) -> None:
        """接收循环"""
        logger.info("UDP接收线程已启动")
        
        while self._running:
            try:
                if not self._socket:
                    break
                
                data, addr = self._socket.recvfrom(4096)
                
                if addr[0] != self.remote_addr[0]:
                    logger.debug(f"忽略来自未知地址的数据: {addr}")
                    continue
                
                # 解析数据帧
                try:
                    frame = UDPFrame.unpack(data)
                    
                    with self._lock:
                        self._stats['recv_count'] += 1
                        self._stats['last_activity'] = time.time()
                    
                    logger.debug(f"收到数据帧: type={frame.data_type}, len={frame.data_len}")
                    
                    # 调用回调函数
                    for callback in self._message_callbacks:
                        try:
                            callback(frame)
                        except Exception as e:
                            logger.error(f"消息回调执行错误: {e}")
                
                except Exception as e:
                    logger.error(f"解析数据帧失败: {e}")
                    self._stats['error_count'] += 1
            
            except socket.timeout:
                continue
            except OSError as e:
                if self._running:  # 只有在运行时才记录错误
                    logger.error(f"接收数据错误: {e}")
                break
            except Exception as e:
                logger.error(f"接收循环异常: {e}")
                break
        
        logger.info("UDP接收线程已停止")
    
    def is_connected(self) -> bool:
        """
        检查连接状态
        
        Returns:
            是否已连接
        """
        return self._running and self._socket is not None
    
    def get_stats(self) -> Dict[str, Any]:
        """
        获取统计信息
        
        Returns:
            统计信息字典
        """
        with self._lock:
            return self._stats.copy()
    
    def reset_stats(self) -> None:
        """重置统计信息"""
        with self._lock:
            self._stats = {
                'sent_count': 0,
                'recv_count': 0,
                'error_count': 0,
                'last_activity': None
            }
    
    def __enter__(self):
        """上下文管理器入口"""
        self.connect()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """上下文管理器出口"""
        self.disconnect()


class UDPClientManager:
    """UDP客户端管理器"""
    
    def __init__(self):
        self._clients: Dict[str, UDPClient] = {}
        self._lock = threading.Lock()
    
    def create_client(self, name: str, local_addr: Tuple[str, int], 
                     remote_addr: Tuple[str, int]) -> UDPClient:
        """
        创建UDP客户端
        
        Args:
            name: 客户端名称
            local_addr: 本地地址
            remote_addr: 远程地址
            
        Returns:
            UDP客户端实例
        """
        with self._lock:
            if name in self._clients:
                logger.warning(f"客户端 {name} 已存在，将替换")
                self._clients[name].disconnect()
            
            client = UDPClient(local_addr, remote_addr)
            self._clients[name] = client
            
            logger.info(f"已创建UDP客户端: {name}")
            return client
    
    def get_client(self, name: str) -> Optional[UDPClient]:
        """
        获取UDP客户端
        
        Args:
            name: 客户端名称
            
        Returns:
            UDP客户端实例或None
        """
        with self._lock:
            return self._clients.get(name)
    
    def remove_client(self, name: str) -> bool:
        """
        移除UDP客户端
        
        Args:
            name: 客户端名称
            
        Returns:
            是否移除成功
        """
        with self._lock:
            if name in self._clients:
                self._clients[name].disconnect()
                del self._clients[name]
                logger.info(f"已移除UDP客户端: {name}")
                return True
            return False
    
    def get_all_clients(self) -> Dict[str, UDPClient]:
        """
        获取所有客户端
        
        Returns:
            客户端字典
        """
        with self._lock:
            return self._clients.copy()
    
    def disconnect_all(self) -> None:
        """断开所有客户端连接"""
        with self._lock:
            for client in self._clients.values():
                client.disconnect()
            logger.info("已断开所有UDP客户端连接")


# 全局客户端管理器
udp_manager = UDPClientManager() 