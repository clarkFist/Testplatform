#!/usr/bin/env python3
"""
VCU测试开关桩管理工具 - 快速修复工具

修复GUI启动问题和Unicode编码问题
"""

import re
from pathlib import Path
import sys


class QuickFixer:
    """快速修复工具"""
    
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.fixes_applied = []
    
    def fix_all(self):
        """执行所有修复"""
        print("🔧 VCU测试开关桩管理工具 - 快速修复工具")
        print("=" * 50)
        
        self.fix_unicode_issues()
        self.fix_keyboard_shortcuts()
        self.fix_config_validation()
        
        self.print_summary()
    
    def fix_unicode_issues(self):
        """修复Unicode编码问题"""
        print("🔤 修复Unicode编码问题...")
        
        main_window_path = self.project_root / "gui" / "main_window.py"
        if not main_window_path.exists():
            print("   ❌ 主窗口文件不存在")
            return
        
        with open(main_window_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 替换所有可能有问题的emoji字符
        emoji_replacements = {
            '🟢': '[在线]',
            '🔴': '[离线]', 
            '🟡': '[连接中]',
            '⚠️': '[警告]',
            '📱': '[设备]',
            '🔗': '[连接]',
            '✅': '[成功]',
            '🔄': '[刷新]',
            '📊': '[统计]',
            '⚡': '[开关桩]',
            '▶️': '[启动]',
            '⏸️': '[停止]',
            '🕐': '[时间]',
            '📁': '[文件]',
            '🔍': '[搜索]',
            '🖥️': '[设备]',
            '🔧': '[测试]',
            '☑️': '[全选]',
            '☐': '[取消]',
            '📂': '[组]',
            '📋': '[日志]',
            '📡': '[连接]',
            '🏠': '[主页]'
        }
        
        original_content = content
        
        for emoji, replacement in emoji_replacements.items():
            content = content.replace(emoji, replacement)
        
        # 修复时间格式字符串中的emoji
        content = re.sub(r'time\.strftime\("🕐[^"]*"', 'time.strftime("%Y-%m-%d %H:%M:%S"', content)
        
        if content != original_content:
            with open(main_window_path, 'w', encoding='utf-8') as f:
                f.write(content)
            self.fixes_applied.append("修复了GUI中的emoji字符编码问题")
            print("   ✅ 已替换所有emoji字符为兼容文本")
        else:
            print("   ✅ 未发现emoji编码问题")
    
    def fix_keyboard_shortcuts(self):
        """修复键盘快捷键问题"""
        print("⌨️ 修复键盘快捷键...")
        
        main_window_path = self.project_root / "gui" / "main_window.py"
        if not main_window_path.exists():
            return
        
        with open(main_window_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 修复键盘事件绑定
        replacements = [
            ('<KeyPress-space>', '<space>'),
            ('<Space>', '<space>'),
            ('lambda e: self.toggle_selected_switches()', 'lambda e: self.toggle_selected_switches() if self.switch_tree.focus() else None')
        ]
        
        original_content = content
        
        for old, new in replacements:
            content = content.replace(old, new)
        
        if content != original_content:
            with open(main_window_path, 'w', encoding='utf-8') as f:
                f.write(content)
            self.fixes_applied.append("修复了键盘快捷键绑定问题")
            print("   ✅ 已修复键盘快捷键绑定")
        else:
            print("   ✅ 键盘快捷键正常")
    
    def fix_config_validation(self):
        """修复配置验证问题"""
        print("⚙️ 修复配置验证...")
        
        config_path = self.project_root / "config" / "default.json"
        if not config_path.exists():
            print("   ❌ 配置文件不存在")
            return
        
        # 检查配置文件是否包含所有必需项
        import json
        try:
            with open(config_path, 'r', encoding='utf-8') as f:
                config = json.load(f)
            
            # 确保server.host存在
            if 'server' not in config:
                config['server'] = {}
            
            if 'host' not in config['server']:
                config['server']['host'] = '0.0.0.0'
                
                with open(config_path, 'w', encoding='utf-8') as f:
                    json.dump(config, f, indent=2, ensure_ascii=False)
                
                self.fixes_applied.append("修复了server.host配置缺失问题")
                print("   ✅ 已添加缺失的server.host配置")
            else:
                print("   ✅ 配置验证正常")
                
        except Exception as e:
            print(f"   ❌ 配置文件格式错误: {e}")
    
    def print_summary(self):
        """打印修复摘要"""
        print("\n" + "=" * 50)
        print("📊 修复结果摘要")
        print("=" * 50)
        
        if self.fixes_applied:
            print(f"✅ 成功应用 {len(self.fixes_applied)} 个修复:")
            for i, fix in enumerate(self.fixes_applied, 1):
                print(f"   {i}. {fix}")
            
            print("\n💡 修复完成！现在可以尝试启动应用程序:")
            print("   python main.py")
            
        else:
            print("🎉 未发现需要修复的问题！")
            print("💡 如果仍有启动问题，请检查:")
            print("   1. Python版本是否为3.8+")
            print("   2. 所有依赖是否已安装")
            print("   3. 查看日志文件中的详细错误信息")


def main():
    """主函数"""
    print("VCU测试开关桩管理工具 - 快速修复工具")
    print("版本: 1.0.0")
    print()
    
    fixer = QuickFixer()
    fixer.fix_all()
    
    print("\n" + "=" * 50)
    print("修复完成！")
    
    input("\n按回车键退出...")


if __name__ == "__main__":
    main() 