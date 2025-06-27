#!/usr/bin/env python3

# 读取文件
with open('UDPToggle/gui/main_window.py', 'r', encoding='utf-8') as f:
    content = f.read()

# 修复save_log方法的结构问题
old_save_log = '''    def save_log(self):
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
            messagebox.showerror("错误", f"保存日志失败: {e}")'''

new_save_log = '''    def save_log(self):
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
                messagebox.showerror("错误", f"保存日志失败: {e}")'''

content = content.replace(old_save_log, new_save_log)

# 写回文件
with open('UDPToggle/gui/main_window.py', 'w', encoding='utf-8') as f:
    f.write(content)

print('修复save_log方法结构')

# 测试语法
import ast
try:
    with open('UDPToggle/gui/main_window.py', 'r', encoding='utf-8') as f:
        content = f.read()
    ast.parse(content)
    print('✅ 语法检查通过 - 所有缩进错误已修复！')
except SyntaxError as e:
    print(f'❌ 还有语法错误: {e}')
    print(f'行号: {e.lineno}') 