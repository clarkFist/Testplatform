import ast

try:
    with open('UDPToggle/gui/main_window.py', 'r', encoding='utf-8') as f:
        content = f.read()
    
    ast.parse(content)
    print('✅ 语法检查通过 - 没有缩进错误')
    
except SyntaxError as e:
    print(f'❌ 语法错误: {e}')
    print(f'行号: {e.lineno}')
    print(f'错误文本: {repr(e.text)}')
    
except Exception as e:
    print(f'其他错误: {e}') 