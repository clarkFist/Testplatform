#!/usr/bin/env python3

# 读取文件
with open('UDPToggle/gui/main_window.py', 'r', encoding='utf-8') as f:
    lines = f.readlines()

# 修复所有剩余的缩进问题
fixed_count = 0
for i, line in enumerate(lines):
    line_num = i + 1
    
    # 修复过度缩进的 f.write 语句
    if 'f.write(f"{timestamp},{entry[' in line and line.count(' ') >= 28:
        lines[i] = '                    f.write(f"{timestamp},{entry[\'level\']},{entry[\'message\']}\\n")\n'
        fixed_count += 1
        print(f'修复第{line_num}行: f.write CSV缩进')
    
    elif 'f.write(f"{timestamp} [{entry[' in line and line.count(' ') >= 20:
        lines[i] = '                    f.write(f"{timestamp} [{entry[\'level\']}] {entry[\'message\']}\\n")\n'
        fixed_count += 1
        print(f'修复第{line_num}行: f.write TXT缩进')
    
    # 修复messagebox.showerror的缩进
    elif 'messagebox.showerror("错误", f"导出失败: {e}")' in line and line.count(' ') >= 20:
        lines[i] = '                messagebox.showerror("错误", f"导出失败: {e}")\n'
        fixed_count += 1
        print(f'修复第{line_num}行: messagebox.showerror缩进')
    
    # 修复progress_dialog.destroy()的缩进
    elif 'progress_dialog.destroy()' in line and line.count(' ') >= 20:
        lines[i] = '            progress_dialog.destroy()\n'
        fixed_count += 1
        print(f'修复第{line_num}行: progress_dialog.destroy缩进')

# 写回文件
with open('UDPToggle/gui/main_window.py', 'w', encoding='utf-8') as f:
    f.writelines(lines)

print(f'修复完成，共修复 {fixed_count} 行')

# 现在测试语法
import ast
try:
    with open('UDPToggle/gui/main_window.py', 'r', encoding='utf-8') as f:
        content = f.read()
    ast.parse(content)
    print('✅ 语法检查通过 - 没有缩进错误')
except SyntaxError as e:
    print(f'❌ 还有语法错误: {e}')
    print(f'行号: {e.lineno}') 