# VS Code常用快捷键总结

## 常用的快捷键:

```json
1、 跳转操作
跳转到行数，也可以 Ctrl+G 直接进入

根据名字查找 symbol，也可以 Ctrl+T

新建文件:   Ctrl+N

文件之间切换:   Ctrl+Tab

打开一个新的VS Code编辑器:    Ctrl+Shift+N

关闭当前窗口:   Ctrl+W

关闭当前的VS Code编辑器:   Ctrl+Shift+W

切出一个新的编辑器窗口（最多3个):   Ctrl+\

切换左中右3个编辑器窗口的快捷键:   Ctrl+1  Ctrl+2  Ctrl+3

2、代码编辑

移动到上一次光标位置： Alt + <-
移动到后一次光标位置： Alt + ->
移动到上一次编辑位置： go to last edit localtion ,自定义编辑
打开文件所在位置: 右键 – open in integrated terminal
移动到文件结尾:   Ctrl+End

移动到文件开头:   Ctrl+Home

移动到定义处:   F12

查看定义处缩略图(只看一眼而不跳转过去):    Alt+F12

多行编辑(列编辑):   Alt+Shift+鼠标左键 或 Ctrl+Alt+Down/Up

同时选中所有匹配编辑(与当前行或选定内容匹配):   Ctrl+Shift+L

撤销上一步操作: Ctrl+Z

手动保存:   Ctrl+S

查找:   Ctrl+F

查找替换:   Ctrl+H
3、 显示操作

全屏显示(再次按则恢复):   F11

放大或缩小(以编辑器左上角为基准):   Ctrl +/-

侧边栏显示或隐藏： Ctrl+B

显示资源管理器(光标切到侧边栏中才有效):   Ctrl+Shift+E

显示搜索(光标切到侧边栏中才有效):   Ctrl+Shift+F

显示(光标切到侧边栏中才有效):   Git Ctrl+Shift+G

显示 Debug:    Ctrl+Shift+D

显示 Output:    Ctrl+Shift+U


自动保存：File -> AutoSave(中文界面下“文件”->“自动保存”) 或者 Ctrl+Shift+P，输入 auto
```

## 修改默认快捷键

打开默认键盘快捷方式设置：File -> Preferences -> Keyboard Shortcuts( 中文界面时：“文件”->"首选项"->"键盘快捷方式")  　　修改快捷键绑定方法一：直接在对应命令那一行点击，出现笔状图标，点击进入修改，如下图一所示：

![](.\pictures\2021-10-15-15-36-32-image.png)

 　　　　　　　　　　　　　　　　　　　　图一
　　修改快捷键绑定方法二：点击搜索栏下侧的“ keybindings.json ”，进入编辑界面，如下图所示：   

![](.\pictures\2021-10-15-15-36-47-image.png)

编写对应规则有一定的方法，如下所示：

```json
// 将键绑定放入此文件中以覆盖默认值
[{
    "key": "f8",
    "command": "workbench.action.tasks.runTask",
    "args": "build",
    "when": "editorTextFocus"
```

key    表示绑定的键

command    表示执行的命令

args    命令的参数，这里我们是build编译任务

when    快捷键在何时生效，这里指的是编辑区

保存完，尝试按下F8，任务便顺利运行了。

自己的" keybindings.json "文件参考如下(谨慎使用)：

```json
// Place your key bindings in this file to overwrite the defaults
[
 // ctrl+space 被切换输入法快捷键占用
 {
 "key": "ctrl+alt+space",
 "command": "editor.action.triggerSuggest",
 "when": "editorTextFocus"
 },
 // ctrl+d 删除一行
 {
 "key": "ctrl+d",
 "command": "editor.action.deleteLines",
 "when": "editorTextFocus"
 },
 // 与删除一行的快捷键互换
 {
 "key": "ctrl+shift+k",
 "command": "editor.action.addSelectionToNextFindMatch",
 "when": "editorFocus"
 },
 // ctrl+shift+/多行注释
 {
 "key":"ctrl+shift+/",
 "command": "editor.action.blockComment",
 "when": "editorTextFocus"
 },
 // 定制与 sublime 相同的大小写转换快捷键
 editor.action.transformToLowercase
 editor.action.transformToUppercase
 {
 "key": "ctrl+k ctrl+u",
 "command": "editor.action.transformToUppercase"
 "when": "editorTextFocus"
 },
 {
 "key": "ctrl+k ctrl+l",
 "command": "editor.action.transformToLowercase"
 "when": "editorTextFocus"
 }
]
```

　　[参考文档](https://lzw.me/a/vscode-visual-studio-code-shortcut.html)：https://lzw.me/a/vscode-visual-studio-code-shortcut.html
　　[官方快捷键大全](https://code.visualstudio.com/docs/getstarted/keybindings)：https://code.visualstudio.com/docs/getstarted/keybindings
读书原为修身，正己才能正人正世；不修身不正己而去正人正世者，无一不是盗名欺世；你把念过的书能用上十之一二，就是很了不得的人了。——朱先生
转载自[https://www.cnblogs.com/schut/p/10461840.html](https://www.cnblogs.com/schut/p/10461840.html))































