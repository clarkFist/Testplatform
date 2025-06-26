"""
示例插件

展示如何创建和使用插件扩展开关桩工具的功能。
"""

import logging
from typing import Dict, Any, Optional
from core.switch_manager import Switch, SwitchState, SwitchType

logger = logging.getLogger(__name__)


class ExamplePlugin:
    """示例插件类"""
    
    def __init__(self):
        """初始化插件"""
        self.name = "example_plugin"
        self.version = "1.0.0"
        self.description = "开关桩工具示例插件"
        self.enabled = True
        
        logger.info(f"已加载插件: {self.name} v{self.version}")
    
    def initialize(self, vcu_controller, switch_manager) -> bool:
        """
        初始化插件
        
        Args:
            vcu_controller: VCU控制器实例
            switch_manager: 开关桩管理器实例
            
        Returns:
            是否初始化成功
        """
        try:
            self.vcu_controller = vcu_controller
            self.switch_manager = switch_manager
            
            # 注册示例开关桩
            self._register_example_switches()
            
            # 添加状态变化回调
            self.switch_manager.add_state_change_callback(self._on_switch_state_changed)
            
            logger.info(f"插件 {self.name} 初始化完成")
            return True
            
        except Exception as e:
            logger.error(f"插件 {self.name} 初始化失败: {e}")
            return False
    
    def _register_example_switches(self) -> None:
        """注册示例开关桩"""
        example_switches = [
            {
                'name': 'EXAMPLE_SWITCH_001',
                'type': SwitchType.ENUM,
                'description': '示例枚举桩001',
                'group': 'example'
            },
            {
                'name': 'EXAMPLE_SWITCH_002',
                'type': SwitchType.MACRO,
                'description': '示例宏定义桩002',
                'group': 'example'
            },
            {
                'name': 'DEBUG_MODE',
                'type': SwitchType.ENUM,
                'description': '调试模式开关',
                'group': 'debug'
            }
        ]
        
        for switch_info in example_switches:
            self.switch_manager.register_switch(
                name=switch_info['name'],
                switch_type=switch_info['type'],
                description=switch_info['description'],
                group=switch_info['group']
            )
        
        logger.info(f"已注册 {len(example_switches)} 个示例开关桩")
    
    def _on_switch_state_changed(self, switch: Switch) -> None:
        """
        开关桩状态变化回调
        
        Args:
            switch: 开关桩对象
        """
        if switch.group == 'example':
            logger.info(f"示例插件监听到状态变化: {switch.name} -> {switch.state.value}")
            
            # 执行自定义逻辑
            if switch.name == 'DEBUG_MODE' and switch.state == SwitchState.OPEN:
                self._enable_debug_mode()
            elif switch.name == 'DEBUG_MODE' and switch.state == SwitchState.CLOSED:
                self._disable_debug_mode()
    
    def _enable_debug_mode(self) -> None:
        """启用调试模式"""
        logger.info("插件已启用调试模式")
        # 在这里添加调试模式的特定逻辑
    
    def _disable_debug_mode(self) -> None:
        """禁用调试模式"""
        logger.info("插件已禁用调试模式")
        # 在这里添加禁用调试模式的逻辑
    
    def get_plugin_info(self) -> Dict[str, Any]:
        """
        获取插件信息
        
        Returns:
            插件信息字典
        """
        return {
            'name': self.name,
            'version': self.version,
            'description': self.description,
            'enabled': self.enabled,
            'switch_count': len(self.switch_manager.get_switches_by_group('example'))
        }
    
    def custom_command(self, command: str, params: Dict[str, Any]) -> Dict[str, Any]:
        """
        执行自定义命令
        
        Args:
            command: 命令名称
            params: 命令参数
            
        Returns:
            执行结果
        """
        if command == 'test_switches':
            return self._test_switches()
        elif command == 'reset_switches':
            return self._reset_switches()
        else:
            return {'error': f'未知命令: {command}'}
    
    def _test_switches(self) -> Dict[str, Any]:
        """测试开关桩功能"""
        logger.info("开始测试示例开关桩...")
        
        example_switches = self.switch_manager.get_switches_by_group('example')
        results = []
        
        for switch in example_switches:
            # 测试开启
            success1 = self.switch_manager.set_switch_state(switch.name, SwitchState.OPEN)
            
            # 测试关闭
            success2 = self.switch_manager.set_switch_state(switch.name, SwitchState.CLOSED)
            
            results.append({
                'switch': switch.name,
                'open_test': success1,
                'close_test': success2
            })
        
        logger.info("示例开关桩测试完成")
        return {
            'message': '测试完成',
            'results': results
        }
    
    def _reset_switches(self) -> Dict[str, Any]:
        """重置开关桩状态"""
        logger.info("重置示例开关桩状态...")
        
        example_switches = self.switch_manager.get_switches_by_group('example')
        count = 0
        
        for switch in example_switches:
            if self.switch_manager.set_switch_state(switch.name, SwitchState.CLOSED):
                count += 1
        
        logger.info(f"已重置 {count} 个开关桩")
        return {
            'message': f'已重置 {count} 个开关桩',
            'count': count
        }
    
    def cleanup(self) -> None:
        """清理插件资源"""
        logger.info(f"正在清理插件 {self.name}...")
        
        # 注销示例开关桩
        example_switches = self.switch_manager.get_switches_by_group('example')
        for switch in example_switches:
            self.switch_manager.unregister_switch(switch.name)
        
        logger.info(f"插件 {self.name} 清理完成")


# 插件工厂函数
def create_plugin():
    """
    创建插件实例
    
    Returns:
        插件实例
    """
    return ExamplePlugin() 