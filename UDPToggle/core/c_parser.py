"""
C文件解析器

负责从C源代码文件中解析开关桩定义，包括宏定义、枚举等。
"""

import re
import os
import logging
from typing import Dict, List, Tuple, Optional, Set
from pathlib import Path
from dataclasses import dataclass
from enum import Enum

logger = logging.getLogger(__name__)


@dataclass
class CSwitch:
    """C文件中的开关桩定义"""
    name: str
    switch_type: str  # 'macro', 'enum', 'define'
    value: Optional[str] = None
    description: str = ""
    file_path: str = ""
    line_number: int = 0
    group: str = "default"


class CFileParser:
    """C文件解析器"""
    
    def __init__(self):
        """初始化解析器"""
        self.switches: List[CSwitch] = []
        self.parsed_files: Set[str] = set()
        
        # 正则表达式模式
        self.patterns = {
            # #define SWITCH_NAME value 或 #define SWITCH_NAME
            'define': re.compile(r'^\s*#define\s+([A-Z_][A-Z0-9_]*)\s*(.*?)(?://.*)?$', re.MULTILINE),
            
            # enum { SWITCH_NAME = value, ... }
            'enum': re.compile(r'enum\s*\w*\s*\{([^}]+)\}', re.MULTILINE | re.DOTALL),
            
            # typedef enum { SWITCH_NAME = value } type_name;
            'typedef_enum': re.compile(r'typedef\s+enum\s*\w*\s*\{([^}]+)\}\s*(\w+);', re.MULTILINE | re.DOTALL),
            
            # 单行注释
            'comment': re.compile(r'//\s*(.*)$', re.MULTILINE),
            
            # 多行注释
            'multiline_comment': re.compile(r'/\*([^*]|\*(?!/))*\*/', re.MULTILINE | re.DOTALL),
            
            # 条件编译块
            'ifdef': re.compile(r'#ifdef\s+(\w+)', re.MULTILINE),
            'ifndef': re.compile(r'#ifndef\s+(\w+)', re.MULTILINE),
            'endif': re.compile(r'#endif', re.MULTILINE),
        }
        
        # 常见的开关桩命名模式
        self.switch_patterns = [
            r'.*_ENABLE$',         # XXX_ENABLE
            r'.*_DISABLE$',        # XXX_DISABLE 
            r'.*_ON$',             # XXX_ON
            r'.*_OFF$',            # XXX_OFF
            r'.*_FLAG$',           # XXX_FLAG
            r'.*_SWITCH$',         # XXX_SWITCH
            r'.*_MODE$',           # XXX_MODE
            r'VCU_.*',             # VCU开头
            r'DEBUG_.*',           # DEBUG开头
            r'TEST_.*',            # TEST开头
            r'CONFIG_.*',          # CONFIG开头
            r'FEATURE_.*',         # FEATURE开头
        ]
    
    def parse_project(self, project_path: str) -> List[CSwitch]:
        """
        解析整个项目目录
        
        Args:
            project_path: 项目路径
            
        Returns:
            解析出的开关桩列表
        """
        self.switches.clear()
        self.parsed_files.clear()
        
        project_dir = Path(project_path)
        if project_dir.is_file():
            # 如果是单个文件
            self._parse_file(project_dir)
        elif project_dir.is_dir():
            # 如果是目录，递归查找C文件
            self._parse_directory(project_dir)
        
        logger.info(f"项目解析完成，共找到 {len(self.switches)} 个开关桩，解析了 {len(self.parsed_files)} 个文件")
        return self.switches
    
    def _parse_directory(self, directory: Path) -> None:
        """递归解析目录中的C文件"""
        c_extensions = {'.c', '.h', '.cpp', '.hpp', '.cc', '.cxx'}
        
        for file_path in directory.rglob('*'):
            if file_path.is_file() and file_path.suffix.lower() in c_extensions:
                try:
                    self._parse_file(file_path)
                except Exception as e:
                    logger.warning(f"解析文件失败: {file_path}, 错误: {e}")
    
    def _parse_file(self, file_path: Path) -> None:
        """解析单个C文件"""
        if str(file_path) in self.parsed_files:
            return
        
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            self.parsed_files.add(str(file_path))
            
            # 解析#define宏定义
            self._parse_defines(content, str(file_path))
            
            # 解析枚举
            self._parse_enums(content, str(file_path))
            
            logger.debug(f"已解析文件: {file_path}")
            
        except Exception as e:
            logger.error(f"读取文件失败: {file_path}, 错误: {e}")
    
    def _parse_defines(self, content: str, file_path: str) -> None:
        """解析#define宏定义"""
        lines = content.split('\n')
        
        for line_no, line in enumerate(lines, 1):
            match = self.patterns['define'].match(line.strip())
            if match:
                name = match.group(1)
                value = match.group(2).strip() if match.group(2) else ""
                
                # 检查是否符合开关桩命名模式
                if self._is_switch_name(name):
                    # 查找注释
                    description = self._find_description(lines, line_no - 1)
                    
                    # 确定分组
                    group = self._determine_group(name, file_path)
                    
                    switch = CSwitch(
                        name=name,
                        switch_type='macro',
                        value=value,
                        description=description,
                        file_path=file_path,
                        line_number=line_no,
                        group=group
                    )
                    
                    self.switches.append(switch)
                    logger.debug(f"找到宏开关桩: {name} = {value}")
    
    def _parse_enums(self, content: str, file_path: str) -> None:
        """解析枚举定义"""
        # 解析普通枚举
        for match in self.patterns['enum'].finditer(content):
            enum_content = match.group(1)
            self._parse_enum_content(enum_content, file_path, 'enum')
        
        # 解析typedef枚举
        for match in self.patterns['typedef_enum'].finditer(content):
            enum_content = match.group(1)
            type_name = match.group(2)
            self._parse_enum_content(enum_content, file_path, 'enum', type_name)
    
    def _parse_enum_content(self, enum_content: str, file_path: str, 
                           switch_type: str, type_name: str = "") -> None:
        """解析枚举内容"""
        # 移除注释
        enum_content = re.sub(r'/\*.*?\*/', '', enum_content, flags=re.DOTALL)
        enum_content = re.sub(r'//.*', '', enum_content)
        
        # 分割枚举项
        items = [item.strip() for item in enum_content.split(',') if item.strip()]
        
        for item in items:
            # 解析枚举项 NAME = value 或 NAME
            parts = item.split('=')
            name = parts[0].strip()
            value = parts[1].strip() if len(parts) > 1 else ""
            
            if self._is_switch_name(name):
                group = self._determine_group(name, file_path, type_name)
                
                switch = CSwitch(
                    name=name,
                    switch_type=switch_type,
                    value=value,
                    description=f"枚举项 ({type_name})" if type_name else "枚举项",
                    file_path=file_path,
                    line_number=0,  # 枚举项行号比较难确定
                    group=group
                )
                
                self.switches.append(switch)
                logger.debug(f"找到枚举开关桩: {name} = {value}")
    
    def _is_switch_name(self, name: str) -> bool:
        """判断是否是开关桩名称"""
        # 过滤掉一些明显不是开关桩的定义
        exclude_patterns = [
            r'.*_H$',          # 头文件保护宏
            r'.*_VERSION.*',   # 版本相关
            r'.*_SIZE$',       # 大小定义
            r'.*_MAX$',        # 最大值
            r'.*_MIN$',        # 最小值
            r'.*_COUNT$',      # 计数
            r'.*_MASK$',       # 掩码（除非明确是开关相关）
            r'^\d+$',          # 纯数字
        ]
        
        # 检查排除模式
        for pattern in exclude_patterns:
            if re.match(pattern, name):
                return False
        
        # 检查开关桩模式
        for pattern in self.switch_patterns:
            if re.match(pattern, name):
                return True
        
        # 检查是否包含开关相关关键词
        switch_keywords = ['ENABLE', 'DISABLE', 'ON', 'OFF', 'FLAG', 'SWITCH', 'MODE']
        return any(keyword in name for keyword in switch_keywords)
    
    def _find_description(self, lines: List[str], line_index: int) -> str:
        """查找开关桩的描述注释"""
        descriptions = []
        
        # 查找同行注释
        current_line = lines[line_index] if line_index < len(lines) else ""
        comment_match = re.search(r'//\s*(.+)', current_line)
        if comment_match:
            descriptions.append(comment_match.group(1).strip())
        
        # 查找前面的注释行
        for i in range(line_index - 1, max(line_index - 5, -1), -1):
            if i < 0 or i >= len(lines):
                continue
            
            line = lines[i].strip()
            if not line:
                continue
            
            if line.startswith('//'):
                comment = line[2:].strip()
                if comment:
                    descriptions.insert(0, comment)
            elif line.startswith('/*') and line.endswith('*/'):
                comment = line[2:-2].strip()
                if comment:
                    descriptions.insert(0, comment)
            else:
                break
        
        return ' '.join(descriptions) if descriptions else ""
    
    def _determine_group(self, name: str, file_path: str, type_name: str = "") -> str:
        """确定开关桩所属的分组"""
        # 基于文件名确定分组
        file_name = Path(file_path).stem.lower()
        
        # 基于名称前缀确定分组
        if name.startswith('VCU_'):
            return 'vcu_control'
        elif name.startswith('DEBUG_'):
            return 'debug'
        elif name.startswith('TEST_'):
            return 'test'
        elif name.startswith('CONFIG_'):
            return 'config'
        elif name.startswith('SAFETY_'):
            return 'safety'
        elif name.startswith('FEATURE_'):
            return 'features'
        elif type_name:
            return type_name.lower()
        elif 'config' in file_name:
            return 'config'
        elif 'debug' in file_name:
            return 'debug'
        elif 'test' in file_name:
            return 'test'
        else:
            return file_name or 'default'
    
    def get_switches_by_group(self) -> Dict[str, List[CSwitch]]:
        """按组获取开关桩"""
        groups = {}
        for switch in self.switches:
            if switch.group not in groups:
                groups[switch.group] = []
            groups[switch.group].append(switch)
        return groups
    
    def get_switches_by_file(self) -> Dict[str, List[CSwitch]]:
        """按文件获取开关桩"""
        files = {}
        for switch in self.switches:
            if switch.file_path not in files:
                files[switch.file_path] = []
            files[switch.file_path].append(switch)
        return files
    
    def export_to_config(self) -> Dict:
        """导出为配置格式"""
        groups = self.get_switches_by_group()
        
        config = {
            "description": "从C项目自动解析的开关桩配置",
            "source_files": list(self.parsed_files),
            "switch_groups": {},
            "default_states": {}
        }
        
        for group_name, switches in groups.items():
            config["switch_groups"][group_name] = {
                "name": group_name.replace('_', ' ').title(),
                "switches": []
            }
            
            for switch in switches:
                switch_config = {
                    "name": switch.name,
                    "type": switch.switch_type,
                    "description": switch.description or f"来自 {Path(switch.file_path).name}:{switch.line_number}"
                }
                config["switch_groups"][group_name]["switches"].append(switch_config)
                
                # 设置默认状态
                if switch.value in ['0', 'FALSE', 'false']:
                    config["default_states"][switch.name] = "closed"
                elif switch.value in ['1', 'TRUE', 'true']:
                    config["default_states"][switch.name] = "open"
                else:
                    config["default_states"][switch.name] = "closed"
        
        return config


# 全局解析器实例
c_parser = CFileParser()

def parse_file(file_path: str) -> Tuple[List[CSwitch], Dict]:
    """
    解析单个文件的公共接口
    
    Args:
        file_path: 文件路径
        
    Returns:
        (开关桩列表, 配置数据)
    """
    switches = c_parser.parse_project(file_path)
    config_data = c_parser.export_to_config()
    return switches, config_data 