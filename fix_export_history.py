#!/usr/bin/env python3

# 读取文件
with open('UDPToggle/gui/main_window.py', 'r', encoding='utf-8') as f:
    content = f.read()

# 修复export_history方法中的结构问题
old_code = '''                with open(file_path, 'w', encoding='utf-8') as f:
                    if file_path.endswith('.csv'):
                        f.write("时间,级别,消息\\n")
                for entry in self.operation_history:
                    timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(entry['timestamp']))
                    f.write(f"{timestamp},{entry['level']},{entry['message']}\\n")
                    else:
                for entry in self.operation_history:
                    timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(entry['timestamp']))
                    f.write(f"{timestamp} [{entry['level']}] {entry['message']}\\n")'''

new_code = '''                with open(file_path, 'w', encoding='utf-8') as f:
                    if file_path.endswith('.csv'):
                        f.write("时间,级别,消息\\n")
                        for entry in self.operation_history:
                            timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(entry['timestamp']))
                            f.write(f"{timestamp},{entry['level']},{entry['message']}\\n")
                    else:
                        for entry in self.operation_history:
                            timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(entry['timestamp']))
                            f.write(f"{timestamp} [{entry['level']}] {entry['message']}\\n")'''

content = content.replace(old_code, new_code)

# 写回文件
with open('UDPToggle/gui/main_window.py', 'w', encoding='utf-8') as f:
    f.write(content)

print('修复export_history方法结构问题')

# 测试语法
import ast
try:
    with open('UDPToggle/gui/main_window.py', 'r', encoding='utf-8') as f:
        content = f.read()
    ast.parse(content)
    print('✅ 语法检查通过 - 没有缩进错误')
except SyntaxError as e:
    print(f'❌ 还有语法错误: {e}')
    print(f'行号: {e.lineno}') 