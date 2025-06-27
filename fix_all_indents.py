#!/usr/bin/env python3

# 读取文件
with open('UDPToggle/gui/main_window.py', 'r', encoding='utf-8') as f:
    content = f.read()

# 修复各种缩进问题
fixes = [
    # 修复 refresh_switch_list 的缩进问题
    ('                                self.refresh_switch_list()', '            self.refresh_switch_list()'),
    # 修复 update_group_combo 的缩进问题  
    ('                                self.update_group_combo()', '            self.update_group_combo()'),
    # 修复其他可能的过度缩进
    ('                            else:', '                    else:'),
    ('                        for entry in self.operation_history:', '                for entry in self.operation_history:'),
    ('                            timestamp = time.strftime', '                    timestamp = time.strftime'),
    ('                            f.write(f"{timestamp} [{entry[\'level\']}] {entry[\'message\']}\\n")', '                    f.write(f"{timestamp} [{entry[\'level\']}] {entry[\'message\']}\\n")'),
    ('                        except Exception as e:', '                except Exception as e:'),
    ('                messagebox.showerror("错误", f"导出失败: {e}")', '                messagebox.showerror("错误", f"导出失败: {e}")'),
]

fixed_count = 0
for old, new in fixes:
    if old in content:
        content = content.replace(old, new)
        fixed_count += 1
        print(f"修复了缩进问题: {old[:50]}...")

# 写回文件
with open('UDPToggle/gui/main_window.py', 'w', encoding='utf-8') as f:
    f.write(content)

print(f"修复完成，共修复 {fixed_count} 处缩进错误") 