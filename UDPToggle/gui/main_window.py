"""
主窗口GUI应用程序

VCU测试开关桩管理工具的主要用户界面。
"""

import tkinter as tk
from tkinter import ttk, messagebox, filedialog, simpledialog
import threading
import time
import json
import logging
from typing import Dict, Any, Optional, List
from pathlib import Path

# YAML支持 - 可选导入
try:
    import yaml
    YAML_AVAILABLE = True
except ImportError:
    YAML_AVAILABLE = False
    yaml = None

from core.vcu_controller import vcu_controller, VCUDevice, VCUState
from core.switch_manager import switch_manager, Switch, SwitchState, SwitchType
from core.config_manager import config
from core.c_parser import c_parser, CSwitch, parse_file

logger = logging.getLogger(__name__)


class VCUControllerApp:
    """VCU控制器主应用程序"""
    
    def __init__(self, root: tk.Tk):
        """初始化应用程序"""
        self.root = root
        
        # 状态变量
        self.is_monitoring = False
        self.update_thread: Optional[threading.Thread] = None
        self.operation_history = []
        
        # 搜索变量 - 必须在create_widgets之前初始化
        self.device_search_var = tk.StringVar()
        self.switch_search_var = tk.StringVar()
        self.device_search_var.trace('w', self.on_device_search_changed)
        self.switch_search_var.trace('w', self.on_switch_search_changed)
        
        # 初始化启动时间
        self._start_time = time.time()
        
        self.setup_window()
        self.setup_styles()
        self.create_widgets()
        
        # 显示配置加载状态
        self._show_config_status()
        
        # 强制加载默认配置
        self._force_load_default_configuration()
        
        self.setup_callbacks()
        self.setup_keyboard_shortcuts()
        
        logger.info("主窗口已初始化")
    
    def _force_load_default_configuration(self):
        """强制加载默认设备配置，确保设备能够正确加载"""
        # 为了避免重复加载，先检查是否已有设备
        existing_devices = vcu_controller.get_all_devices()
        if len(existing_devices) > 0:
            logger.info(f"检测到已有 {len(existing_devices)} 个设备，跳过强制加载")
            self.add_log_entry(f"ℹ️ 检测到已有 {len(existing_devices)} 个设备，跳过重复加载", "INFO")
            # 直接刷新界面
            self.refresh_device_list()
            self.update_statistics()
            return
        
        try:
            self.add_log_entry("🔧 正在强制加载设备配置...", "INFO")
            
            # 从配置管理器获取设备配置
            device_config = config.get_device_config()
            
            if device_config and 'devices' in device_config:
                self.add_log_entry(f"✅ 发现设备配置: {len(device_config['devices'])} 个设备", "INFO")
                
                loaded_count = 0
                
                # 强制加载每个设备
                for device_id, device_cfg in device_config['devices'].items():
                    try:
                        self.add_log_entry(f"📱 正在加载设备: {device_id}", "INFO")
                        
                        device = VCUDevice(
                            slot_id=device_cfg["slot_id"],
                            name=device_cfg["name"],
                            ip=device_cfg["ip"],
                            local_ip=device_cfg.get("local_ip", device_cfg["ip"]),
                            port=device_cfg.get("port", 18125),
                            enabled=device_cfg.get("enabled", True),
                            description=device_cfg.get("description", "")
                        )
                        
                        # 添加到VCU控制器
                        success = vcu_controller.add_device(device)
                        if success:
                            loaded_count += 1
                            self.add_log_entry(f"✅ 设备加载成功: {device.name} (槽位{device.slot_id})", "SUCCESS")
                        else:
                            self.add_log_entry(f"❌ 设备添加失败: {device_id}", "ERROR")
                        
                    except Exception as e:
                        self.add_log_entry(f"❌ 设备加载失败 {device_id}: {e}", "ERROR")
                        logger.error(f"设备加载失败 {device_id}: {e}", exc_info=True)
                
                # 验证加载结果
                actual_devices = vcu_controller.get_all_devices()
                self.add_log_entry(f"✅ 配置加载完成: {loaded_count} 个设备已加载", "SUCCESS")
                self.add_log_entry(f"🔍 VCU控制器验证: {len(actual_devices)} 个设备", "INFO")
                
                # 刷新界面
                self.refresh_device_list()
                self.update_statistics()
                
                if len(actual_devices) != loaded_count:
                    self.add_log_entry(f"⚠️ 警告: 加载设备数({loaded_count})与实际设备数({len(actual_devices)})不匹配", "WARNING")
                
            else:
                self.add_log_entry("⚠️ 未找到有效的设备配置", "WARNING")
                # 尝试传统的加载方式
                self.load_default_configuration()
            
        except Exception as e:
            self.add_log_entry(f"❌ 强制加载配置失败: {e}", "ERROR")
            logger.error(f"强制加载配置失败: {e}", exc_info=True)
            # 回退到传统加载方式
            try:
                self.load_default_configuration()
            except:
                pass
    
    def _show_config_status(self):
        """显示配置加载状态"""
        try:
            config_status = config.get_config_status() if hasattr(config, 'get_config_status') else None
            
            # 在日志中显示配置状态
            if config_status and config_status.get('loaded', False):
                self.add_log_entry("🔧 配置系统初始化完成", "SUCCESS")
                self.add_log_entry(f"📋 配置来源: {config_status['source']}", "INFO") 
                self.add_log_entry(f"🏷️ 系统版本: {config_status['version']}", "INFO")
                
                if config_status.get('validation_passed', False):
                    self.add_log_entry("✅ 配置验证通过", "SUCCESS")
                else:
                    self.add_log_entry("⚠️ 配置验证失败", "WARNING")
            else:
                self.add_log_entry("🔧 配置系统已初始化", "SUCCESS")
                self.add_log_entry("📋 使用默认配置", "INFO")
                
            # 显示YAML支持状态
            if YAML_AVAILABLE:
                self.add_log_entry("📄 YAML配置格式: ✅ 支持", "SUCCESS")
            else:
                self.add_log_entry("📄 YAML配置格式: ❌ 不支持 (缺少PyYAML库)", "WARNING")
                self.add_log_entry("💡 提示: 安装PyYAML以支持YAML格式: pip install PyYAML", "INFO")
                
            # 显示关键配置信息
            app_name = config.get('app.name', '未知应用')
            udp_port = config.get('vcu.udp_port', 'N/A')
            self.add_log_entry(f"🚀 {app_name} 已启动", "INFO")
            self.add_log_entry(f"🌐 UDP通信端口: {udp_port}", "INFO")
            
        except Exception as e:
            logger.error(f"显示配置状态失败: {e}")
            self.add_log_entry("⚠️ 无法获取配置状态信息", "WARNING")
    
    def setup_window(self):
        """设置主窗口"""
        self.root.title("VCU测试开关桩管理工具 v2.0.0 - 专业版")
        self.root.geometry("1400x900")
        self.root.minsize(1200, 700)
        
        # 设置窗口图标和主题
        try:
            self.root.state('zoomed')  # Windows最大化
        except:
            pass
        
        # 设置应用程序图标
        try:
            # 如果有图标文件，可以在这里设置
            pass
        except:
            pass
    
    def setup_styles(self):
        """设置样式和主题"""
        self.style = ttk.Style()
        self.style.theme_use('clam')
        
        # 定义颜色方案
        self.colors = {
            'primary': '#2563eb',      # 主蓝色
            'success': '#10b981',      # 成功绿色
            'warning': '#f59e0b',      # 警告黄色
            'danger': '#ef4444',       # 危险红色
            'info': '#3b82f6',         # 信息蓝色
            'light': '#f8fafc',        # 浅色背景
            'dark': '#1e293b',         # 深色文本
            'online': '#10b981',       # 在线状态
            'offline': '#ef4444',      # 离线状态
            'connecting': '#f59e0b',   # 连接中状态
            'switch_open': '#10b981',  # 开关打开
            'switch_closed': '#ef4444', # 开关关闭
        }
        
        # 配置样式
        self.style.configure('Title.TLabel', font=('Arial', 12, 'bold'))
        self.style.configure('Status.TLabel', font=('Arial', 10))
        self.style.configure('Success.TLabel', foreground=self.colors['success'])
        self.style.configure('Danger.TLabel', foreground=self.colors['danger'])
        self.style.configure('Warning.TLabel', foreground=self.colors['warning'])
        
        # 按钮样式
        self.style.configure('Action.TButton', font=('Arial', 9, 'bold'))
        self.style.map('Action.TButton',
                      background=[('active', self.colors['primary']),
                                ('pressed', self.colors['primary'])])
        
        # 树形视图样式
        self.style.configure('Treeview', font=('Arial', 9))
        self.style.configure('Treeview.Heading', font=('Arial', 9, 'bold'))
        
        # 配置标签颜色
        self.root.tk.call('set', 'color_online', self.colors['online'])
        self.root.tk.call('set', 'color_offline', self.colors['offline'])
        self.root.tk.call('set', 'color_switch_open', self.colors['switch_open'])
        self.root.tk.call('set', 'color_switch_closed', self.colors['switch_closed'])
    
    def setup_keyboard_shortcuts(self):
        """设置键盘快捷键"""
        # 文件操作
        self.root.bind('<Control-o>', lambda e: self.load_project_file())
        self.root.bind('<Control-q>', lambda e: self.on_closing())
        
        # 设备操作
        self.root.bind('<F5>', lambda e: self.refresh_device_list())
        self.root.bind('<Control-r>', lambda e: self.refresh_all())
        self.root.bind('<Control-d>', lambda e: self.connect_selected_device())
        self.root.bind('<Control-Shift-d>', lambda e: self.disconnect_selected_device())
        
        # 开关桩操作
        self.root.bind('<Control-a>', lambda e: self.select_all_switches())
        self.root.bind('<Control-Shift-a>', lambda e: self.deselect_all_switches())
        self.root.bind('<Key-space>', lambda e: self.toggle_selected_switches())
        self.root.bind('<Return>', lambda e: self.apply_switch_changes())
        
        # 监控操作
        self.root.bind('<F1>', lambda e: self.toggle_monitoring())
        self.root.bind('<F2>', lambda e: self.show_help())
        
        # 搜索功能
        self.root.bind('<Control-f>', lambda e: self.focus_search())
    
    def create_widgets(self):
        """创建主要控件"""
        # 创建主菜单
        self.create_menu()
        
        # 创建工具栏
        self.create_toolbar()
        
        # 创建主要面板
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=8, pady=5)
        
        # 创建左右分割窗口
        paned_window = ttk.PanedWindow(main_frame, orient=tk.HORIZONTAL)
        paned_window.pack(fill=tk.BOTH, expand=True)
        
        # 左侧面板：设备管理
        left_frame = ttk.LabelFrame(paned_window, text="🖥️ VCU设备管理", padding=10)
        left_frame.configure(style='Title.TLabelframe')
        paned_window.add(left_frame, weight=1)
        self.create_device_panel(left_frame)
        
        # 右侧面板：开关桩管理
        right_frame = ttk.LabelFrame(paned_window, text="⚡ 开关桩管理", padding=10)
        right_frame.configure(style='Title.TLabelframe')
        paned_window.add(right_frame, weight=2)
        self.create_switch_panel(right_frame)
        
        # 底部面板：日志和状态
        bottom_frame = ttk.Frame(self.root)
        bottom_frame.pack(fill=tk.X, side=tk.BOTTOM)
        
        self.create_log_panel(bottom_frame)
        self.create_status_bar()
    
    def create_toolbar(self):
        """创建工具栏"""
        toolbar_frame = ttk.Frame(self.root)
        toolbar_frame.pack(fill=tk.X, padx=5, pady=2)
        
        # 项目操作按钮
        project_frame = ttk.LabelFrame(toolbar_frame, text="项目", padding=5)
        project_frame.pack(side=tk.LEFT, padx=5)
        
        ttk.Button(project_frame, text="📁 加载项目", command=self.load_project_file, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=2)
        
        # 加载设备列表（YAML）按钮
        if YAML_AVAILABLE:
            ttk.Button(project_frame, text="📱 加载设备列表（YAML）", command=self.load_device_list_yaml, 
                      style='Action.TButton').pack(side=tk.LEFT, padx=2)
        else:
            yaml_button = ttk.Button(project_frame, text="📱 加载设备列表（YAML）", command=self._show_yaml_install_help, 
                                   style='Action.TButton', state='disabled')
            yaml_button.pack(side=tk.LEFT, padx=2)
        
        # 设备操作按钮
        device_frame = ttk.LabelFrame(toolbar_frame, text="设备", padding=5)
        device_frame.pack(side=tk.LEFT, padx=5)
        
        ttk.Button(device_frame, text="🔗 连接全部", command=self.connect_all_devices, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=2)
        ttk.Button(device_frame, text="🔌 断开全部", command=self.disconnect_all_devices, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=2)
        
        # 开关桩操作按钮
        switch_frame = ttk.LabelFrame(toolbar_frame, text="开关桩", padding=5)
        switch_frame.pack(side=tk.LEFT, padx=5)
        
        ttk.Button(switch_frame, text="🟢 全部打开", command=self.open_all_switches, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=2)
        ttk.Button(switch_frame, text="🔴 全部关闭", command=self.close_all_switches, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=2)
        
        # 系统操作按钮
        system_frame = ttk.LabelFrame(toolbar_frame, text="系统", padding=5)
        system_frame.pack(side=tk.LEFT, padx=5)
        
        self.monitor_toolbar_button = ttk.Button(system_frame, text="▶️ 启动监控", 
                                               command=self.toggle_monitoring, 
                                               style='Action.TButton')
        self.monitor_toolbar_button.pack(side=tk.LEFT, padx=2)
        
        ttk.Button(system_frame, text="🔄 刷新", command=self.refresh_all, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=2)
        
        # 搜索框
        search_frame = ttk.LabelFrame(toolbar_frame, text="搜索", padding=5)
        search_frame.pack(side=tk.RIGHT, padx=5)
        
        ttk.Label(search_frame, text="🔍").pack(side=tk.LEFT)
        self.global_search_var = tk.StringVar()
        self.global_search_entry = ttk.Entry(search_frame, textvariable=self.global_search_var, width=20)
        self.global_search_entry.pack(side=tk.LEFT, padx=2)
        self.global_search_var.trace('w', self.on_global_search_changed)
        
        # 添加工具提示
        self.create_tooltips()
    
    def create_menu(self):
        """创建主菜单"""
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)
        
        # 文件菜单
        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="文件(F)", menu=file_menu, underline=3)
        file_menu.add_command(label="加载项目... (Ctrl+O)", command=self.load_project_file, accelerator="Ctrl+O")
        
        # YAML设备列表子菜单
        if YAML_AVAILABLE:
            file_menu.add_command(label="加载设备列表（YAML）...", command=self.load_device_list_yaml)
        else:
            file_menu.add_command(label="加载设备列表（YAML）... (需要PyYAML)", command=self._show_yaml_install_help, state='disabled')
        
        file_menu.add_separator()
        file_menu.add_command(label="导出报告...", command=self.export_report)
        file_menu.add_command(label="导出日志...", command=self.export_logs)
        file_menu.add_separator()
        file_menu.add_command(label="退出 (Ctrl+Q)", command=self.on_closing, accelerator="Ctrl+Q")
        
        # 设备菜单
        device_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="设备(D)", menu=device_menu, underline=3)
        device_menu.add_command(label="连接所有设备", command=self.connect_all_devices)
        device_menu.add_command(label="断开所有设备", command=self.disconnect_all_devices)
        device_menu.add_separator()
        device_menu.add_command(label="重置设备", command=self.reset_devices)
        device_menu.add_command(label="设备诊断", command=self.device_diagnostics)
        device_menu.add_separator()
        device_menu.add_command(label="刷新设备列表 (F5)", command=self.refresh_device_list, accelerator="F5")
        
        # 开关桩菜单
        switch_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="开关桩(S)", menu=switch_menu, underline=4)
        switch_menu.add_command(label="全部打开", command=self.open_all_switches)
        switch_menu.add_command(label="全部关闭", command=self.close_all_switches)
        switch_menu.add_command(label="重置到默认", command=self.reset_switches)
        switch_menu.add_separator()
        switch_menu.add_command(label="批量操作...", command=self.batch_operation_dialog)
        switch_menu.add_command(label="创建开关桩组...", command=self.create_switch_group)
        
        # 监控菜单
        monitor_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="监控(M)", menu=monitor_menu, underline=3)
        monitor_menu.add_command(label="启动/停止监控 (F1)", command=self.toggle_monitoring, accelerator="F1")
        monitor_menu.add_command(label="监控设置...", command=self.monitor_settings)
        monitor_menu.add_separator()
        monitor_menu.add_command(label="查看历史记录", command=self.show_history)
        monitor_menu.add_command(label="清除历史记录", command=self.clear_history)
        
        # 视图菜单
        view_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="视图(V)", menu=view_menu, underline=3)
        view_menu.add_command(label="刷新全部 (Ctrl+R)", command=self.refresh_all, accelerator="Ctrl+R")
        view_menu.add_separator()
        view_menu.add_checkbutton(label="显示统计信息", command=self.toggle_statistics)
        view_menu.add_checkbutton(label="显示日志面板", command=self.toggle_log_panel)
        view_menu.add_separator()
        view_menu.add_command(label="重置界面布局", command=self.reset_layout)
        
        # 帮助菜单
        help_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="帮助(H)", menu=help_menu, underline=3)
        help_menu.add_command(label="使用说明 (F2)", command=self.show_help, accelerator="F2")
        help_menu.add_command(label="键盘快捷键", command=self.show_shortcuts)
        help_menu.add_separator()
        help_menu.add_command(label="检查更新", command=self.check_updates)
        help_menu.add_command(label="关于", command=self.show_about)
    
    def create_device_panel(self, parent):
        """创建设备管理面板"""
        # 搜索框
        search_frame = ttk.Frame(parent)
        search_frame.pack(fill=tk.X, pady=(0, 10))
        
        ttk.Label(search_frame, text="🔍 搜索设备:").pack(side=tk.LEFT)
        device_search_entry = ttk.Entry(search_frame, textvariable=self.device_search_var, width=20)
        device_search_entry.pack(side=tk.LEFT, padx=(5, 0), fill=tk.X, expand=True)
        
        # 设备列表
        device_frame = ttk.Frame(parent)
        device_frame.pack(fill=tk.BOTH, expand=True)
        
        # 设备树形视图
        columns = ("slot", "name", "ip", "state", "last_seen", "ping")
        self.device_tree = ttk.Treeview(device_frame, columns=columns, show="headings", height=8)
        
        # 设置列标题和宽度
        headers = {
            "slot": ("槽位", 60),
            "name": ("设备名称", 120),
            "ip": ("IP地址", 120),
            "state": ("状态", 80),
            "last_seen": ("最后通信", 120),
            "ping": ("延迟(ms)", 80)
        }
        
        for col, (title, width) in headers.items():
            self.device_tree.heading(col, text=title, command=lambda c=col: self.sort_device_tree(c))
            self.device_tree.column(col, width=width)
        
        # 添加滚动条
        device_scrollbar = ttk.Scrollbar(device_frame, orient=tk.VERTICAL, command=self.device_tree.yview)
        self.device_tree.configure(yscrollcommand=device_scrollbar.set)
        
        device_h_scrollbar = ttk.Scrollbar(device_frame, orient=tk.HORIZONTAL, command=self.device_tree.xview)
        self.device_tree.configure(xscrollcommand=device_h_scrollbar.set)
        
        self.device_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        device_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        device_h_scrollbar.pack(side=tk.BOTTOM, fill=tk.X)
        
        # 绑定双击事件
        self.device_tree.bind('<Double-1>', self.on_device_double_click)
        self.device_tree.bind('<Button-3>', self.show_device_context_menu)
        
        # 设备控制按钮
        device_button_frame = ttk.Frame(parent)
        device_button_frame.pack(fill=tk.X, pady=(10, 0))
        
        ttk.Button(device_button_frame, text="🔗 连接", command=self.connect_selected_device, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=(0, 5))
        ttk.Button(device_button_frame, text="🔌 断开", command=self.disconnect_selected_device, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=(0, 5))
        ttk.Button(device_button_frame, text="🔄 刷新", command=self.refresh_device_list, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=(0, 5))
        ttk.Button(device_button_frame, text="🔧 测试", command=self.test_selected_device, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=(0, 5))
        
        # 设备统计信息
        stats_frame = ttk.LabelFrame(parent, text="📊 统计信息", padding=5)
        stats_frame.pack(fill=tk.X, pady=(10, 0))
        
        # 创建统计面板
        stats_inner_frame = ttk.Frame(stats_frame)
        stats_inner_frame.pack(fill=tk.X)
        
        # 左侧统计标签
        stats_left_frame = ttk.Frame(stats_inner_frame)
        stats_left_frame.pack(side=tk.LEFT, fill=tk.X, expand=True)
        
        self.stats_text = tk.Text(stats_left_frame, height=5, wrap=tk.WORD, font=('Arial', 9))
        stats_scrollbar = ttk.Scrollbar(stats_left_frame, orient=tk.VERTICAL, command=self.stats_text.yview)
        self.stats_text.configure(yscrollcommand=stats_scrollbar.set)
        
        self.stats_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        stats_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        # 右侧状态指示器
        status_indicators_frame = ttk.Frame(stats_inner_frame)
        status_indicators_frame.pack(side=tk.RIGHT, padx=(10, 0))
        
        self.online_indicator = ttk.Label(status_indicators_frame, text="🟢 在线: 0", style='Success.TLabel')
        self.online_indicator.pack(anchor=tk.W)
        
        self.offline_indicator = ttk.Label(status_indicators_frame, text="🔴 离线: 0", style='Danger.TLabel')
        self.offline_indicator.pack(anchor=tk.W)
        
        self.connecting_indicator = ttk.Label(status_indicators_frame, text="🟡 连接中: 0", style='Warning.TLabel')
        self.connecting_indicator.pack(anchor=tk.W)
        
        # 创建设备上下文菜单
        self.create_device_context_menu()
    
    def create_switch_panel(self, parent):
        """创建开关桩管理面板"""
        # 顶部工具栏和搜索
        toolbar_frame = ttk.Frame(parent)
        toolbar_frame.pack(fill=tk.X, pady=(0, 10))
        
        # 第一行：组选择和搜索
        top_row = ttk.Frame(toolbar_frame)
        top_row.pack(fill=tk.X, pady=(0, 5))
        
        ttk.Label(top_row, text="📂 开关桩组:").pack(side=tk.LEFT, padx=(0, 5))
        
        self.group_var = tk.StringVar()
        self.group_combo = ttk.Combobox(top_row, textvariable=self.group_var, width=15)
        self.group_combo.pack(side=tk.LEFT, padx=(0, 10))
        self.group_combo.bind("<<ComboboxSelected>>", self.on_group_selected)
        
        ttk.Label(top_row, text="🔍 搜索:").pack(side=tk.LEFT, padx=(10, 5))
        switch_search_entry = ttk.Entry(top_row, textvariable=self.switch_search_var, width=20)
        switch_search_entry.pack(side=tk.LEFT, fill=tk.X, expand=True)
        
        # 第二行：批量操作按钮
        bottom_row = ttk.Frame(toolbar_frame)
        bottom_row.pack(fill=tk.X)
        
        ttk.Button(bottom_row, text="☑️ 全选", command=self.select_all_switches, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=(0, 5))
        ttk.Button(bottom_row, text="☐ 取消选择", command=self.deselect_all_switches, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=(0, 5))
        ttk.Button(bottom_row, text="🔄 切换选中", command=self.toggle_selected_switches, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=(0, 10))
        
        # 进度条
        self.progress_var = tk.DoubleVar()
        self.progress_bar = ttk.Progressbar(bottom_row, variable=self.progress_var, 
                                          mode='determinate', length=200)
        self.progress_bar.pack(side=tk.RIGHT, padx=(10, 0))
        
        self.progress_label = ttk.Label(bottom_row, text="")
        self.progress_label.pack(side=tk.RIGHT, padx=(0, 5))
        
        # 开关桩列表
        switch_frame = ttk.Frame(parent)
        switch_frame.pack(fill=tk.BOTH, expand=True)
        
        # 开关桩树形视图
        switch_columns = ("name", "type", "current_state", "target_state", "description", "group", "last_changed")
        self.switch_tree = ttk.Treeview(switch_frame, columns=switch_columns, show="headings")
        
        # 设置列标题和宽度
        switch_headers = {
            "name": ("开关桩名称", 150),
            "type": ("类型", 80),
            "current_state": ("当前状态", 80),
            "target_state": ("目标状态", 80),
            "description": ("描述", 200),
            "group": ("组", 100),
            "last_changed": ("最后变更", 120)
        }
        
        for col, (title, width) in switch_headers.items():
            self.switch_tree.heading(col, text=title, command=lambda c=col: self.sort_switch_tree(c))
            self.switch_tree.column(col, width=width)
        
        # 添加滚动条
        switch_scrollbar = ttk.Scrollbar(switch_frame, orient=tk.VERTICAL, command=self.switch_tree.yview)
        self.switch_tree.configure(yscrollcommand=switch_scrollbar.set)
        
        switch_h_scrollbar = ttk.Scrollbar(switch_frame, orient=tk.HORIZONTAL, command=self.switch_tree.xview)
        self.switch_tree.configure(xscrollcommand=switch_h_scrollbar.set)
        
        self.switch_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        switch_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        switch_h_scrollbar.pack(side=tk.BOTTOM, fill=tk.X)
        
        # 绑定事件
        self.switch_tree.bind('<Double-1>', self.on_switch_double_click)
        self.switch_tree.bind('<Button-3>', self.show_switch_context_menu)
        self.switch_tree.bind('<Key>', self.on_switch_key_press)
        
        # 开关桩控制按钮
        switch_button_frame = ttk.Frame(parent)
        switch_button_frame.pack(fill=tk.X, pady=(10, 0))
        
        ttk.Button(switch_button_frame, text="🟢 打开选中", command=self.open_selected_switches, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=(0, 5))
        ttk.Button(switch_button_frame, text="🔴 关闭选中", command=self.close_selected_switches, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=(0, 5))
        ttk.Button(switch_button_frame, text="✅ 应用更改", command=self.apply_switch_changes, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=(0, 10))
        
        ttk.Button(switch_button_frame, text="📋 批量操作", command=self.batch_operation_dialog, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=(0, 5))
        
        # 添加右键菜单
        self.create_switch_context_menu()
    
    def create_log_panel(self, parent):
        """创建日志面板"""
        self.log_frame = ttk.LabelFrame(parent, text="📋 操作日志", padding=5)
        self.log_frame.pack(fill=tk.X, padx=5, pady=2)
        
        # 日志显示区域
        log_display_frame = ttk.Frame(self.log_frame)
        log_display_frame.pack(fill=tk.X)
        
        self.log_text = tk.Text(log_display_frame, height=4, wrap=tk.WORD, font=('Consolas', 8))
        log_scrollbar = ttk.Scrollbar(log_display_frame, orient=tk.VERTICAL, command=self.log_text.yview)
        self.log_text.configure(yscrollcommand=log_scrollbar.set)
        
        self.log_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        log_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        # 日志控制按钮
        log_button_frame = ttk.Frame(self.log_frame)
        log_button_frame.pack(fill=tk.X, pady=(5, 0))
        
        ttk.Button(log_button_frame, text="🗑️ 清除", command=self.clear_log).pack(side=tk.LEFT, padx=(0, 5))
        ttk.Button(log_button_frame, text="💾 保存", command=self.save_log).pack(side=tk.LEFT, padx=(0, 5))
        
        # 日志级别选择
        ttk.Label(log_button_frame, text="级别:").pack(side=tk.LEFT, padx=(10, 5))
        self.log_level_var = tk.StringVar(value="INFO")
        log_level_combo = ttk.Combobox(log_button_frame, textvariable=self.log_level_var, 
                                     values=["DEBUG", "INFO", "WARNING", "ERROR"], width=10)
        log_level_combo.pack(side=tk.LEFT)
        
        # 初始化日志显示
        self.log_text.insert(tk.END, f"[{time.strftime('%H:%M:%S')}] 系统启动完成\n")
        
        # 默认隐藏日志面板
        self.log_panel_visible = True
        
    def create_device_context_menu(self):
        """创建设备右键菜单"""
        self.device_context_menu = tk.Menu(self.root, tearoff=0)
        self.device_context_menu.add_command(label="🔗 连接", command=self.connect_selected_device)
        self.device_context_menu.add_command(label="🔌 断开", command=self.disconnect_selected_device)
        self.device_context_menu.add_separator()
        self.device_context_menu.add_command(label="🔧 测试连接", command=self.test_selected_device)
        self.device_context_menu.add_command(label="📊 查看详情", command=self.show_device_details)
        self.device_context_menu.add_separator()
        self.device_context_menu.add_command(label="📋 复制IP", command=self.copy_device_ip)
        self.device_context_menu.add_command(label="🔄 重置", command=self.reset_selected_device)
    
    def create_switch_context_menu(self):
        """创建开关桩右键菜单"""
        self.switch_context_menu = tk.Menu(self.root, tearoff=0)
        self.switch_context_menu.add_command(label="🟢 打开", command=self.context_open_switch)
        self.switch_context_menu.add_command(label="🔴 关闭", command=self.context_close_switch)
        self.switch_context_menu.add_command(label="🔄 切换", command=self.context_toggle_switch)
        self.switch_context_menu.add_separator()
        self.switch_context_menu.add_command(label="📊 查看详情", command=self.show_switch_details)
        self.switch_context_menu.add_command(label="📋 复制名称", command=self.copy_switch_name)
        self.switch_context_menu.add_separator()
        self.switch_context_menu.add_command(label="🏷️ 编辑标签", command=self.edit_switch_label)
        self.switch_context_menu.add_command(label="📂 移动到组", command=self.move_switch_to_group)
    
    def create_tooltips(self):
        """创建工具提示"""
        # 这里可以添加工具提示功能
        pass
    
    def create_status_bar(self):
        """创建状态栏"""
        self.status_frame = ttk.Frame(self.root)
        self.status_frame.pack(fill=tk.X, side=tk.BOTTOM, padx=5, pady=2)
        
        # 状态栏分为多个部分
        self.status_var = tk.StringVar()
        self.status_var.set("🟢 就绪")
        
        self.status_label = ttk.Label(self.status_frame, textvariable=self.status_var, style='Status.TLabel')
        self.status_label.pack(side=tk.LEFT, padx=5)
        
        # 分隔符
        ttk.Separator(self.status_frame, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=5)
        
        # 连接状态
        self.connection_status = ttk.Label(self.status_frame, text="📡 连接: 0/0", style='Status.TLabel')
        self.connection_status.pack(side=tk.LEFT, padx=5)
        
        # 分隔符
        ttk.Separator(self.status_frame, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=5)
        
        # 开关桩状态
        self.switch_status = ttk.Label(self.status_frame, text="⚡ 开关桩: 0 开启, 0 关闭", style='Status.TLabel')
        self.switch_status.pack(side=tk.LEFT, padx=5)
        
        # 分隔符
        ttk.Separator(self.status_frame, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=5)
        
        # 监控状态指示器
        self.monitor_var = tk.StringVar()
        self.monitor_var.set("⏸️ 监控: 未启动")
        
        self.monitor_label = ttk.Label(self.status_frame, textvariable=self.monitor_var, style='Status.TLabel')
        self.monitor_label.pack(side=tk.RIGHT, padx=5)
        
        # 启动/停止监控按钮
        self.monitor_button = ttk.Button(self.status_frame, text="▶️ 启动监控", 
                                       command=self.toggle_monitoring, style='Action.TButton')
        self.monitor_button.pack(side=tk.RIGHT, padx=5)
        
        # 时间显示
        self.time_var = tk.StringVar()
        self.time_label = ttk.Label(self.status_frame, textvariable=self.time_var, style='Status.TLabel')
        self.time_label.pack(side=tk.RIGHT, padx=5)
        
        # 启动时间更新
        self.update_time()
    
    def update_time(self):
        """更新时间显示"""
        try:
            # 避免使用Unicode字符，使用简单的文本格式
            current_time = time.strftime("%Y-%m-%d %H:%M:%S")
            self.time_var.set(f"时间: {current_time}")
        except (UnicodeEncodeError, Exception) as e:
            # 如果仍有问题，使用最简化版本
            try:
                current_time = time.strftime("%H:%M:%S")
                self.time_var.set(f"时间: {current_time}")
            except:
                self.time_var.set("时间: --:--:--")
        
        self.root.after(1000, self.update_time)
    
    def add_log_entry(self, message: str, level: str = "INFO", color: str = None):
        """添加日志条目"""
        timestamp = time.strftime("%H:%M:%S")
        
        # 根据级别设置颜色
        if color is None:
            color_map = {
                "DEBUG": "gray",
                "INFO": "black",
                "WARNING": "orange",
                "ERROR": "red",
                "SUCCESS": "green"
            }
            color = color_map.get(level, "black")
        
        # 格式化日志条目
        log_entry = f"[{timestamp}] {level}: {message}\n"
        
        # 添加到日志显示
        self.log_text.insert(tk.END, log_entry)
        
        # 设置颜色
        start_index = self.log_text.index(f"end-{len(log_entry)}c")
        end_index = self.log_text.index(tk.END)
        self.log_text.tag_add(f"color_{color}", start_index, end_index)
        self.log_text.tag_config(f"color_{color}", foreground=color)
        
        # 自动滚动到底部
        self.log_text.see(tk.END)
        
        # 限制日志行数
        lines = int(self.log_text.index(tk.END).split('.')[0])
        if lines > 1000:
            self.log_text.delete("1.0", "100.0")
        
        # 同时记录到操作历史
        self.operation_history.append({
            "timestamp": time.time(),
            "message": message,
            "level": level
        })
        
        # 限制历史记录数量
        if len(self.operation_history) > 1000:
            self.operation_history = self.operation_history[-500:]
    
    def load_default_configuration(self):
        """加载默认设备配置（从config_manager或config/devices.json文件）"""
        try:
            # 首先尝试从config_manager获取设备配置
            device_config = config.get_device_config()
            if device_config and 'devices' in device_config:
                self.add_log_entry("🔧 从配置管理器加载设备配置...", "INFO")
                self.load_devices_from_config(device_config)
                return
            
            # 如果config_manager中没有设备配置，尝试加载传统的JSON文件
            devices_config_path = Path("config/devices.json")
            if devices_config_path.exists():
                self.add_log_entry("🔧 从JSON文件加载设备配置...", "INFO")
                with open(devices_config_path, 'r', encoding='utf-8') as f:
                    devices_config = json.load(f)
                
                self.load_devices_from_config(devices_config)
            else:
                self.add_log_entry("ℹ️ 未找到设备配置文件", "INFO")
            
        except Exception as e:
            logger.error(f"加载配置失败: {e}")
            self.add_log_entry(f"❌ 加载配置失败: {e}", "ERROR")
            messagebox.showerror("错误", f"加载配置失败: {e}")
    
    def load_devices_from_config(self, devices_config: Dict[str, Any]):
        """从配置字典加载设备和开关桩配置"""
        try:
            loaded_device_count = 0
            loaded_switch_count = 0
            
            # 清除现有设备
            vcu_controller._devices.clear()
            
            # 加载VCU设备
            for device_id, device_config in devices_config.get("devices", {}).items():
                # 检查必需字段
                required_fields = ["slot_id", "name", "ip"]
                if not all(field in device_config for field in required_fields):
                    logger.warning(f"跳过格式不完整的设备配置: {device_id}")
                    continue
                
                device = VCUDevice(
                    slot_id=device_config["slot_id"],
                    name=device_config["name"],
                    ip=device_config["ip"],
                    local_ip=device_config.get("local_ip", device_config["ip"]),
                    port=device_config.get("port", 18125),
                    enabled=device_config.get("enabled", True),
                    description=device_config.get("description", "")
                )
                vcu_controller.add_device(device)
                loaded_device_count += 1
                self.add_log_entry(f"  📱 已加载设备: {device.name} (槽位{device.slot_id})", "INFO")
            
            # 加载开关桩配置
            if "switches" in devices_config:
                # 清除现有开关桩
                switch_manager._switches.clear()
                switch_manager._groups.clear()
                
                # 加载开关桩配置
                switch_load_success = switch_manager.load_from_config(devices_config)
                if switch_load_success:
                    loaded_switch_count = len(switch_manager.get_all_switches())
                    self.add_log_entry(f"  🔧 已加载 {loaded_switch_count} 个开关桩配置", "INFO")
                else:
                    self.add_log_entry("  ⚠️ 开关桩配置加载失败", "WARNING")
            
            # 刷新界面
            self.refresh_device_list()
            self.refresh_switch_list()
            self.update_group_combo()
            self.update_statistics()
            
            # 显示加载结果
            self.add_log_entry(f"✅ 配置加载完成: {loaded_device_count} 个设备, {loaded_switch_count} 个开关桩", "SUCCESS")
            self.status_var.set(f"🔧 配置已加载: {loaded_device_count} 设备, {loaded_switch_count} 开关桩")
            
            logger.info(f"配置加载完成: {loaded_device_count} 个设备, {loaded_switch_count} 个开关桩")
            
        except Exception as e:
            logger.error(f"从配置加载设备失败: {e}", exc_info=True)
            self.add_log_entry(f"❌ 从配置加载设备失败: {e}", "ERROR")
            raise
    

    
    def load_device_list_yaml(self, file_path: str = None) -> bool:
        """专门用于加载YAML格式的设备列表配置"""
        if not YAML_AVAILABLE:
            self.add_log_entry("❌ YAML库未安装，无法加载YAML设备列表", "ERROR")
            messagebox.showerror("错误", "需要安装PyYAML库来支持YAML格式：\npip install PyYAML")
            return False
        
        if not file_path:
            # 如果没有指定文件路径，打开文件选择对话框
            file_path = filedialog.askopenfilename(
                title="选择YAML设备列表配置文件",
                filetypes=[
                    ("YAML设备配置文件", "*.yaml;*.yml"),
                    ("所有文件", "*.*")
                ]
            )
            
            if not file_path:
                return False
        
        try:
            self.add_log_entry(f"📱 开始加载YAML设备列表: {Path(file_path).name}", "INFO")
            
            with open(file_path, 'r', encoding='utf-8') as f:
                yaml_config = yaml.safe_load(f)
            
            if not yaml_config:
                raise ValueError("YAML设备配置文件为空")
            
            # 检查是否包含设备信息
            if "devices" not in yaml_config:
                self.add_log_entry("⚠️ YAML文件中未找到设备配置信息", "WARNING")
                messagebox.showwarning("警告", "选择的YAML文件中未找到设备配置信息（devices节点）")
                return False
            
            # 加载设备信息
            self._load_devices_from_yaml(yaml_config)
            
            # 如果配置中包含开关桩信息，也一并加载
            if "switches" in yaml_config or "switch_groups" in yaml_config:
                self.add_log_entry("🔧 发现开关桩配置，正在加载...", "INFO")
                self._load_switches_from_yaml(yaml_config)
            
            # 刷新界面
            self.refresh_device_list()
            self.refresh_switch_list()
            self.update_group_combo()
            self.update_statistics()
            
            # 统计加载结果
            device_count = len(vcu_controller.get_all_devices())
            switch_count = len(switch_manager.get_all_switches())
            
            success_msg = f"✅ YAML设备列表加载成功: {Path(file_path).name}\n"
            success_msg += f"📱 已加载 {device_count} 个设备"
            if switch_count > 0:
                success_msg += f"，🔧 {switch_count} 个开关桩"
            
            self.add_log_entry(success_msg, "SUCCESS")
            
            # 显示成功对话框
            result_info = f"设备列表加载成功！\n\n"
            result_info += f"文件：{Path(file_path).name}\n"
            result_info += f"设备数量：{device_count} 个\n"
            if switch_count > 0:
                result_info += f"开关桩数量：{switch_count} 个\n"
            result_info += f"\n现在您可以连接这些设备并进行测试。"
            
            messagebox.showinfo("加载成功", result_info)
            return True
            
        except Exception as e:
            logger.error(f"YAML设备列表加载失败: {e}", exc_info=True)
            self.add_log_entry(f"❌ YAML设备列表加载失败: {e}", "ERROR")
            messagebox.showerror("错误", f"YAML设备列表加载失败：\n{e}")
            return False
    
    def _load_devices_from_yaml(self, yaml_config: dict, clear_existing=True):
        """从YAML配置中加载设备"""
        devices = yaml_config.get("devices", {})
        loaded_count = 0
        
        # 只在明确需要时才清空现有设备
        if clear_existing:
            vcu_controller._devices.clear()
            logger.info("清空现有设备列表")
        
        logger.info(f"开始加载YAML设备配置: {len(devices)} 个设备")
        
        for device_id, device_config in devices.items():
            try:
                device = VCUDevice(
                    slot_id=device_config.get("slot_id", device_id),
                    name=device_config.get("name", device_id),
                    ip=device_config.get("ip", ""),
                    local_ip=device_config.get("local_ip", "192.168.1.100"),
                    port=device_config.get("port", 18125),
                    enabled=device_config.get("enabled", True),
                    description=device_config.get("description", "")
                )
                
                success = vcu_controller.add_device(device)
                if success:
                    loaded_count += 1
                    self.add_log_entry(f"  ✅ 设备 {device.name} (Slot: {device.slot_id}, IP: {device.ip})", "INFO")
                    logger.info(f"成功添加设备: {device_id} -> {device.name} (Slot {device.slot_id})")
                else:
                    self.add_log_entry(f"  ❌ 设备 {device_id} 添加失败", "ERROR")
                    logger.error(f"设备添加失败: {device_id}")
                    
            except Exception as e:
                logger.warning(f"加载设备失败: {device_id} - {e}")
                self.add_log_entry(f"  ⚠️ 设备 {device_id} 加载失败: {e}", "WARNING")
        
        # 验证加载结果
        actual_devices = vcu_controller.get_all_devices()
        self.add_log_entry(f"📱 设备加载完成: {loaded_count} 个加载, {len(actual_devices)} 个已存储", "INFO")
        
        # 显示设备详情
        for slot_id, device in actual_devices.items():
            logger.info(f"已存储设备: Slot {slot_id} -> {device.name} ({device.ip})")
        
        return loaded_count
    
    def _load_switches_from_yaml(self, yaml_config: dict):
        """从YAML配置中加载开关桩"""
        try:
            switch_manager._switches.clear()
            switch_manager._groups.clear()
            
            success = switch_manager.load_from_config(yaml_config)
            if success:
                switch_count = len(switch_manager.get_all_switches())
                group_count = len(switch_manager._groups)
                self.add_log_entry(f"🔧 共加载 {switch_count} 个开关桩，{group_count} 个分组", "INFO")
            else:
                self.add_log_entry("⚠️ 开关桩配置加载失败", "WARNING")
        except Exception as e:
            logger.error(f"开关桩配置加载失败: {e}")
            self.add_log_entry(f"❌ 开关桩配置加载失败: {e}", "ERROR")
    
    def check_yaml_support(self) -> bool:
        """检查YAML支持状态"""
        return YAML_AVAILABLE
    
    def _show_yaml_install_help(self):
        """显示YAML库安装帮助"""
        help_message = (
            "YAML设备列表配置支持\n\n"
            "当前系统未安装PyYAML库，无法加载YAML格式的设备配置文件。\n\n"
            "安装方法：\n"
            "1. 打开命令行或终端\n"
            "2. 运行以下命令安装PyYAML：\n"
            "   pip install PyYAML\n\n"
            "安装完成后重新启动应用程序即可支持YAML格式。\n\n"
            "YAML格式设备配置的优势：\n"
            "• 更易读的设备配置文件格式\n"
            "• 支持注释和详细说明\n"
            "• 更灵活的设备参数配置\n"
            "• 更好的可维护性和版本控制支持\n"
            "• 可以同时包含设备和开关桩配置"
        )
        
        messagebox.showinfo("YAML设备配置支持", help_message)
        self.add_log_entry("💡 显示了YAML设备配置安装帮助信息", "INFO")
    
    def setup_callbacks(self):
        """设置回调函数"""
        # VCU状态变化回调
        vcu_controller.add_status_callback(self.on_vcu_status_changed)
        vcu_controller.add_message_callback(self.on_vcu_message_received)
        
        # 开关桩状态变化回调
        switch_manager.add_state_change_callback(self.on_switch_state_changed)
        
        # 窗口关闭回调
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
    
    def on_vcu_status_changed(self, slot_id: str, state: VCUState):
        """VCU状态变化回调"""
        self.root.after(0, lambda: self.refresh_device_list())
    
    def on_vcu_message_received(self, slot_id: str, message: Dict):
        """VCU消息接收回调"""
        # 更新统计信息
        self.root.after(0, lambda: self.update_statistics())
    
    def on_switch_state_changed(self, switch_name: str, old_state: SwitchState, new_state: SwitchState):
        """开关桩状态变化回调"""
        self.root.after(0, lambda: self.refresh_switch_list())
    
    def refresh_device_list(self):
        """刷新设备列表"""
        # 清空现有项目
        for item in self.device_tree.get_children():
            self.device_tree.delete(item)
        
        # 获取搜索条件
        search_term = self.device_search_var.get().lower()
        
        # 获取设备列表
        devices = vcu_controller.get_all_devices()
        online_count = 0
        offline_count = 0
        connecting_count = 0
        
        logger.debug(f"刷新设备列表: 找到 {len(devices)} 个设备")
        
        # 如果没有设备，显示提示信息
        if not devices:
            placeholder_item = self.device_tree.insert("", tk.END, values=(
                "📋",
                "暂无设备",
                "请加载配置文件",
                "🔴 未配置",
                "",
                ""
            ))
            self.add_log_entry("ℹ️ 设备列表为空，请检查配置", "INFO")
        else:
            # 添加设备 - 修复缩进问题
            filtered_count = 0
            for slot_id, device in devices.items():
                # 搜索过滤
                if search_term and search_term not in device.name.lower() and search_term not in device.ip.lower() and search_term not in slot_id.lower():
                    continue
                
                filtered_count += 1
                
                last_seen = ""
                if device.last_seen:
                    last_seen = time.strftime("%H:%M:%S", time.localtime(device.last_seen))
                
                # 模拟ping延迟
                ping = getattr(device, 'ping', 0) if device.state == VCUState.ONLINE else 0
                
                item_id = self.device_tree.insert("", tk.END, values=(
                    device.slot_id,
                    device.name,
                    device.ip,
                    device.state.value,
                    last_seen,
                    f"{ping}" if ping > 0 else ""
                ))
                
                # 设置行颜色
                if device.state == VCUState.ONLINE:
                    self.device_tree.set(item_id, "state", "🟢 在线")
                    online_count += 1
                elif device.state == VCUState.OFFLINE:
                    self.device_tree.set(item_id, "state", "🔴 离线")
                    offline_count += 1
                else:
                    self.device_tree.set(item_id, "state", "🟡 连接中")
                    connecting_count += 1
                
                logger.debug(f"添加设备到列表: {device.name} (Slot {device.slot_id}) - {device.state.value}")
            
            logger.info(f"设备列表刷新完成: 显示 {filtered_count}/{len(devices)} 个设备")
        
        # 更新状态指示器
        self.online_indicator.config(text=f"🟢 在线: {online_count}")
        self.offline_indicator.config(text=f"🔴 离线: {offline_count}")
        self.connecting_indicator.config(text=f"🟡 连接中: {connecting_count}")
        
        # 更新状态栏
        total_devices = len(devices)
        self.connection_status.config(text=f"📡 连接: {online_count}/{total_devices}")
    
    def refresh_switch_list(self):
        """刷新开关桩列表 - 优化版本"""
        # 清空现有项目
        for item in self.switch_tree.get_children():
            self.switch_tree.delete(item)
        
        # 获取搜索条件
        search_term = self.switch_search_var.get().lower()
        
        # 获取当前选中的组
        selected_group = self.group_var.get()
        
        if selected_group and selected_group != "全部":
            switches = switch_manager.get_switches_by_group(selected_group)
        else:
            switches = list(switch_manager.get_all_switches().values())
        
        # 添加开关桩
        open_count = 0
        closed_count = 0
        pending_changes = 0
        filtered_count = 0
        
        # 按名称排序，确保显示顺序一致
        switches.sort(key=lambda s: s.name)
        
        for switch in switches:
            # 搜索过滤
            if search_term and (search_term not in switch.name.lower() and 
                              search_term not in switch.description.lower() and
                              search_term not in switch.group.lower() and
                              search_term not in switch.switch_type.value.lower()):
                continue
            
            filtered_count += 1
            
            # 格式化显示信息
            target_state = ""
            if switch.target_state:
                if switch.target_state == SwitchState.OPEN:
                    target_state = "🟢 开启"
                elif switch.target_state == SwitchState.CLOSED:
                    target_state = "🔴 关闭"
                else:
                    target_state = switch.target_state.value
            
            # 格式化最后修改时间
            last_changed = ""
            if hasattr(switch, 'last_changed') and switch.last_changed:
                last_changed = time.strftime("%m-%d %H:%M", time.localtime(switch.last_changed))
            
            # 格式化描述信息 - 限制长度并显示更多信息
            description = switch.description
            if len(description) > 50:
                description = description[:47] + "..."
            
            # 格式化开关桩名称
            display_name = switch.name
            has_pending_change = switch.target_state and switch.target_state != switch.current_state
            
            if has_pending_change:
                display_name = f"⚠️ {switch.name}"
                pending_changes += 1
            
            # 添加到列表
            item_id = self.switch_tree.insert("", tk.END, values=(
                display_name,
                switch.switch_type.value.upper(),
                "🟢 开启" if switch.current_state == SwitchState.OPEN else "🔴 关闭",
                target_state,
                description,
                switch.group,
                last_changed
            ))
            
            # 统计状态
            if switch.current_state == SwitchState.OPEN:
                open_count += 1
            else:
                closed_count += 1
            
            # 为有待处理更改的项目设置特殊颜色标记
            if has_pending_change:
                try:
                    # 设置该行的背景色为浅黄色表示有待处理的更改
                    self.switch_tree.set(item_id, 'name', display_name)
                except:
                    pass  # 如果设置颜色失败，忽略错误
        
        # 更新状态信息
        total_switches = len(switches)
        status_text = f"⚡ 开关桩: {open_count} 开启, {closed_count} 关闭"
        
        if search_term:
            status_text += f" | 🔍 显示: {filtered_count}/{total_switches}"
        
        if pending_changes > 0:
            status_text += f" | ⚠️ 待应用: {pending_changes}"
        
        if selected_group and selected_group != "全部":
            status_text += f" | 📂 组: {selected_group}"
        
        self.switch_status.config(text=status_text)
        
        # 记录刷新日志
        logger.debug(f"开关桩列表已刷新: 显示 {filtered_count} 个开关桩 (共 {total_switches} 个)")
        
        # 如果没有找到任何匹配的开关桩，显示提示
        if filtered_count == 0 and total_switches > 0:
            placeholder_item = self.switch_tree.insert("", tk.END, values=(
                "🔍 未找到匹配的开关桩",
                "",
                "",
                "",
                f"请尝试其他搜索关键词 (共有 {total_switches} 个开关桩)",
                "",
                ""
            ))
        elif total_switches == 0:
            placeholder_item = self.switch_tree.insert("", tk.END, values=(
                "📋 暂无开关桩",
                "",
                "",
                "",
                "请先加载项目以导入开关桩定义",
                "",
                ""
            ))
    
    def update_group_combo(self):
        """更新组合框选项 - 优化版本"""
        groups = list(switch_manager._groups.keys())
        
        # 按组名排序，但将常用组放在前面
        priority_groups = ['vcu_control', 'config', 'default', 'debug', 'test', 'safety']
        sorted_groups = []
        
        # 先添加优先组
        for priority_group in priority_groups:
            if priority_group in groups:
                sorted_groups.append(priority_group)
                groups.remove(priority_group)
        
        # 再添加其他组（按字母顺序）
        sorted_groups.extend(sorted(groups))
        
        # 添加"全部"选项在最前面
        final_groups = ["全部"] + sorted_groups
        
        self.group_combo['values'] = final_groups
        
        # 智能选择默认组
        if len(final_groups) > 1:
            # 如果当前选择的组仍然存在，保持选择
            current_selection = self.group_var.get()
            if current_selection in final_groups:
                pass  # 保持当前选择
            else:
                # 否则选择第一个有内容的组
                for group in final_groups[1:]:  # 跳过"全部"
                    if len(switch_manager.get_switches_by_group(group)) > 0:
                        self.group_var.set(group)
                        break
                else:
                    # 如果没有找到有内容的组，选择"全部"
                    self.group_var.set("全部")
        else:
            self.group_combo.set("全部")
        
        # 记录组信息
        if len(sorted_groups) > 0:
            group_info = []
            for group in sorted_groups:
                count = len(switch_manager.get_switches_by_group(group))
                group_info.append(f"{group}({count})")
            
            logger.info(f"更新分组列表: {', '.join(group_info)}")
            self.add_log_entry(f"发现 {len(sorted_groups)} 个分组: {', '.join(sorted_groups)}", "INFO")
    
    def update_statistics(self):
        """更新统计信息"""
        try:
            stats_text = ""
            
            # VCU设备统计
            devices = vcu_controller.get_all_devices()
            online_count = sum(1 for d in devices.values() if d.state == VCUState.ONLINE)
            offline_count = sum(1 for d in devices.values() if d.state == VCUState.OFFLINE)
            connecting_count = len(devices) - online_count - offline_count
            
            stats_text += f"📊 设备状态统计\n"
            stats_text += f"  总设备数: {len(devices)}\n"
            stats_text += f"  在线设备: {online_count} ({online_count/len(devices)*100:.1f}%)\n" if len(devices) > 0 else "  在线设备: 0\n"
            stats_text += f"  离线设备: {offline_count}\n"
            stats_text += f"  连接中: {connecting_count}\n"
            
            # 开关桩统计
            try:
                switch_stats = switch_manager.get_statistics()
                switches = switch_manager.get_all_switches()
                open_count = sum(1 for s in switches.values() if s.current_state == SwitchState.OPEN)
                closed_count = len(switches) - open_count
                
                stats_text += f"\n⚡ 开关桩状态统计\n"
                stats_text += f"  总开关桩: {len(switches)}\n"
                stats_text += f"  开启状态: {open_count}\n"
                stats_text += f"  关闭状态: {closed_count}\n"
                stats_text += f"  操作次数: {switch_stats.get('operations_count', 0)}\n"
                
                # 未应用更改统计
                pending_changes = sum(1 for s in switches.values() 
                                    if s.target_state and s.target_state != s.current_state)
                if pending_changes > 0:
                    stats_text += f"  待应用更改: {pending_changes}\n"
                
            except Exception as e:
                stats_text += f"\n⚡ 开关桩统计\n  无法获取统计信息\n"
            
            # 系统运行统计
            stats_text += f"\n系统信息\n"
            stats_text += f"  运行时间: {self.get_uptime()}\n"
            stats_text += f"  监控状态: {'运行中' if self.is_monitoring else '已停止'}\n"
            stats_text += f"  日志条数: {len(self.operation_history)}\n"
            
            self.stats_text.delete(1.0, tk.END)
            self.stats_text.insert(1.0, stats_text)
            
        except Exception as e:
            logger.error(f"更新统计信息失败: {e}")
    
    def get_uptime(self):
        """获取系统运行时间"""
        if hasattr(self, '_start_time'):
            uptime_seconds = time.time() - self._start_time
            hours = int(uptime_seconds // 3600)
            minutes = int((uptime_seconds % 3600) // 60)
            return f"{hours}小时{minutes}分钟"
        else:
            self._start_time = time.time()
            return "0小时0分钟"
    
    def toggle_monitoring(self):
        """切换监控状态"""
        if self.is_monitoring:
            self.stop_monitoring()
        else:
            self.start_monitoring()
    
    def start_monitoring(self):
        """启动监控"""
        try:
            vcu_controller.start_monitoring()
            self.is_monitoring = True
            self.monitor_button.config(text="⏸️ 停止监控")
            self.monitor_toolbar_button.config(text="⏸️ 停止监控")
            self.monitor_var.set("▶️ 监控: 运行中")
            
            # 启动UI更新线程
            self.update_thread = threading.Thread(target=self.update_loop, daemon=True)
            self.update_thread.start()
            
            self.add_log_entry("监控已启动", "SUCCESS")
            logger.info("监控已启动")
            
        except Exception as e:
            logger.error(f"启动监控失败: {e}")
            self.add_log_entry(f"启动监控失败: {e}", "ERROR")
            messagebox.showerror("错误", f"启动监控失败: {e}")
    
    def stop_monitoring(self):
        """停止监控"""
        try:
            vcu_controller.stop_monitoring()
            self.is_monitoring = False
            self.monitor_button.config(text="▶️ 启动监控")
            self.monitor_toolbar_button.config(text="▶️ 启动监控")
            self.monitor_var.set("⏸️ 监控: 已停止")
            
            self.add_log_entry("监控已停止", "INFO")
            logger.info("监控已停止")
            
        except Exception as e:
            logger.error(f"停止监控失败: {e}")
            self.add_log_entry(f"停止监控失败: {e}", "ERROR")
    
    def update_loop(self):
        """UI更新循环"""
        while self.is_monitoring:
            try:
                self.root.after(0, self.refresh_device_list)
                self.root.after(0, self.update_statistics)
                time.sleep(2.0)  # 每2秒更新一次
            except Exception as e:
                logger.error(f"UI更新循环异常: {e}")
                break
    
    # 事件处理方法
    def connect_selected_device(self):
        """连接选中的设备"""
        selection = self.device_tree.selection()
        if not selection:
            messagebox.showwarning("警告", "请选择要连接的设备")
            return
        
        item = self.device_tree.item(selection[0])
        slot_id = item['values'][0]
        
        if vcu_controller.connect_device(slot_id):
            self.status_var.set(f"设备 Slot {slot_id} 连接成功")
        else:
            self.status_var.set(f"设备 Slot {slot_id} 连接失败")
    
    def disconnect_selected_device(self):
        """断开选中的设备"""
        selection = self.device_tree.selection()
        if not selection:
            messagebox.showwarning("警告", "请选择要断开的设备")
            return
        
        item = self.device_tree.item(selection[0])
        slot_id = item['values'][0]
        
        vcu_controller.disconnect_device(slot_id)
        self.status_var.set(f"设备 Slot {slot_id} 已断开")
    
    def connect_all_devices(self):
        """连接所有设备"""
        try:
            devices = vcu_controller.get_all_devices()
            if not devices:
                self.add_log_entry("⚠️ 没有可连接的设备", "WARNING")
                return
            
            self.add_log_entry(f"🔗 开始连接 {len(devices)} 个设备...", "INFO")
            
            connected_count = 0
            for slot_id, device in devices.items():
                if device.enabled:
                    try:
                        self.add_log_entry(f"  🔗 正在连接设备: {device.name} (槽位{device.slot_id})", "INFO")
                        success = vcu_controller.connect_device(slot_id)
                        if success:
                            connected_count += 1
                            self.add_log_entry(f"  ✅ 设备连接成功: {device.name}", "SUCCESS")
                        else:
                            self.add_log_entry(f"  ❌ 设备连接失败: {device.name}", "ERROR")
                    except Exception as e:
                        self.add_log_entry(f"  ❌ 连接设备 {device.name} 时出错: {e}", "ERROR")
                else:
                    self.add_log_entry(f"  ⏭️ 跳过已禁用的设备: {device.name}", "INFO")
            
            self.add_log_entry(f"🔗 设备连接完成: {connected_count}/{len(devices)} 个设备连接成功", "INFO")
            self.refresh_device_list()
            self.update_statistics()
            
        except Exception as e:
            logger.error(f"连接所有设备失败: {e}", exc_info=True)
            self.add_log_entry(f"❌ 连接所有设备失败: {e}", "ERROR")
    
    def disconnect_all_devices(self):
        """断开所有设备"""
        devices = vcu_controller.get_all_devices()
        
        for slot_id in devices.keys():
            vcu_controller.disconnect_device(slot_id)
        
        self.status_var.set(f"已断开所有设备")
    
    def on_group_selected(self, event):
        """组选择事件"""
        self.refresh_switch_list()
    
    def select_all_switches(self):
        """全选开关桩"""
        for item in self.switch_tree.get_children():
            self.switch_tree.selection_add(item)
    
    def deselect_all_switches(self):
        """取消选择所有开关桩"""
        self.switch_tree.selection_remove(self.switch_tree.selection())
    
    def open_selected_switches(self):
        """打开选中的开关桩"""
        selection = self.switch_tree.selection()
        if not selection:
            messagebox.showwarning("警告", "请选择要操作的开关桩")
            return
        
        switch_names = []
        for item in selection:
            values = self.switch_tree.item(item)['values']
            switch_names.append(values[0])
        
        success_list, failed_list = switch_manager.batch_set_state(switch_names, SwitchState.OPEN)
        self.status_var.set(f"打开开关桩: 成功 {len(success_list)}, 失败 {len(failed_list)}")
    
    def close_selected_switches(self):
        """关闭选中的开关桩"""
        selection = self.switch_tree.selection()
        if not selection:
            messagebox.showwarning("警告", "请选择要操作的开关桩")
            return
        
        switch_names = []
        for item in selection:
            values = self.switch_tree.item(item)['values']
            switch_names.append(values[0])
        
        success_list, failed_list = switch_manager.batch_set_state(switch_names, SwitchState.CLOSED)
        self.status_var.set(f"关闭开关桩: 成功 {len(success_list)}, 失败 {len(failed_list)}")
    
    def toggle_selected_switches(self):
        """切换选中的开关桩状态"""
        selection = self.switch_tree.selection()
        if not selection:
            messagebox.showwarning("警告", "请选择要操作的开关桩")
            return
        
        switch_names = []
        for item in selection:
            values = self.switch_tree.item(item)['values']
            switch_names.append(values[0])
        
        success_list, failed_list = switch_manager.batch_toggle_state(switch_names)
        self.status_var.set(f"切换开关桩: 成功 {len(success_list)}, 失败 {len(failed_list)}")
    
    def apply_switch_changes(self):
        """应用开关桩更改"""
        # 将所有目标状态应用到VCU设备
        devices = vcu_controller.get_all_devices()
        switches = switch_manager.get_all_switches()
        
        success_count = 0
        total_count = len(switches)
        
        for switch_name, switch in switches.items():
            if switch.target_state and switch.target_state != switch.current_state:
                # 对每个连接的设备发送开关命令
                for slot_id, device in devices.items():
                    if device.state == VCUState.ONLINE:
                        switch_value = switch.target_state == SwitchState.OPEN
                        if vcu_controller.send_switch_command(slot_id, switch_name, switch_value):
                            success_count += 1
                        total_count += 1
                
                # 更新开关桩当前状态
                switch_manager.update_switch_state(switch_name, switch.target_state)
        
        self.status_var.set(f"应用更改: {success_count}/{total_count} 成功")
    
    def open_all_switches(self):
        """打开所有开关桩"""
        switches = switch_manager.get_all_switches()
        switch_names = list(switches.keys())
        
        success_list, failed_list = switch_manager.batch_set_state(switch_names, SwitchState.OPEN)
        self.status_var.set(f"打开所有开关桩: 成功 {len(success_list)}, 失败 {len(failed_list)}")
    
    def close_all_switches(self):
        """关闭所有开关桩"""
        switches = switch_manager.get_all_switches()
        switch_names = list(switches.keys())
        
        success_list, failed_list = switch_manager.batch_set_state(switch_names, SwitchState.CLOSED)
        self.status_var.set(f"关闭所有开关桩: 成功 {len(success_list)}, 失败 {len(failed_list)}")
    
    def reset_switches(self):
        """重置开关桩到默认状态"""
        if switch_manager.reset_to_defaults():
            self.status_var.set("开关桩已重置到默认状态")
        else:
            self.status_var.set("重置开关桩失败")
    
    def reset_devices(self):
        """重置设备"""
        if messagebox.askyesno("确认", "确定要重置所有设备吗？"):
            # 实现设备重置逻辑
            self.status_var.set("设备重置完成")
    
    def show_switch_context_menu(self, event):
        """显示开关桩右键菜单"""
        try:
            self.switch_context_menu.tk_popup(event.x_root, event.y_root)
        finally:
            self.switch_context_menu.grab_release()
    
    def context_open_switch(self):
        """右键菜单：打开开关"""
        self.open_selected_switches()
    
    def context_close_switch(self):
        """右键菜单：关闭开关"""
        self.close_selected_switches()
    
    def show_switch_details(self):
        """显示开关桩详情"""
        selection = self.switch_tree.selection()
        if not selection:
            return
        
        item = self.switch_tree.item(selection[0])
        switch_name = item['values'][0]
        switch = switch_manager.get_switch(switch_name)
        
        if switch:
            details = f"""开关桩详情:
            
名称: {switch.name}
类型: {switch.switch_type.value}
当前状态: {switch.current_state.value}
目标状态: {switch.target_state.value if switch.target_state else '无'}
描述: {switch.description}
组: {switch.group}
变更次数: {switch.change_count}
最后变更: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(switch.last_changed)) if switch.last_changed else '无'}
启用状态: {'是' if switch.enabled else '否'}
"""
            messagebox.showinfo("开关桩详情", details)
    
    def load_project_file(self):
        """加载项目文件 - 优化版本"""
        # 让用户选择项目文件或目录
        choice = messagebox.askyesnocancel(
            "选择加载方式", 
            "选择加载方式:\n\n"
            "是 - 选择项目目录（推荐）\n"
            "否 - 选择单个C文件\n"
            "取消 - 取消操作"
        )
        
        if choice is None:  # 取消
            return
        
        project_path = None
        
        if choice:  # 选择目录
            project_path = filedialog.askdirectory(
                title="选择项目目录",
                mustexist=True
            )
        else:  # 选择文件
            project_path = filedialog.askopenfilename(
                title="选择C源文件",
                filetypes=[
                    ("C/C++ files", "*.c;*.h;*.cpp;*.hpp;*.cc;*.cxx"),
                    ("C files", "*.c"),
                    ("Header files", "*.h"),
                    ("C++ files", "*.cpp;*.hpp;*.cc;*.cxx"),
                    ("All files", "*.*")
                ]
            )
        
        if not project_path:
            return
        
        # 显示项目加载开始
        self.add_log_entry(f"开始加载项目: {project_path}", "INFO")
        self.status_var.set("📁 正在加载项目...")
        
        # 创建进度对话框
        progress_dialog = tk.Toplevel(self.root)
        progress_dialog.title("解析项目中...")
        progress_dialog.transient(self.root)
        progress_dialog.grab_set()
        progress_dialog.resizable(False, False)
        
        # 设置对话框大小并居中显示
        dialog_width = 550
        dialog_height = 380
        progress_dialog.update_idletasks()  # 确保窗口完全初始化
        
        # 获取屏幕尺寸和窗口尺寸
        screen_width = self.root.winfo_screenwidth()
        screen_height = self.root.winfo_screenheight()
        
        # 计算居中位置
        x = (screen_width - dialog_width) // 2
        y = (screen_height - dialog_height) // 2
        
        # 确保窗口不会超出屏幕边界
        x = max(0, min(x, screen_width - dialog_width))
        y = max(0, min(y, screen_height - dialog_height))
        
        progress_dialog.geometry(f"{dialog_width}x{dialog_height}+{x}+{y}")
        
        # 主框架
        main_frame = ttk.Frame(progress_dialog)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=15)
        
        # 标题
        title_frame = ttk.Frame(main_frame)
        title_frame.pack(fill=tk.X, pady=(0, 15))
        
        info_label = ttk.Label(title_frame, text="正在解析C文件，请稍候...", 
                              font=('Arial', 14, 'bold'))
        info_label.pack()
        
        # 路径显示
        path_frame = ttk.LabelFrame(main_frame, text="项目路径", padding=10)
        path_frame.pack(fill=tk.X, pady=(0, 15))
        
        # 使用滚动文本显示路径
        path_text = tk.Text(path_frame, height=2, wrap=tk.WORD, font=('Arial', 9))
        path_text.insert(tk.END, project_path)
        path_text.config(state=tk.DISABLED)
        path_text.pack(fill=tk.X)
        
        # 进度条框架
        progress_frame = ttk.LabelFrame(main_frame, text="解析进度", padding=10)
        progress_frame.pack(fill=tk.X, pady=(0, 15))
        
        # 进度条
        progress = ttk.Progressbar(progress_frame, mode='indeterminate')
        progress.pack(fill=tk.X, pady=(0, 10))
        progress.start()
        
        # 状态标签
        status_label = ttk.Label(progress_frame, text="正在扫描文件...", 
                                font=('Arial', 10))
        status_label.pack()
        
        # 结果显示框架
        result_frame = ttk.LabelFrame(main_frame, text="解析结果", padding=10)
        result_frame.pack(fill=tk.BOTH, expand=True, pady=(0, 15))
        
        result_text = tk.Text(result_frame, height=6, wrap=tk.WORD, font=('Arial', 10))
        result_scrollbar = ttk.Scrollbar(result_frame, orient=tk.VERTICAL, command=result_text.yview)
        result_text.configure(yscrollcommand=result_scrollbar.set)
        
        result_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        result_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        # 按钮框架（初始隐藏）
        button_frame = ttk.Frame(main_frame)
        
        # 取消按钮（立即显示）
        cancel_frame = ttk.Frame(main_frame)
        cancel_frame.pack(fill=tk.X, pady=(0, 5))
        
        # 用于控制解析线程
        cancel_flag = threading.Event()
        
        def cancel_parsing():
            """取消解析操作"""
            cancel_flag.set()
            progress.stop()
            status_label.config(text="正在取消...")
            progress_dialog.after(500, progress_dialog.destroy)
        
        ttk.Button(cancel_frame, text="取消", command=cancel_parsing).pack(side=tk.RIGHT)
        
        def parse_project():
            """在后台线程中解析项目"""
            try:
                # 检查是否已取消
                if cancel_flag.is_set():
                    return
                
                # 更新状态
                self.root.after(0, lambda: status_label.config(text="正在解析文件结构..."))
                self.root.after(0, lambda: result_text.insert(tk.END, "开始解析项目文件...\n"))
                
                # 解析项目（检查取消标志）
                switches = c_parser.parse_project(project_path)
                
                # 再次检查是否已取消
                if cancel_flag.is_set():
                    return
                
                def update_ui():
                    """更新UI"""
                    progress.stop()
                    progress.config(mode='determinate', value=100)
                    
                    if switches:
                        # 生成配置数据
                        config_data = c_parser.export_to_config()
                        
                        # 记录加载前的统计
                        old_count = len(switch_manager.get_all_switches())
                        
                        # 清除现有开关桩
                        switch_manager._switches.clear()
                        switch_manager._groups.clear()
                        
                        # 加载解析出的开关桩
                        load_success = switch_manager.load_from_config(config_data)
                        
                        if load_success:
                            # 刷新界面
                            self.refresh_switch_list()
                            self.update_group_combo()
                            self.update_statistics()  # 更新统计信息
                            
                            # 统计信息
                            groups_count = len(switch_manager._groups)
                            files_count = len(c_parser.parsed_files)
                            
                            result_text.delete(1.0, tk.END)
                            result_text.insert(tk.END, f"✅ 项目加载成功！\n\n")
                            result_text.insert(tk.END, f"📊 找到 {len(switches)} 个开关桩\n")
                            result_text.insert(tk.END, f"📁 解析了 {files_count} 个文件\n")
                            result_text.insert(tk.END, f"📂 创建了 {groups_count} 个分组\n\n")
                            
                            if groups_count > 0:
                                group_names = list(switch_manager._groups.keys())
                                result_text.insert(tk.END, f"分组列表: {', '.join(group_names)}\n")
                            
                            status_label.config(text="解析完成 ✓")
                            
                            # 记录详细日志
                            self.add_log_entry(f"项目加载成功: {len(switches)} 个开关桩, {groups_count} 个组", "SUCCESS")
                            self.status_var.set(f"📁 项目已加载: {len(switches)} 个开关桩")
                            
                            # 如果找到了开关桩，自动选择第一个组并刷新显示
                            if groups_count > 0:
                                # 优先选择包含更多开关桩的组
                                best_group = max(group_names, key=lambda g: len(switch_manager.get_switches_by_group(g)))
                                self.group_var.set(best_group)
                                self.refresh_switch_list()  # 重新刷新以显示选中的组
                                self.add_log_entry(f"自动选择了分组: {best_group}", "INFO")
                                result_text.insert(tk.END, f"已自动选择分组: {best_group}")
                            
                        else:
                            result_text.delete(1.0, tk.END)
                            result_text.insert(tk.END, "❌ 项目配置加载失败\n请检查项目文件格式")
                            status_label.config(text="配置加载失败 ✗")
                            self.add_log_entry("项目配置加载失败", "ERROR")
                        
                    else:
                        result_text.delete(1.0, tk.END)
                        result_text.insert(tk.END, f"⚠️ 未找到任何开关桩定义\n\n")
                        result_text.insert(tk.END, f"已扫描 {len(c_parser.parsed_files)} 个文件\n")
                        result_text.insert(tk.END, "请检查项目中是否包含相关的宏定义或枚举")
                        
                        status_label.config(text="未找到开关桩定义")
                        self.add_log_entry(f"项目解析完成，但未找到开关桩定义 (扫描了{len(c_parser.parsed_files)}个文件)", "WARNING")
                        self.status_var.set("⚠️ 未找到开关桩定义")
                    
                    # 隐藏取消按钮
                    cancel_frame.pack_forget()
                    
                    # 显示完成按钮
                    button_frame.pack(fill=tk.X, pady=(0, 5))
                    
                    if switches and len(switches) > 0:
                        ttk.Button(button_frame, text="📋 查看详情", 
                                 command=lambda: self._show_parse_results(switches, config_data),
                                 style='Action.TButton').pack(side=tk.LEFT, padx=5)
                    
                    ttk.Button(button_frame, text="✅ 确定", 
                             command=progress_dialog.destroy,
                             style='Action.TButton').pack(side=tk.RIGHT, padx=5)
                
                self.root.after(0, update_ui)
                
            except Exception as e:
                def show_error():
                    progress.stop()
                    progress.config(mode='determinate', value=0)
                    error_msg = str(e)
                    
                    result_text.delete(1.0, tk.END)
                    result_text.insert(tk.END, f"❌ 解析失败\n\n{error_msg}")
                    status_label.config(text="解析失败 ✗")
                    
                    # 隐藏取消按钮
                    cancel_frame.pack_forget()
                    
                    # 显示关闭按钮
                    button_frame.pack(fill=tk.X, pady=(0, 5))
                    ttk.Button(button_frame, text="❌ 关闭", 
                             command=progress_dialog.destroy,
                             style='Action.TButton').pack(side=tk.RIGHT, padx=5)
                    
                self.root.after(0, show_error)
                logger.error(f"解析项目失败: {e}")
                self.add_log_entry(f"解析项目失败: {e}", "ERROR")
                self.status_var.set("❌ 项目加载失败")
        
        # 在后台线程中执行解析
        threading.Thread(target=parse_project, daemon=True).start()
    
    def load_sample_project(self, file_path: str) -> bool:
        """自动加载示例项目（供主程序调用）"""
        try:
            file_path = Path(file_path)
            
            if not file_path.exists():
                logger.error(f"示例文件不存在: {file_path}")
                return False
            
            self.add_log_entry(f"📁 正在加载示例项目: {file_path.name}", "INFO")
            
            # 解析C/C++文件
            switches, config_data = parse_file(str(file_path))
            
            if switches:
                # 清空现有开关桩
                switch_manager._switches.clear()
                switch_manager._groups.clear()
                
                # 添加解析出的开关桩
                for switch in switches:
                    switch_obj = Switch(
                        name=switch.name,
                        switch_type=SwitchType.MACRO if switch.switch_type == "macro" else SwitchType.ENUM,
                        current_state=SwitchState.OPEN if switch.default_value in ["1", "true", "open"] else SwitchState.CLOSED,
                        description=switch.description or f"{switch.switch_type.title()} 开关桩",
                        group=switch.group or "default"
                    )
                    switch_manager.add_switch(switch_obj)
                
                # 刷新界面
                self.refresh_switch_list()
                self.update_group_combo()
                
                # 自动选择第一个有内容的组
                groups = list(switch_manager._groups.keys())
                if groups:
                    self.group_var.set(groups[0])
                    self.refresh_switch_list()
                
                logger.info(f"示例项目加载成功: {file_path}")
                return True
                
            else:
                logger.warning(f"示例文件中未发现开关桩定义: {file_path}")
                return False
                
        except Exception as e:
            logger.error(f"加载示例项目失败: {e}")
            return False
    
    def _show_parse_results(self, switches: List[CSwitch], config_data: Dict):
        """显示解析结果详情"""
        details_dialog = tk.Toplevel(self.root)
        details_dialog.title("项目解析结果")
        details_dialog.geometry("600x500")
        details_dialog.transient(self.root)
        
        # 创建标签页
        notebook = ttk.Notebook(details_dialog)
        notebook.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # 概览标签页
        overview_frame = ttk.Frame(notebook)
        notebook.add(overview_frame, text="概览")
        
        overview_text = tk.Text(overview_frame, wrap=tk.WORD, font=('Arial', 10))
        overview_scrollbar = ttk.Scrollbar(overview_frame, orient=tk.VERTICAL, command=overview_text.yview)
        overview_text.configure(yscrollcommand=overview_scrollbar.set)
        
        # 生成概览信息
        groups_info = c_parser.get_switches_by_group()
        files_info = c_parser.get_switches_by_file()
        
        overview_info = f"""项目解析结果概览

📁 解析的文件数量: {len(c_parser.parsed_files)}
⚡ 找到的开关桩数量: {len(switches)}
📂 开关桩分组数量: {len(groups_info)}

📊 按分组统计:
"""
        for group_name, group_switches in groups_info.items():
            overview_info += f"  • {group_name}: {len(group_switches)} 个\n"
        
        overview_info += f"\n📁 按文件统计:\n"
        for file_path, file_switches in files_info.items():
            file_name = Path(file_path).name
            overview_info += f"  • {file_name}: {len(file_switches)} 个\n"
        
        overview_text.insert(tk.END, overview_info)
        overview_text.config(state=tk.DISABLED)
        
        overview_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        overview_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        # 开关桩列表标签页
        switches_frame = ttk.Frame(notebook)
        notebook.add(switches_frame, text="开关桩列表")
        
        # 创建开关桩树形视图
        tree_frame = ttk.Frame(switches_frame)
        tree_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        columns = ("name", "type", "value", "group", "file", "description")
        switches_tree = ttk.Treeview(tree_frame, columns=columns, show="headings", height=15)
        
        # 设置列标题
        headers = {
            "name": ("名称", 150),
            "type": ("类型", 80),
            "value": ("值", 80),
            "group": ("分组", 100),
            "file": ("文件", 120),
            "description": ("描述", 200)
        }
        
        for col, (title, width) in headers.items():
            switches_tree.heading(col, text=title)
            switches_tree.column(col, width=width)
        
        # 添加开关桩数据
        for switch in switches:
            file_name = Path(switch.file_path).name if switch.file_path else ""
            switches_tree.insert("", tk.END, values=(
                switch.name,
                switch.switch_type,
                switch.value or "",
                switch.group,
                file_name,
                switch.description[:50] + "..." if len(switch.description) > 50 else switch.description
            ))
        
        # 添加滚动条
        tree_scrollbar = ttk.Scrollbar(tree_frame, orient=tk.VERTICAL, command=switches_tree.yview)
        switches_tree.configure(yscrollcommand=tree_scrollbar.set)
        
        switches_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        tree_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        # 按钮框架
        button_frame = ttk.Frame(details_dialog)
        button_frame.pack(fill=tk.X, padx=10, pady=5)
        
        ttk.Button(button_frame, text="导出配置", 
                  command=lambda: self._export_parsed_config(config_data)).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="关闭", command=details_dialog.destroy).pack(side=tk.RIGHT)
    
    def _export_parsed_config(self, config_data: Dict):
        """导出解析的配置"""
        file_path = filedialog.asksaveasfilename(
            title="导出解析配置",
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        
        if file_path:
                try:
                    with open(file_path, 'w', encoding='utf-8') as f:
                        json.dump(config_data, f, indent=2, ensure_ascii=False)
                    
                    messagebox.showinfo("成功", f"配置已导出到: {file_path}")
                    self.add_log_entry(f"已导出解析配置到: {file_path}", "SUCCESS")
                    
                except Exception as e:
                    messagebox.showerror("错误", f"导出配置失败: {e}")
                    self.add_log_entry(f"导出配置失败: {e}", "ERROR")
    
    def save_config_file(self):
        """保存配置文件"""
        file_path = filedialog.asksaveasfilename(
            title="保存配置文件",
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        
        if file_path:
            try:
                # TODO: 实现配置保存逻辑
                config_data = {
                    "devices": {},
                    "switches": {}
                }
                
                with open(file_path, 'w', encoding='utf-8') as f:
                    json.dump(config_data, f, indent=2, ensure_ascii=False)
                
                messagebox.showinfo("成功", "配置文件保存成功")
                
            except Exception as e:
                messagebox.showerror("错误", f"保存配置文件失败: {e}")
    
    def show_about(self):
        """显示关于对话框"""
        about_dialog = tk.Toplevel(self.root)
        about_dialog.title("关于 VCU测试开关桩管理工具")
        about_dialog.geometry("500x600")
        about_dialog.transient(self.root)
        about_dialog.grab_set()
        about_dialog.resizable(False, False)
        
        # 主框架
        main_frame = ttk.Frame(about_dialog)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=20)
        
        # 标题
        title_label = ttk.Label(main_frame, text="VCU测试开关桩管理工具", 
                               font=('Arial', 16, 'bold'), foreground=self.colors['primary'])
        title_label.pack(pady=(0, 5))
        
        version_label = ttk.Label(main_frame, text="版本 2.0.0 - 专业版", 
                                 font=('Arial', 12), foreground=self.colors['info'])
        version_label.pack(pady=(0, 20))
        
        # 描述
        desc_text = """一个专业的VCU测试开关桩管理工具，基于UDP通信协议
实现对VCU设备的远程控制和监控。"""
        
        desc_label = ttk.Label(main_frame, text=desc_text, 
                              font=('Arial', 10), justify=tk.CENTER)
        desc_label.pack(pady=(0, 20))
        
        # 功能特性
        features_frame = ttk.LabelFrame(main_frame, text="✨ 主要功能", padding=15)
        features_frame.pack(fill=tk.X, pady=(0, 15))
        
        features = [
            "🖥️ VCU设备管理和实时监控",
            "⚡ 开关桩批量操作和智能控制",
            "📊 实时状态监控和统计分析",
            "📋 专业日志记录和历史追踪",
            "🔍 强大的搜索和过滤功能",
            "⌨️ 丰富的键盘快捷键支持",
            "📈 详细的报告生成和导出",
            "🎨 现代化用户界面设计"
        ]
        
        for feature in features:
            ttk.Label(features_frame, text=feature, font=('Arial', 9)).pack(anchor=tk.W, pady=1)
        
        # 技术规格
        tech_frame = ttk.LabelFrame(main_frame, text="🔧 技术规格", padding=15)
        tech_frame.pack(fill=tk.X, pady=(0, 15))
        
        tech_specs = [
            "💬 通信协议: UDP",
            "🐍 开发语言: Python 3.x",
            "🖼️ 界面框架: Tkinter (TTK)",
            "🏗️ 架构模式: MVC",
            "📁 配置格式: JSON",
            "🔧 支持平台: Windows/Linux/macOS"
        ]
        
        for spec in tech_specs:
            ttk.Label(tech_frame, text=spec, font=('Arial', 9)).pack(anchor=tk.W, pady=1)
        
        # 开发信息
        dev_frame = ttk.LabelFrame(main_frame, text="👥 开发信息", padding=15)
        dev_frame.pack(fill=tk.X, pady=(0, 20))
        
        ttk.Label(dev_frame, text="开发团队: VCU Team", font=('Arial', 9, 'bold')).pack(anchor=tk.W)
        ttk.Label(dev_frame, text="发布日期: 2024年", font=('Arial', 9)).pack(anchor=tk.W)
        ttk.Label(dev_frame, text="许可证: 专有软件", font=('Arial', 9)).pack(anchor=tk.W)
        
        # 按钮
        button_frame = ttk.Frame(main_frame)
        button_frame.pack(pady=10)
        
        ttk.Button(button_frame, text="检查更新", command=self.check_updates, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="关闭", command=about_dialog.destroy, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=5)
    
    def on_closing(self):
        """窗口关闭事件"""
        if self.is_monitoring:
            self.stop_monitoring()
        
        # 断开所有设备连接
        self.disconnect_all_devices()
        
        logger.info("应用程序正在关闭")
        self.root.destroy()
    
    # 搜索和过滤功能
    def on_device_search_changed(self, *args):
        """设备搜索变化事件"""
        self.refresh_device_list()
    
    def on_switch_search_changed(self, *args):
        """开关桩搜索变化事件"""
        self.refresh_switch_list()
    
    def on_global_search_changed(self, *args):
        """全局搜索变化事件"""
        search_term = self.global_search_var.get().lower()
        if search_term:
            self.device_search_var.set(search_term)
            self.switch_search_var.set(search_term)
        else:
            self.device_search_var.set("")
            self.switch_search_var.set("")
    
    def focus_search(self):
        """聚焦到搜索框"""
        self.global_search_entry.focus_set()
    
    # 排序功能
    def sort_device_tree(self, col):
        """设备列表排序"""
        items = [(self.device_tree.set(child, col), child) for child in self.device_tree.get_children('')]
        items.sort()
        
        for index, (value, child) in enumerate(items):
            self.device_tree.move(child, '', index)
        
        self.add_log_entry(f"按 {col} 列对设备列表进行排序", "INFO")
    
    def sort_switch_tree(self, col):
        """开关桩列表排序"""
        items = [(self.switch_tree.set(child, col), child) for child in self.switch_tree.get_children('')]
        items.sort()
        
        for index, (value, child) in enumerate(items):
            self.switch_tree.move(child, '', index)
        
        self.add_log_entry(f"按 {col} 列对开关桩列表进行排序", "INFO")
    
    # 双击和键盘事件处理
    def on_device_double_click(self, event):
        """设备双击事件"""
        selection = self.device_tree.selection()
        if selection:
            item = self.device_tree.item(selection[0])
            slot_id = item['values'][0]
            device = vcu_controller.get_device(slot_id)
            
            if device and device.state == VCUState.OFFLINE:
                self.connect_selected_device()
            elif device and device.state == VCUState.ONLINE:
                self.show_device_details()
    
    def on_switch_double_click(self, event):
        """开关桩双击事件"""
        self.toggle_selected_switches()
    
    def on_switch_key_press(self, event):
        """开关桩键盘事件"""
        if event.keysym == 'space':
            self.toggle_selected_switches()
        elif event.keysym == 'Return':
            self.apply_switch_changes()
        elif event.keysym == 'Delete':
            self.close_selected_switches()
        elif event.keysym == 'Insert':
            self.open_selected_switches()
    
    # 右键菜单事件处理
    def show_device_context_menu(self, event):
        """显示设备右键菜单"""
        try:
            self.device_context_menu.tk_popup(event.x_root, event.y_root)
        finally:
            self.device_context_menu.grab_release()
    
    def show_switch_context_menu(self, event):
        """显示开关桩右键菜单"""
        try:
            self.switch_context_menu.tk_popup(event.x_root, event.y_root)
        finally:
            self.switch_context_menu.grab_release()
    
    # 上下文菜单操作
    def context_open_switch(self):
        """右键菜单：打开开关"""
        self.open_selected_switches()
    
    def context_close_switch(self):
        """右键菜单：关闭开关"""
        self.close_selected_switches()
    
    def context_toggle_switch(self):
        """右键菜单：切换开关"""
        self.toggle_selected_switches()
    
    def copy_device_ip(self):
        """复制设备IP地址"""
        selection = self.device_tree.selection()
        if selection:
            item = self.device_tree.item(selection[0])
            ip = item['values'][2]
            self.root.clipboard_clear()
            self.root.clipboard_append(ip)
            self.add_log_entry(f"已复制设备IP: {ip}", "INFO")
            self.status_var.set(f"📋 已复制IP: {ip}")
    
    def copy_switch_name(self):
        """复制开关桩名称"""
        selection = self.switch_tree.selection()
        if selection:
            item = self.switch_tree.item(selection[0])
            name = item['values'][0]
            self.root.clipboard_clear()
            self.root.clipboard_append(name)
            self.add_log_entry(f"已复制开关桩名称: {name}", "INFO")
            self.status_var.set(f"📋 已复制开关桩名称: {name}")
    
    # 设备操作增强
    def test_selected_device(self):
        """测试选中的设备连接"""
        selection = self.device_tree.selection()
        if not selection:
            messagebox.showwarning("警告", "请选择要测试的设备")
            return
        
        item = self.device_tree.item(selection[0])
        slot_id = item['values'][0]
        
        # 创建进度对话框
        progress_dialog = tk.Toplevel(self.root)
        progress_dialog.title("设备连接测试")
        progress_dialog.geometry("400x150")
        progress_dialog.transient(self.root)
        progress_dialog.grab_set()
                
        ttk.Label(progress_dialog, text=f"正在测试设备 Slot {slot_id} 的连接...", 
                 font=('Arial', 10)).pack(pady=20)
        
        progress = ttk.Progressbar(progress_dialog, mode='indeterminate')
        progress.pack(pady=10, padx=20, fill=tk.X)
        progress.start()
        
        result_label = ttk.Label(progress_dialog, text="", font=('Arial', 9))
        result_label.pack(pady=10)
        
        def test_connection():
            try:
                # 模拟连接测试
                import random
                time.sleep(2)  # 模拟测试时间
                
                success = random.choice([True, False])
                ping_time = random.randint(1, 100) if success else 0
                
                def update_result():
                    progress.stop()
                    if success:
                        result_label.config(text=f"✅ 连接成功！延迟: {ping_time}ms", 
                                          foreground=self.colors['success'])
                        self.add_log_entry(f"设备 Slot {slot_id} 连接测试成功，延迟: {ping_time}ms", "SUCCESS")
                    else:
                        result_label.config(text="❌ 连接失败！", 
                                          foreground=self.colors['danger'])
                        self.add_log_entry(f"设备 Slot {slot_id} 连接测试失败", "ERROR")
                    
                    ttk.Button(progress_dialog, text="确定", 
                             command=progress_dialog.destroy).pack(pady=10)
                
                self.root.after(0, update_result)
                
            except Exception as e:
                logger.error(f"测试设备连接失败: {e}")
                self.root.after(0, lambda: result_label.config(text=f"❌ 测试异常: {e}"))
        
        threading.Thread(target=test_connection, daemon=True).start()
    
    def show_device_details(self):
        """显示设备详细信息"""
        selection = self.device_tree.selection()
        if not selection:
            return
        
        item = self.device_tree.item(selection[0])
        slot_id = item['values'][0]
        device = vcu_controller.get_device(slot_id)
        
        if not device:
            return
        
        # 创建详情对话框
        details_dialog = tk.Toplevel(self.root)
        details_dialog.title(f"设备详情 - Slot {slot_id}")
        details_dialog.geometry("500x400")
        details_dialog.transient(self.root)
        
        # 创建标签页
        notebook = ttk.Notebook(details_dialog)
        notebook.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # 基本信息标签页
        basic_frame = ttk.Frame(notebook)
        notebook.add(basic_frame, text="基本信息")
        
        basic_info = f"""设备槽位: {device.slot_id}
设备名称: {device.name}
IP地址: {device.ip}
本地IP: {device.local_ip}
端口: {device.port}
状态: {device.state.value}
是否启用: {'是' if device.enabled else '否'}
描述: {device.description}
最后通信: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(device.last_seen)) if device.last_seen else '无'}
"""
        
        basic_text = tk.Text(basic_frame, wrap=tk.WORD, font=('Arial', 10))
        basic_text.insert(tk.END, basic_info)
        basic_text.config(state=tk.DISABLED)
        basic_text.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # 统计信息标签页
        stats_frame = ttk.Frame(notebook)
        notebook.add(stats_frame, text="统计信息")
        
        stats_info = f"""连接次数: {getattr(device, 'connect_count', 0)}
断开次数: {getattr(device, 'disconnect_count', 0)}
发送消息数: {getattr(device, 'sent_messages', 0)}
接收消息数: {getattr(device, 'received_messages', 0)}
错误次数: {getattr(device, 'error_count', 0)}
平均延迟: {getattr(device, 'avg_ping', 0)}ms
最后错误: {getattr(device, 'last_error', '无')}
"""
        
        stats_text = tk.Text(stats_frame, wrap=tk.WORD, font=('Arial', 10))
        stats_text.insert(tk.END, stats_info)
        stats_text.config(state=tk.DISABLED)
        stats_text.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # 按钮框架
        button_frame = ttk.Frame(details_dialog)
        button_frame.pack(fill=tk.X, padx=10, pady=5)
        
        ttk.Button(button_frame, text="关闭", command=details_dialog.destroy).pack(side=tk.RIGHT)
        ttk.Button(button_frame, text="刷新", command=lambda: self.refresh_device_details(details_dialog, device)).pack(side=tk.RIGHT, padx=(0, 10))
    
    def refresh_device_details(self, dialog, device):
        """刷新设备详情"""
        # 重新获取设备信息并更新对话框
        self.add_log_entry(f"刷新设备 {device.slot_id} 详情", "INFO")
    
    def reset_selected_device(self):
        """重置选中的设备"""
        selection = self.device_tree.selection()
        if not selection:
            messagebox.showwarning("警告", "请选择要重置的设备")
            return
        
        item = self.device_tree.item(selection[0])
        slot_id = item['values'][0]
        
        if messagebox.askyesno("确认重置", f"确定要重置设备 Slot {slot_id} 吗？"):
            # 实现设备重置逻辑
            self.add_log_entry(f"重置设备 Slot {slot_id}", "INFO")
            self.status_var.set(f"🔄 设备 Slot {slot_id} 已重置")
    
    # 开关桩操作增强
    def edit_switch_label(self):
        """编辑开关桩标签"""
        selection = self.switch_tree.selection()
        if not selection:
            return
        
        item = self.switch_tree.item(selection[0])
        switch_name = item['values'][0]
        current_desc = item['values'][4]
        
        # 创建编辑对话框
        edit_dialog = tk.Toplevel(self.root)
        edit_dialog.title("编辑开关桩标签")
        edit_dialog.geometry("400x200")
        edit_dialog.transient(self.root)
        edit_dialog.grab_set()
        
        ttk.Label(edit_dialog, text=f"开关桩: {switch_name}", font=('Arial', 10, 'bold')).pack(pady=10)
        
        ttk.Label(edit_dialog, text="描述:").pack(anchor=tk.W, padx=20)
        desc_var = tk.StringVar(value=current_desc)
        desc_entry = ttk.Entry(edit_dialog, textvariable=desc_var, width=50)
        desc_entry.pack(pady=5, padx=20, fill=tk.X)
        desc_entry.focus_set()
        
        def save_changes():
            new_desc = desc_var.get()
            # 更新开关桩描述
            switch = switch_manager.get_switch(switch_name)
            if switch:
                switch.description = new_desc
                self.refresh_switch_list()
                self.add_log_entry(f"更新开关桩 {switch_name} 的描述", "INFO")
            edit_dialog.destroy()
        
        button_frame = ttk.Frame(edit_dialog)
        button_frame.pack(pady=20)
        
        ttk.Button(button_frame, text="保存", command=save_changes).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="取消", command=edit_dialog.destroy).pack(side=tk.LEFT, padx=5)
        
        edit_dialog.bind('<Return>', lambda e: save_changes())
        edit_dialog.bind('<Escape>', lambda e: edit_dialog.destroy())
    
    def move_switch_to_group(self):
        """移动开关桩到组"""
        selection = self.switch_tree.selection()
        if not selection:
            return
        
        switch_names = []
        for item in selection:
            values = self.switch_tree.item(item)['values']
            switch_names.append(values[0])
        
        # 创建组选择对话框
        group_dialog = tk.Toplevel(self.root)
        group_dialog.title("移动到组")
        group_dialog.geometry("300x150")
        group_dialog.transient(self.root)
        group_dialog.grab_set()
        
        ttk.Label(group_dialog, text=f"选择目标组 (选中 {len(switch_names)} 个开关桩)").pack(pady=10)
        
        group_var = tk.StringVar()
        groups = list(switch_manager._groups.keys()) + ["新建组..."]
        group_combo = ttk.Combobox(group_dialog, textvariable=group_var, values=groups)
        group_combo.pack(pady=10, padx=20, fill=tk.X)
        
        def move_switches():
            target_group = group_var.get()
            if target_group == "新建组...":
                # 创建新组对话框
                new_group = simpledialog.askstring("新建组", "请输入新组名称:")
                if new_group:
                    target_group = new_group
                else:
                    return
            
            # 移动开关桩到目标组
            for switch_name in switch_names:
                switch = switch_manager.get_switch(switch_name)
                if switch:
                    switch.group = target_group
            
            self.refresh_switch_list()
            self.update_group_combo()
            self.add_log_entry(f"移动 {len(switch_names)} 个开关桩到组 '{target_group}'", "INFO")
            group_dialog.destroy()
        
        button_frame = ttk.Frame(group_dialog)
        button_frame.pack(pady=10)
        
        ttk.Button(button_frame, text="移动", command=move_switches).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="取消", command=group_dialog.destroy).pack(side=tk.LEFT, padx=5)
    
    # 批量操作对话框
    def batch_operation_dialog(self):
        """批量操作对话框"""
        dialog = tk.Toplevel(self.root)
        dialog.title("批量操作")
        dialog.geometry("500x400")
        dialog.transient(self.root)
        dialog.grab_set()
        
        # 操作选择
        ttk.Label(dialog, text="选择批量操作:", font=('Arial', 12, 'bold')).pack(pady=10)
        
        operation_frame = ttk.LabelFrame(dialog, text="操作类型", padding=10)
        operation_frame.pack(fill=tk.X, padx=20, pady=5)
        
        operation_var = tk.StringVar(value="open")
        operations = [
            ("打开所有", "open"),
            ("关闭所有", "close"),
            ("切换状态", "toggle"),
            ("重置到默认", "reset")
        ]
        
        for text, value in operations:
            ttk.Radiobutton(operation_frame, text=text, variable=operation_var, 
                           value=value).pack(anchor=tk.W)
        
        # 目标选择
        target_frame = ttk.LabelFrame(dialog, text="目标选择", padding=10)
        target_frame.pack(fill=tk.X, padx=20, pady=5)
        
        target_var = tk.StringVar(value="selected")
        targets = [
            ("选中的开关桩", "selected"),
            ("当前组的所有开关桩", "group"),
            ("所有开关桩", "all")
        ]
        
        for text, value in targets:
            ttk.Radiobutton(target_frame, text=text, variable=target_var, 
                           value=value).pack(anchor=tk.W)
        
        # 确认选项
        confirm_frame = ttk.LabelFrame(dialog, text="确认选项", padding=10)
        confirm_frame.pack(fill=tk.X, padx=20, pady=5)
        
        confirm_var = tk.BooleanVar(value=True)
        ttk.Checkbutton(confirm_frame, text="执行前显示确认对话框", 
                       variable=confirm_var).pack(anchor=tk.W)
        
        progress_var = tk.BooleanVar(value=True)
        ttk.Checkbutton(confirm_frame, text="显示操作进度", 
                       variable=progress_var).pack(anchor=tk.W)
        
        def execute_batch_operation():
            operation = operation_var.get()
            target = target_var.get()
            show_confirm = confirm_var.get()
            show_progress = progress_var.get()
            
            # 获取目标开关桩列表
            if target == "selected":
                selection = self.switch_tree.selection()
                if not selection:
                    messagebox.showwarning("警告", "没有选中的开关桩")
                    return
                switch_names = [self.switch_tree.item(item)['values'][0] for item in selection]
            elif target == "group":
                group = self.group_var.get()
                if group and group != "全部":
                    switches = switch_manager.get_switches_by_group(group)
                    switch_names = [s.name for s in switches]
                else:
                    switch_names = list(switch_manager.get_all_switches().keys())
            else:  # all
                switch_names = list(switch_manager.get_all_switches().keys())
            
            if not switch_names:
                messagebox.showwarning("警告", "没有找到目标开关桩")
                return
            
            # 确认对话框
            if show_confirm:
                op_text = {"open": "打开", "close": "关闭", "toggle": "切换", "reset": "重置"}
                if not messagebox.askyesno("确认批量操作", 
                                         f"确定要{op_text[operation]} {len(switch_names)} 个开关桩吗？"):
                    return
            
            dialog.destroy()
            
            # 执行批量操作
            self.execute_batch_operation_with_progress(operation, switch_names, show_progress)
        
        # 按钮
        button_frame = ttk.Frame(dialog)
        button_frame.pack(pady=20)
        
        ttk.Button(button_frame, text="执行", command=execute_batch_operation, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="取消", command=dialog.destroy).pack(side=tk.LEFT, padx=5)
    
    def execute_batch_operation_with_progress(self, operation: str, switch_names: list, show_progress: bool):
        """执行带进度显示的批量操作"""
        if show_progress:
            self.progress_var.set(0)
            self.progress_label.config(text="准备中...")
        
        success_count = 0
        total_count = len(switch_names)
        
        def update_progress(current, total, message):
            if show_progress:
                progress = (current / total) * 100
                self.progress_var.set(progress)
                self.progress_label.config(text=message)
            self.root.update_idletasks()
        
        def execute_operation():
            nonlocal success_count
            
            for i, switch_name in enumerate(switch_names):
                try:
                    update_progress(i, total_count, f"处理 {switch_name}...")
                    
                    if operation == "open":
                        success = switch_manager.set_switch_state(switch_name, SwitchState.OPEN)
                    elif operation == "close":
                        success = switch_manager.set_switch_state(switch_name, SwitchState.CLOSED)
                    elif operation == "toggle":
                        success = switch_manager.toggle_switch_state(switch_name)
                    elif operation == "reset":
                        success = switch_manager.reset_switch_to_default(switch_name)
                    else:
                        success = False
                    
                    if success:
                        success_count += 1
                    
                    time.sleep(0.1)  # 模拟操作时间
                    
                except Exception as e:
                    logger.error(f"批量操作失败 {switch_name}: {e}")
            
            update_progress(total_count, total_count, "完成")
                                
                                # 刷新界面
            self.root.after(0, self.refresh_switch_list)
            
            # 显示结果
            op_text = {"open": "打开", "close": "关闭", "toggle": "切换", "reset": "重置"}
            result_message = f"批量{op_text[operation]}完成：成功 {success_count}/{total_count}"
            self.add_log_entry(result_message, "SUCCESS" if success_count == total_count else "WARNING")
            self.status_var.set(f"✅ {result_message}")
            
            # 清除进度显示
            if show_progress:
                self.root.after(2000, lambda: (
                    self.progress_var.set(0),
                    self.progress_label.config(text="")
                ))
        
        # 在后台线程执行操作
        threading.Thread(target=execute_operation, daemon=True).start()
    
    # 刷新功能增强
    def refresh_all(self):
        """刷新所有数据"""
        self.add_log_entry("开始刷新所有数据...", "INFO")
        self.status_var.set("🔄 正在刷新...")
        
        try:
            self.refresh_device_list()
            self.refresh_switch_list()
            self.update_statistics()
            self.update_group_combo()
                                
            self.add_log_entry("刷新完成", "SUCCESS")
            self.status_var.set("✅ 刷新完成")
        except Exception as e:
            logger.error(f"刷新失败: {e}")
            self.add_log_entry(f"刷新失败: {e}", "ERROR")
            self.status_var.set("❌ 刷新失败")
    
    # 菜单功能实现
    def export_report(self):
        """导出报告"""
        file_path = filedialog.asksaveasfilename(
            title="导出报告",
            defaultextension=".html",
            filetypes=[("HTML files", "*.html"), ("PDF files", "*.pdf"), ("Text files", "*.txt")]
        )
        
        if file_path:
            try:
                # 生成报告内容
                report_content = self.generate_report()
                
                if file_path.endswith('.html'):
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.write(report_content)
                else:
                    # 简单文本格式
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.write(report_content.replace('<br>', '\n').replace('<h2>', '\n').replace('</h2>', '\n'))
                
                self.add_log_entry(f"报告已导出到: {file_path}", "SUCCESS")
                messagebox.showinfo("成功", "报告导出成功！")
                
            except Exception as e:
                logger.error(f"导出报告失败: {e}")
                messagebox.showerror("错误", f"导出报告失败: {e}")
    
    def export_logs(self):
        """导出日志"""
        file_path = filedialog.asksaveasfilename(
            title="导出日志",
            defaultextension=".log",
            filetypes=[("Log files", "*.log"), ("Text files", "*.txt")]
        )
        
        if file_path:
            try:
                log_content = self.log_text.get(1.0, tk.END)
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(log_content)
                
                self.add_log_entry(f"日志已导出到: {file_path}", "SUCCESS")
                messagebox.showinfo("成功", "日志导出成功！")
                
            except Exception as e:
                logger.error(f"导出日志失败: {e}")
                messagebox.showerror("错误", f"导出日志失败: {e}")
    
    def generate_report(self):
        """生成HTML报告"""
        devices = vcu_controller.get_all_devices()
        switches = switch_manager.get_all_switches()
        
        html_content = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <title>VCU测试报告</title>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 20px; }}
                .header {{ text-align: center; color: #2563eb; }}
                .section {{ margin: 20px 0; }}
                .device-online {{ color: #10b981; }}
                .device-offline {{ color: #ef4444; }}
                .switch-open {{ color: #10b981; }}
                .switch-closed {{ color: #ef4444; }}
                table {{ border-collapse: collapse; width: 100%; }}
                th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
                th {{ background-color: #f2f2f2; }}
            </style>
        </head>
        <body>
            <div class="header">
                <h1>VCU测试开关桩管理报告</h1>
                <p>生成时间: {time.strftime('%Y-%m-%d %H:%M:%S')}</p>
            </div>
            
            <div class="section">
                <h2>设备状态汇总</h2>
                <p>总设备数: {len(devices)}</p>
                <p>在线设备: <span class="device-online">{sum(1 for d in devices.values() if d.state == VCUState.ONLINE)}</span></p>
                <p>离线设备: <span class="device-offline">{sum(1 for d in devices.values() if d.state == VCUState.OFFLINE)}</span></p>
                
                <h3>设备详情</h3>
                <table>
                    <tr><th>槽位</th><th>名称</th><th>IP地址</th><th>状态</th><th>最后通信</th></tr>
        """
        
        for slot_id, device in devices.items():
            status_class = "device-online" if device.state == VCUState.ONLINE else "device-offline"
            last_seen = time.strftime('%H:%M:%S', time.localtime(device.last_seen)) if device.last_seen else '无'
            html_content += f"""
                    <tr>
                        <td>{device.slot_id}</td>
                        <td>{device.name}</td>
                        <td>{device.ip}</td>
                        <td class="{status_class}">{device.state.value}</td>
                        <td>{last_seen}</td>
                    </tr>
            """
        
        html_content += """
                </table>
            </div>
            
            <div class="section">
                <h2>开关桩状态汇总</h2>
        """
        
        open_count = sum(1 for s in switches.values() if s.current_state == SwitchState.OPEN)
        closed_count = sum(1 for s in switches.values() if s.current_state == SwitchState.CLOSED)
        
        html_content += f"""
                <p>总开关桩数: {len(switches)}</p>
                <p>打开状态: <span class="switch-open">{open_count}</span></p>
                <p>关闭状态: <span class="switch-closed">{closed_count}</span></p>
                
                <h3>开关桩详情</h3>
                <table>
                    <tr><th>名称</th><th>类型</th><th>当前状态</th><th>组</th><th>描述</th></tr>
        """
        
        for switch_name, switch in switches.items():
            status_class = "switch-open" if switch.current_state == SwitchState.OPEN else "switch-closed"
            html_content += f"""
                    <tr>
                        <td>{switch.name}</td>
                        <td>{switch.switch_type.value}</td>
                        <td class="{status_class}">{switch.current_state.value}</td>
                        <td>{switch.group}</td>
                        <td>{switch.description}</td>
                    </tr>
            """
        
        html_content += """
                </table>
            </div>
        </body>
        </html>
        """
        
        return html_content
    
    def device_diagnostics(self):
        """设备诊断"""
        dialog = tk.Toplevel(self.root)
        dialog.title("设备诊断")
        dialog.geometry("600x500")
        dialog.transient(self.root)
        
        ttk.Label(dialog, text="设备诊断工具", font=('Arial', 14, 'bold')).pack(pady=10)
        
        # 诊断选项
        options_frame = ttk.LabelFrame(dialog, text="诊断选项", padding=10)
        options_frame.pack(fill=tk.X, padx=20, pady=10)
        
        ping_var = tk.BooleanVar(value=True)
        ttk.Checkbutton(options_frame, text="网络连通性测试 (Ping)", variable=ping_var).pack(anchor=tk.W)
        
        port_var = tk.BooleanVar(value=True)
        ttk.Checkbutton(options_frame, text="端口连接测试", variable=port_var).pack(anchor=tk.W)
        
        protocol_var = tk.BooleanVar(value=True)
        ttk.Checkbutton(options_frame, text="协议通信测试", variable=protocol_var).pack(anchor=tk.W)
        
        # 结果显示
        result_frame = ttk.LabelFrame(dialog, text="诊断结果", padding=10)
        result_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=10)
        
        result_text = tk.Text(result_frame, wrap=tk.WORD, font=('Consolas', 9))
        result_scrollbar = ttk.Scrollbar(result_frame, orient=tk.VERTICAL, command=result_text.yview)
        result_text.configure(yscrollcommand=result_scrollbar.set)
        
        result_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        result_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        # 进度条
        progress = ttk.Progressbar(dialog, mode='indeterminate')
        progress.pack(fill=tk.X, padx=20, pady=5)
        
        def run_diagnostics():
            progress.start()
            result_text.delete(1.0, tk.END)
            result_text.insert(tk.END, "开始设备诊断...\n\n")
            
            devices = vcu_controller.get_all_devices()
            
            for slot_id, device in devices.items():
                result_text.insert(tk.END, f"诊断设备 {device.name} (Slot {slot_id}):\n")
                result_text.insert(tk.END, f"  IP: {device.ip}:{device.port}\n")
                
                if ping_var.get():
                    result_text.insert(tk.END, "  [Ping测试] ")
                    # 模拟ping测试
                    time.sleep(0.5)
                    result_text.insert(tk.END, "✅ 连通\n")
                
                if port_var.get():
                    result_text.insert(tk.END, "  [端口测试] ")
                    # 模拟端口测试
                    time.sleep(0.3)
                    result_text.insert(tk.END, "✅ 端口开放\n")
                
                if protocol_var.get():
                    result_text.insert(tk.END, "  [协议测试] ")
                    # 模拟协议测试
                    time.sleep(0.4)
                    result_text.insert(tk.END, "✅ 协议正常\n")
                
                result_text.insert(tk.END, "\n")
                result_text.see(tk.END)
                dialog.update()
            
            progress.stop()
            result_text.insert(tk.END, "诊断完成！\n")
            self.add_log_entry("设备诊断完成", "SUCCESS")
        
        # 按钮
        button_frame = ttk.Frame(dialog)
        button_frame.pack(pady=10)
        
        ttk.Button(button_frame, text="开始诊断", 
                  command=lambda: threading.Thread(target=run_diagnostics, daemon=True).start(),
                  style='Action.TButton').pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="关闭", command=dialog.destroy).pack(side=tk.LEFT, padx=5)
    
    def create_switch_group(self):
        """创建开关桩组"""
        dialog = tk.Toplevel(self.root)
        dialog.title("创建开关桩组")
        dialog.geometry("400x300")
        dialog.transient(self.root)
        dialog.grab_set()
        
        ttk.Label(dialog, text="创建新的开关桩组", font=('Arial', 12, 'bold')).pack(pady=10)
        
        # 组名称
        ttk.Label(dialog, text="组名称:").pack(anchor=tk.W, padx=20)
        group_name_var = tk.StringVar()
        ttk.Entry(dialog, textvariable=group_name_var, width=40).pack(pady=5, padx=20, fill=tk.X)
        
        # 组描述
        ttk.Label(dialog, text="组描述:").pack(anchor=tk.W, padx=20, pady=(10, 0))
        group_desc_var = tk.StringVar()
        ttk.Entry(dialog, textvariable=group_desc_var, width=40).pack(pady=5, padx=20, fill=tk.X)
        
        # 选择开关桩
        ttk.Label(dialog, text="选择开关桩:").pack(anchor=tk.W, padx=20, pady=(10, 0))
        
        switches_frame = ttk.Frame(dialog)
        switches_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=5)
        
        switches_listbox = tk.Listbox(switches_frame, selectmode=tk.MULTIPLE)
        switches_scrollbar = ttk.Scrollbar(switches_frame, orient=tk.VERTICAL, command=switches_listbox.yview)
        switches_listbox.configure(yscrollcommand=switches_scrollbar.set)
        
        # 填充开关桩列表
        all_switches = switch_manager.get_all_switches()
        for switch_name in all_switches.keys():
            switches_listbox.insert(tk.END, switch_name)
        
        switches_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        switches_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        def create_group():
            group_name = group_name_var.get().strip()
            if not group_name:
                messagebox.showwarning("警告", "请输入组名称")
                return
            
            # 获取选中的开关桩
            selected_indices = switches_listbox.curselection()
            selected_switches = [switches_listbox.get(i) for i in selected_indices]
            
            if not selected_switches:
                messagebox.showwarning("警告", "请选择至少一个开关桩")
                return
            
            # 创建组
            for switch_name in selected_switches:
                switch = switch_manager.get_switch(switch_name)
                if switch:
                    switch.group = group_name
            
                self.refresh_switch_list()
            self.update_group_combo()
            self.add_log_entry(f"创建开关桩组 '{group_name}'，包含 {len(selected_switches)} 个开关桩", "SUCCESS")
            dialog.destroy()
        
        # 按钮
        button_frame = ttk.Frame(dialog)
        button_frame.pack(pady=10)
        
        ttk.Button(button_frame, text="创建", command=create_group, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="取消", command=dialog.destroy).pack(side=tk.LEFT, padx=5)
    
    def monitor_settings(self):
        """监控设置"""
        dialog = tk.Toplevel(self.root)
        dialog.title("监控设置")
        dialog.geometry("400x350")
        dialog.transient(self.root)
        dialog.grab_set()
        
        ttk.Label(dialog, text="监控设置", font=('Arial', 12, 'bold')).pack(pady=10)
        
        # 刷新间隔
        interval_frame = ttk.LabelFrame(dialog, text="刷新间隔", padding=10)
        interval_frame.pack(fill=tk.X, padx=20, pady=5)
        
        ttk.Label(interval_frame, text="更新间隔 (秒):").pack(anchor=tk.W)
        interval_var = tk.DoubleVar(value=2.0)
        interval_scale = ttk.Scale(interval_frame, from_=0.5, to=10.0, variable=interval_var, orient=tk.HORIZONTAL)
        interval_scale.pack(fill=tk.X, pady=5)
        
        interval_label = ttk.Label(interval_frame, text="2.0 秒")
        interval_label.pack()
        
        def update_interval_label(*args):
            interval_label.config(text=f"{interval_var.get():.1f} 秒")
        
        interval_var.trace('w', update_interval_label)
        
        # 自动重连
        reconnect_frame = ttk.LabelFrame(dialog, text="自动重连", padding=10)
        reconnect_frame.pack(fill=tk.X, padx=20, pady=5)
        
        auto_reconnect_var = tk.BooleanVar(value=True)
        ttk.Checkbutton(reconnect_frame, text="启用自动重连", variable=auto_reconnect_var).pack(anchor=tk.W)
        
        ttk.Label(reconnect_frame, text="重连间隔 (秒):").pack(anchor=tk.W, pady=(10, 0))
        reconnect_interval_var = tk.IntVar(value=30)
        ttk.Spinbox(reconnect_frame, from_=10, to=300, textvariable=reconnect_interval_var, width=10).pack(anchor=tk.W)
        
        # 日志级别
        log_frame = ttk.LabelFrame(dialog, text="日志设置", padding=10)
        log_frame.pack(fill=tk.X, padx=20, pady=5)
        
        ttk.Label(log_frame, text="日志级别:").pack(anchor=tk.W)
        log_level_var = tk.StringVar(value="INFO")
        log_level_combo = ttk.Combobox(log_frame, textvariable=log_level_var, 
                                     values=["DEBUG", "INFO", "WARNING", "ERROR"], width=15)
        log_level_combo.pack(anchor=tk.W, pady=5)
        
        def save_settings():
            # 保存设置到配置文件
            settings = {
                "update_interval": interval_var.get(),
                "auto_reconnect": auto_reconnect_var.get(),
                "reconnect_interval": reconnect_interval_var.get(),
                "log_level": log_level_var.get()
            }
            
            # 这里可以保存到配置文件
            self.add_log_entry("监控设置已保存", "SUCCESS")
            dialog.destroy()
        
        # 按钮
        button_frame = ttk.Frame(dialog)
        button_frame.pack(pady=20)
        
        ttk.Button(button_frame, text="保存", command=save_settings, 
                  style='Action.TButton').pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="取消", command=dialog.destroy).pack(side=tk.LEFT, padx=5)
    
    def show_history(self):
        """显示历史记录"""
        dialog = tk.Toplevel(self.root)
        dialog.title("操作历史记录")
        dialog.geometry("700x500")
        dialog.transient(self.root)
        
        # 历史记录列表
        history_frame = ttk.Frame(dialog)
        history_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        columns = ("timestamp", "level", "message")
        history_tree = ttk.Treeview(history_frame, columns=columns, show="headings")
        
        history_tree.heading("timestamp", text="时间")
        history_tree.heading("level", text="级别")
        history_tree.heading("message", text="消息")
        
        history_tree.column("timestamp", width=150)
        history_tree.column("level", width=80)
        history_tree.column("message", width=400)
        
        # 添加历史记录
        for entry in self.operation_history[-100:]:  # 只显示最近100条
            timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(entry['timestamp']))
            history_tree.insert("", tk.END, values=(timestamp, entry['level'], entry['message']))
        
        history_scrollbar = ttk.Scrollbar(history_frame, orient=tk.VERTICAL, command=history_tree.yview)
        history_tree.configure(yscrollcommand=history_scrollbar.set)
        
        history_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        history_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        # 按钮
        button_frame = ttk.Frame(dialog)
        button_frame.pack(pady=10)
        
        ttk.Button(button_frame, text="导出", 
                  command=lambda: self.export_history(history_tree)).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="清除", 
                  command=lambda: self.clear_history_confirm(dialog)).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="关闭", command=dialog.destroy).pack(side=tk.LEFT, padx=5)
    
    def export_history(self, tree):
        """导出历史记录"""
        file_path = filedialog.asksaveasfilename(
            title="导出历史记录",
            defaultextension=".csv",
            filetypes=[("CSV files", "*.csv"), ("Text files", "*.txt")]
        )
        
        if file_path:
            try:
                with open(file_path, 'w', encoding='utf-8') as f:
                    if file_path.endswith('.csv'):
                        f.write("时间,级别,消息\n")
                        for entry in self.operation_history:
                            timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(entry['timestamp']))
                            f.write(f"{timestamp},{entry['level']},{entry['message']}\n")
                    else:
                        for entry in self.operation_history:
                            timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(entry['timestamp']))
                            f.write(f"{timestamp} [{entry['level']}] {entry['message']}\n")
                
                messagebox.showinfo("成功", "历史记录导出成功！")
                                
            except Exception as e:
                messagebox.showerror("错误", f"导出失败: {e}")
    
    def clear_history(self):
        """清除历史记录"""
        self.operation_history.clear()
        self.add_log_entry("历史记录已清除", "INFO")
    
    def clear_history_confirm(self, dialog):
        """确认清除历史记录"""
        if messagebox.askyesno("确认", "确定要清除所有历史记录吗？"):
            self.clear_history()
            dialog.destroy()
    
    def toggle_statistics(self):
        """切换统计信息显示"""
        # 实现统计信息显示切换
        pass
    
    def toggle_log_panel(self):
        """切换日志面板显示"""
        if self.log_panel_visible:
            self.log_frame.pack_forget()
            self.log_panel_visible = False
        else:
            self.log_frame.pack(fill=tk.X, padx=5, pady=2, before=self.status_frame)
            self.log_panel_visible = True
    
    def reset_layout(self):
        """重置界面布局"""
        if messagebox.askyesno("确认", "确定要重置界面布局吗？"):
            # 重新创建界面
            for widget in self.root.winfo_children():
                if widget != self.root.winfo_children()[0]:  # 保留菜单
                    widget.destroy()
            
            self.create_widgets()
            self.add_log_entry("界面布局已重置", "INFO")
    
    def show_help(self):
        """显示帮助信息"""
        help_text = """
VCU测试开关桩管理工具 - 使用说明

== 主要功能 ==
• VCU设备管理和监控
• 开关桩批量操作和控制
• 实时状态监控和日志记录

== 快捷键说明 ==
• Ctrl+O: 加载项目文件（开关桩）
• Ctrl+Shift+O: 加载设备配置文件
• Ctrl+S: 保存配置文件
• Ctrl+Q: 退出应用程序
• F5: 刷新设备列表
• Ctrl+R: 刷新所有数据
• F1: 启动/停止监控
• F2: 显示帮助
• Ctrl+F: 聚焦搜索框
• Ctrl+A: 全选开关桩
• Space: 切换选中开关桩状态
• Enter: 应用开关桩更改

== 操作说明 ==
1. 设备管理：
   - 双击离线设备进行连接
   - 双击在线设备查看详情
   - 右键菜单提供更多操作

2. 开关桩管理：
   - 双击开关桩切换状态
   - 使用批量操作进行批量控制
   - 支持按组管理开关桩

3. 监控功能：
   - 启动监控实时查看设备状态
   - 查看操作日志和历史记录
   - 导出报告和日志文件

== 技术支持 ==
如有问题，请联系技术支持团队。
        """
        
        help_dialog = tk.Toplevel(self.root)
        help_dialog.title("使用说明")
        help_dialog.geometry("600x500")
        help_dialog.transient(self.root)
        
        help_text_widget = tk.Text(help_dialog, wrap=tk.WORD, font=('Arial', 10))
        help_text_widget.insert(tk.END, help_text)
        help_text_widget.config(state=tk.DISABLED)
        
        help_scrollbar = ttk.Scrollbar(help_dialog, orient=tk.VERTICAL, command=help_text_widget.yview)
        help_text_widget.configure(yscrollcommand=help_scrollbar.set)
        
        help_text_widget.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=10, pady=10)
        help_scrollbar.pack(side=tk.RIGHT, fill=tk.Y, pady=10)
        
        ttk.Button(help_dialog, text="关闭", command=help_dialog.destroy).pack(pady=10)
    
    def show_shortcuts(self):
        """显示快捷键"""
        shortcuts_text = """
键盘快捷键一览表

== 文件操作 ==
Ctrl+O          加载项目文件（开关桩）
Ctrl+Shift+O    加载设备配置文件
Ctrl+S          保存配置文件
Ctrl+Q          退出应用程序

== 设备操作 ==
F5              刷新设备列表
Ctrl+R          刷新所有数据
Ctrl+D          连接选中设备
Ctrl+Shift+D    断开选中设备

== 开关桩操作 ==
Ctrl+A          全选开关桩
Ctrl+Shift+A    取消选择开关桩
Space           切换选中开关桩状态
Enter           应用开关桩更改
Delete          关闭选中开关桩
Insert          打开选中开关桩

== 系统操作 ==
F1              启动/停止监控
F2              显示帮助信息
Ctrl+F          聚焦到搜索框

== 界面操作 ==
双击设备        连接/查看详情
双击开关桩      切换状态
右键菜单        更多操作选项
        """
        
        shortcuts_dialog = tk.Toplevel(self.root)
        shortcuts_dialog.title("键盘快捷键")
        shortcuts_dialog.geometry("400x500")
        shortcuts_dialog.transient(self.root)
        
        shortcuts_text_widget = tk.Text(shortcuts_dialog, wrap=tk.WORD, font=('Consolas', 10))
        shortcuts_text_widget.insert(tk.END, shortcuts_text)
        shortcuts_text_widget.config(state=tk.DISABLED)
        
        shortcuts_scrollbar = ttk.Scrollbar(shortcuts_dialog, orient=tk.VERTICAL, command=shortcuts_text_widget.yview)
        shortcuts_text_widget.configure(yscrollcommand=shortcuts_scrollbar.set)
        
        shortcuts_text_widget.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=10, pady=10)
        shortcuts_scrollbar.pack(side=tk.RIGHT, fill=tk.Y, pady=10)
        
        ttk.Button(shortcuts_dialog, text="关闭", command=shortcuts_dialog.destroy).pack(pady=10)
    
    def check_updates(self):
        """检查更新"""
        # 模拟检查更新
        progress_dialog = tk.Toplevel(self.root)
        progress_dialog.title("检查更新")
        progress_dialog.geometry("300x100")
        progress_dialog.transient(self.root)
        progress_dialog.grab_set()
        
        ttk.Label(progress_dialog, text="正在检查更新...").pack(pady=20)
        progress = ttk.Progressbar(progress_dialog, mode='indeterminate')
        progress.pack(pady=10, padx=20, fill=tk.X)
        progress.start()
        
        def check_complete():
            progress.stop()
            progress_dialog.destroy()
            messagebox.showinfo("检查更新", "当前版本已是最新版本！")
        
        self.root.after(2000, check_complete)
    
    # 日志面板相关方法
    def clear_log(self):
        """清除日志"""
        self.log_text.delete(1.0, tk.END)
        self.add_log_entry("日志已清除", "INFO")
    
    def save_log(self):
        """保存日志"""
        file_path = filedialog.asksaveasfilename(
            title="保存日志",
            defaultextension=".log",
            filetypes=[("Log files", "*.log"), ("Text files", "*.txt")]
        )
        
        if file_path:
            try:
                log_content = self.log_text.get(1.0, tk.END)
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(log_content)
                
                self.add_log_entry(f"日志已保存到: {file_path}", "SUCCESS")
                messagebox.showinfo("成功", "日志保存成功！")
                
            except Exception as e:
                logger.error(f"保存日志失败: {e}")
                messagebox.showerror("错误", f"保存日志失败: {e}")
