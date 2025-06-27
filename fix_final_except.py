#!/usr/bin/env python3

# 读取文件
with open('UDPToggle/gui/main_window.py', 'r', encoding='utf-8') as f:
    lines = f.readlines()

# 修复最后的except语句缩进问题
for i, line in enumerate(lines):
    line_num = i + 1
    
    # 修复第2918和2919行的缩进
    if line_num == 2918 and 'logger.error' in line:
        lines[i] = '            logger.error(f"保存日志失败: {e}")\n'
        print(f'修复第{line_num}行: logger.error缩进')
    elif line_num == 2919 and 'messagebox.showerror' in line:
        lines[i] = '            messagebox.showerror("错误", f"保存日志失败: {e}")\n'
        print(f'修复第{line_num}行: messagebox.showerror缩进')

# 写回文件
with open('UDPToggle/gui/main_window.py', 'w', encoding='utf-8') as f:
    f.writelines(lines)

print('修复最后except语句缩进问题')

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