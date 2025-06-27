#!/usr/bin/env python3

# 读取文件
with open('UDPToggle/gui/main_window.py', 'r', encoding='utf-8') as f:
    lines = f.readlines()

# 修复第2714行的缩进问题
for i, line in enumerate(lines):
    line_num = i + 1
    if line_num == 2714 and 'f.write(f"{timestamp},{entry[' in line:
        # 修复缩进：应该是20个空格而不是28个
        lines[i] = '                    f.write(f"{timestamp},{entry[\'level\']},{entry[\'message\']}\\n")\n'
        print(f'修复第{line_num}行缩进')
        break

# 写回文件
with open('UDPToggle/gui/main_window.py', 'w', encoding='utf-8') as f:
    f.writelines(lines)

print('修复完成') 