#!/usr/bin/env python3

# 读取文件
with open('UDPToggle/gui/main_window.py', 'r', encoding='utf-8') as f:
    lines = f.readlines()

# 修复第1399行的缩进问题
for i, line in enumerate(lines):
    line_num = i + 1
    if line_num == 1399 and 'self.refresh_switch_list()' in line:
        # 将第1399行的缩进与前面的行保持一致
        lines[i] = '                                self.refresh_switch_list()  # 重新刷新以显示选中的组\n'
        print(f'修复第{line_num}行缩进')
        break

# 写回文件
with open('UDPToggle/gui/main_window.py', 'w', encoding='utf-8') as f:
    f.writelines(lines)

print('修复完成') 