#!/usr/bin/env python3

# 读取文件
with open('UDPToggle/gui/main_window.py', 'r', encoding='utf-8') as f:
    content = f.read()

# 修复except语句的缩进问题
old_code = '''                messagebox.showinfo("成功", "历史记录导出成功！")
                                
                except Exception as e:
                messagebox.showerror("错误", f"导出失败: {e}")'''

new_code = '''                messagebox.showinfo("成功", "历史记录导出成功！")
                                
            except Exception as e:
                messagebox.showerror("错误", f"导出失败: {e}")'''

content = content.replace(old_code, new_code)

# 写回文件
with open('UDPToggle/gui/main_window.py', 'w', encoding='utf-8') as f:
    f.write(content)

print('修复except语句缩进问题')

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