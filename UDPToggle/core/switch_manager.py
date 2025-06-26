"""
开关桩管理器

负责管理VCU测试开关桩的状态和操作。
"""

import logging
import time
import threading
from typing import Dict, List, Optional, Any, Callable, Tuple
from dataclasses import dataclass, field
from enum import Enum

logger = logging.getLogger(__name__)


class SwitchType(Enum):
    """开关类型"""
    ENUM = "enum"           # 枚举桩
    MACRO = "macro"         # 宏定义桩
    BOOL = "bool"           # 布尔桩
    INTEGER = "integer"     # 整数桩
    FLOAT = "float"         # 浮点桩


class SwitchState(Enum):
    """开关状态"""
    OPEN = "open"           # 打开
    CLOSED = "closed"       # 关闭
    UNKNOWN = "unknown"     # 未知
    ERROR = "error"         # 错误


@dataclass
class Switch:
    """开关桩信息"""
    name: str
    switch_type: SwitchType
    description: str = ""
    current_state: SwitchState = SwitchState.UNKNOWN
    target_state: Optional[SwitchState] = None
    last_changed: Optional[float] = None
    change_count: int = 0
    enabled: bool = True
    group: str = "default"
    tags: List[str] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def __post_init__(self):
        """初始化后处理"""
        if self.metadata is None:
            self.metadata = {}


@dataclass
class SwitchGroup:
    """开关桩组"""
    name: str
    description: str = ""
    switches: List[str] = field(default_factory=list)
    enabled: bool = True
    
    def __post_init__(self):
        if self.switches is None:
            self.switches = []


class SwitchManager:
    """开关桩管理器"""
    
    def __init__(self):
        """初始化开关桩管理器"""
        self._switches: Dict[str, Switch] = {}
        self._groups: Dict[str, SwitchGroup] = {}
        self._default_states: Dict[str, SwitchState] = {}
        
        # 回调函数
        self._state_change_callbacks: List[Callable[[str, SwitchState, SwitchState], None]] = []
        self._batch_operation_callbacks: List[Callable[[List[str], str, bool], None]] = []
        
        # 统计信息
        self._stats = {
            'total_switches': 0,
            'operations_count': 0,
            'batch_operations_count': 0,
            'errors_count': 0,
            'last_operation_time': None
        }
        
        self._lock = threading.Lock()
    
    def add_switch(self, switch: Switch) -> bool:
        """
        添加开关桩
        
        Args:
            switch: 开关桩信息
            
        Returns:
            是否添加成功
        """
        try:
            with self._lock:
                self._switches[switch.name] = switch
                self._stats['total_switches'] = len(self._switches)
            
            logger.info(f"已添加开关桩: {switch.name} ({switch.switch_type.value})")
            return True
            
        except Exception as e:
            logger.error(f"添加开关桩失败: {e}")
            return False
    
    def remove_switch(self, name: str) -> bool:
        """
        移除开关桩
        
        Args:
            name: 开关桩名称
            
        Returns:
            是否移除成功
        """
        try:
            with self._lock:
                if name in self._switches:
                    del self._switches[name]
                    self._stats['total_switches'] = len(self._switches)
                    
                    # 从组中移除
                    for group in self._groups.values():
                        if name in group.switches:
                            group.switches.remove(name)
                    
                    logger.info(f"已移除开关桩: {name}")
                    return True
                else:
                    logger.warning(f"开关桩不存在: {name}")
                    return False
                    
        except Exception as e:
            logger.error(f"移除开关桩失败: {e}")
            return False
    
    def get_switch(self, name: str) -> Optional[Switch]:
        """
        获取开关桩信息
        
        Args:
            name: 开关桩名称
            
        Returns:
            开关桩信息或None
        """
        with self._lock:
            return self._switches.get(name)
    
    def get_all_switches(self) -> Dict[str, Switch]:
        """
        获取所有开关桩
        
        Returns:
            开关桩字典
        """
        with self._lock:
            return self._switches.copy()
    
    def update_switch_state(self, name: str, new_state: SwitchState) -> bool:
        """
        更新开关桩状态
        
        Args:
            name: 开关桩名称
            new_state: 新状态
            
        Returns:
            是否更新成功
        """
        try:
            with self._lock:
                switch = self._switches.get(name)
                if not switch:
                    logger.error(f"开关桩不存在: {name}")
                    return False
                
                old_state = switch.current_state
                switch.current_state = new_state
                switch.last_changed = time.time()
                switch.change_count += 1
                
                self._stats['operations_count'] += 1
                self._stats['last_operation_time'] = time.time()
            
            # 触发状态变化回调
            for callback in self._state_change_callbacks:
                try:
                    callback(name, old_state, new_state)
                except Exception as e:
                    logger.error(f"状态变化回调执行错误: {e}")
            
            logger.info(f"开关桩状态已更新: {name} {old_state.value} -> {new_state.value}")
            return True
            
        except Exception as e:
            logger.error(f"更新开关桩状态失败: {e}")
            self._stats['errors_count'] += 1
            return False
    
    def set_target_state(self, name: str, target_state: SwitchState) -> bool:
        """
        设置目标状态
        
        Args:
            name: 开关桩名称
            target_state: 目标状态
            
        Returns:
            是否设置成功
        """
        try:
            with self._lock:
                switch = self._switches.get(name)
                if not switch:
                    logger.error(f"开关桩不存在: {name}")
                    return False
                
                switch.target_state = target_state
            
            logger.debug(f"设置目标状态: {name} -> {target_state.value}")
            return True
            
        except Exception as e:
            logger.error(f"设置目标状态失败: {e}")
            return False
    
    def add_group(self, group: SwitchGroup) -> bool:
        """
        添加开关桩组
        
        Args:
            group: 开关桩组
            
        Returns:
            是否添加成功
        """
        try:
            with self._lock:
                self._groups[group.name] = group
            
            logger.info(f"已添加开关桩组: {group.name}")
            return True
            
        except Exception as e:
            logger.error(f"添加开关桩组失败: {e}")
            return False
    
    def get_group(self, name: str) -> Optional[SwitchGroup]:
        """
        获取开关桩组
        
        Args:
            name: 组名称
            
        Returns:
            开关桩组或None
        """
        with self._lock:
            return self._groups.get(name)
    
    def get_switches_by_group(self, group_name: str) -> List[Switch]:
        """
        按组获取开关桩
        
        Args:
            group_name: 组名称
            
        Returns:
            开关桩列表
        """
        with self._lock:
            group = self._groups.get(group_name)
            if not group:
                return []
            
            switches = []
            for switch_name in group.switches:
                switch = self._switches.get(switch_name)
                if switch:
                    switches.append(switch)
            
            return switches
    
    def get_switches_by_type(self, switch_type: SwitchType) -> List[Switch]:
        """
        按类型获取开关桩
        
        Args:
            switch_type: 开关类型
            
        Returns:
            开关桩列表
        """
        with self._lock:
            return [switch for switch in self._switches.values() 
                   if switch.switch_type == switch_type]
    
    def get_switches_by_state(self, state: SwitchState) -> List[Switch]:
        """
        按状态获取开关桩
        
        Args:
            state: 开关状态
            
        Returns:
            开关桩列表
        """
        with self._lock:
            return [switch for switch in self._switches.values() 
                   if switch.current_state == state]
    
    def batch_set_state(self, switch_names: List[str], 
                        target_state: SwitchState) -> Tuple[List[str], List[str]]:
        """
        批量设置开关桩状态
        
        Args:
            switch_names: 开关桩名称列表
            target_state: 目标状态
            
        Returns:
            (成功列表, 失败列表)
        """
        success_list = []
        failed_list = []
        
        try:
            with self._lock:
                for name in switch_names:
                    if name in self._switches:
                        switch = self._switches[name]
                        old_state = switch.current_state
                        switch.target_state = target_state
                        success_list.append(name)
                        
                        logger.debug(f"批量设置: {name} -> {target_state.value}")
                    else:
                        failed_list.append(name)
                        logger.warning(f"批量设置失败，开关桩不存在: {name}")
                
                self._stats['batch_operations_count'] += 1
                self._stats['last_operation_time'] = time.time()
            
            # 触发批量操作回调
            for callback in self._batch_operation_callbacks:
                try:
                    callback(success_list, "set_state", True)
                    if failed_list:
                        callback(failed_list, "set_state", False)
                except Exception as e:
                    logger.error(f"批量操作回调执行错误: {e}")
            
            logger.info(f"批量设置完成: 成功 {len(success_list)}, 失败 {len(failed_list)}")
            
        except Exception as e:
            logger.error(f"批量设置开关桩状态失败: {e}")
            self._stats['errors_count'] += 1
        
        return success_list, failed_list
    
    def batch_toggle_state(self, switch_names: List[str]) -> Tuple[List[str], List[str]]:
        """
        批量切换开关桩状态
        
        Args:
            switch_names: 开关桩名称列表
            
        Returns:
            (成功列表, 失败列表)
        """
        success_list = []
        failed_list = []
        
        try:
            with self._lock:
                for name in switch_names:
                    switch = self._switches.get(name)
                    if switch:
                        # 切换状态
                        if switch.current_state == SwitchState.OPEN:
                            new_state = SwitchState.CLOSED
                        elif switch.current_state == SwitchState.CLOSED:
                            new_state = SwitchState.OPEN
                        else:
                            # 对于未知或错误状态，默认设为关闭
                            new_state = SwitchState.CLOSED
                        
                        switch.target_state = new_state
                        success_list.append(name)
                        
                        logger.debug(f"批量切换: {name} {switch.current_state.value} -> {new_state.value}")
                    else:
                        failed_list.append(name)
                        logger.warning(f"批量切换失败，开关桩不存在: {name}")
                
                self._stats['batch_operations_count'] += 1
                self._stats['last_operation_time'] = time.time()
            
            # 触发批量操作回调
            for callback in self._batch_operation_callbacks:
                try:
                    callback(success_list, "toggle_state", True)
                    if failed_list:
                        callback(failed_list, "toggle_state", False)
                except Exception as e:
                    logger.error(f"批量操作回调执行错误: {e}")
            
            logger.info(f"批量切换完成: 成功 {len(success_list)}, 失败 {len(failed_list)}")
            
        except Exception as e:
            logger.error(f"批量切换开关桩状态失败: {e}")
            self._stats['errors_count'] += 1
        
        return success_list, failed_list
    
    def reset_to_defaults(self) -> bool:
        """
        重置所有开关桩到默认状态
        
        Returns:
            是否重置成功
        """
        try:
            success_count = 0
            with self._lock:
                for name, switch in self._switches.items():
                    default_state = self._default_states.get(name, SwitchState.CLOSED)
                    switch.target_state = default_state
                    success_count += 1
                
                self._stats['batch_operations_count'] += 1
                self._stats['last_operation_time'] = time.time()
            
            logger.info(f"已重置 {success_count} 个开关桩到默认状态")
            return True
            
        except Exception as e:
            logger.error(f"重置到默认状态失败: {e}")
            self._stats['errors_count'] += 1
            return False
    
    def set_default_states(self, defaults: Dict[str, SwitchState]) -> None:
        """
        设置默认状态
        
        Args:
            defaults: 默认状态字典
        """
        with self._lock:
            self._default_states.update(defaults)
        
        logger.info(f"已设置 {len(defaults)} 个默认状态")
    
    def get_statistics(self) -> Dict[str, Any]:
        """
        获取统计信息
        
        Returns:
            统计信息字典
        """
        with self._lock:
            stats = self._stats.copy()
            
            # 添加实时统计
            stats.update({
                'switch_count_by_type': {
                    switch_type.value: len([s for s in self._switches.values() 
                                         if s.switch_type == switch_type])
                    for switch_type in SwitchType
                },
                'switch_count_by_state': {
                    state.value: len([s for s in self._switches.values() 
                                    if s.current_state == state])
                    for state in SwitchState
                },
                'group_count': len(self._groups),
                'enabled_switches': len([s for s in self._switches.values() if s.enabled])
            })
            
        return stats
    
    def add_state_change_callback(self, callback: Callable[[str, SwitchState, SwitchState], None]) -> None:
        """
        添加状态变化回调
        
        Args:
            callback: 回调函数 (switch_name, old_state, new_state)
        """
        self._state_change_callbacks.append(callback)
    
    def add_batch_operation_callback(self, callback: Callable[[List[str], str, bool], None]) -> None:
        """
        添加批量操作回调
        
        Args:
            callback: 回调函数 (switch_names, operation, success)
        """
        self._batch_operation_callbacks.append(callback)
    
    def load_from_config(self, config_data: Dict[str, Any]) -> bool:
        """
        从配置加载开关桩
        
        Args:
            config_data: 配置数据
            
        Returns:
            是否加载成功
        """
        try:
            # 加载开关桩组
            if 'switch_groups' in config_data:
                for group_name, group_config in config_data['switch_groups'].items():
                    group = SwitchGroup(
                        name=group_name,
                        description=group_config.get('description', ''),
                        switches=[s['name'] for s in group_config.get('switches', [])]
                    )
                    self.add_group(group)
                    
                    # 添加组内的开关桩
                    for switch_config in group_config.get('switches', []):
                        switch = Switch(
                            name=switch_config['name'],
                            switch_type=SwitchType(switch_config.get('type', 'enum')),
                            description=switch_config.get('description', ''),
                            group=group_name
                        )
                        self.add_switch(switch)
            
            # 加载默认状态
            if 'default_states' in config_data:
                defaults = {}
                for name, state_str in config_data['default_states'].items():
                    try:
                        defaults[name] = SwitchState(state_str)
                    except ValueError:
                        logger.warning(f"无效的默认状态: {name}={state_str}")
                        defaults[name] = SwitchState.CLOSED
                
                self.set_default_states(defaults)
            
            logger.info("开关桩配置加载完成")
            return True
            
        except Exception as e:
            logger.error(f"加载开关桩配置失败: {e}")
            return False


# 全局开关桩管理器实例
switch_manager = SwitchManager() 