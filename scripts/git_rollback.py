# -*- coding: utf-8 -*-
"""
Git提交历史查看和回退工具。
列出近期向远端仓库提交的更改，并提供用户友好的方法快速回退到任意一次提交。
"""

import subprocess
import sys
import re
from datetime import datetime


def run_cmd(cmd, capture_output=True):
    """运行shell命令并返回结果"""
    try:
        if capture_output:
            result = subprocess.run(cmd, shell=True, capture_output=True, text=True, encoding='utf-8')
            return result.returncode == 0, result.stdout.strip(), result.stderr.strip()
        else:
            result = subprocess.run(cmd, shell=True)
            return result.returncode == 0, "", ""
    except Exception as e:
        return False, "", str(e)1


def get_git_log(count=20, branch=None):
    """获取Git提交历史"""
    branch_arg = f" {branch}" if branch else ""
    cmd = f'git log{branch_arg} --oneline --decorate --graph -n {count}'
    success, output, error = run_cmd(cmd)
    
    if not success:
        print(f"❌ 获取Git日志失败: {error}")
        return None
    
    return output


def get_detailed_commit_info(commit_hash):
    """获取特定提交的详细信息"""
    cmd = f'git show --stat {commit_hash}'
    success, output, error = run_cmd(cmd)
    
    if not success:
        print(f"❌ 获取提交详情失败: {error}")
        return None
    
    return output


def get_current_branch():
    """获取当前分支名称"""
    cmd = 'git branch --show-current'
    success, output, error = run_cmd(cmd)
    
    if not success:
        return "main"
    
    return output or "main"


def show_commit_history(count=20):
    """显示提交历史"""
    print("🔍 正在获取提交历史...")
    print("=" * 80)
    
    log_output = get_git_log(count)
    if not log_output:
        return None
    
    lines = log_output.split('\n')
    commits = []
    
    print("📋 近期提交历史:")
    print("-" * 80)
    
    for i, line in enumerate(lines):
        if line.strip():
            # 提取提交哈希值
            hash_match = re.search(r'([a-f0-9]{7,40})', line)
            if hash_match:
                commit_hash = hash_match.group(1)
                commits.append(commit_hash)
            
            print(f"{i+1:2d}. {line}")
    
    print("-" * 80)
    return commits


def confirm_action(message):
    """确认用户操作"""
    while True:
        response = input(f"{message} (y/n): ").lower().strip()
        if response in ['y', 'yes', 'Y']:
            return True
        elif response in ['n', 'no', 'N']:
            return False
        else:
            print("请输入 y 或 n")


def reset_to_commit(commit_hash, mode='mixed'):
    """回退到指定提交"""
    current_branch = get_current_branch()
    
    print(f"\n⚠️  准备在分支 '{current_branch}' 上执行回退操作")
    print(f"📍 目标提交: {commit_hash}")
    print(f"🔧 回退模式: {mode}")
    
    if mode == 'soft':
        print("   - soft: 保留工作区和暂存区的更改")
    elif mode == 'mixed':
        print("   - mixed: 保留工作区的更改，清空暂存区")
    elif mode == 'hard':
        print("   - hard: 丢弃所有更改（危险操作）")
    
    if not confirm_action("\n确定要执行此回退操作吗？"):
        print("❌ 操作已取消")
        return False
    
    # 执行回退
    cmd = f'git reset --{mode} {commit_hash}'
    success, output, error = run_cmd(cmd, capture_output=False)
    
    if success:
        print(f"✅ 成功回退到提交 {commit_hash}")
        
        # 如果是hard模式，询问是否要强制推送到远程
        if mode == 'hard':
            if confirm_action("是否要强制推送到远程仓库？（这会覆盖远程历史）"):
                push_cmd = f'git push origin {current_branch} --force'
                push_success, _, push_error = run_cmd(push_cmd, capture_output=False)
                if push_success:
                    print("✅ 已强制推送到远程仓库")
                else:
                    print(f"❌ 推送失败: {push_error}")
        return True
    else:
        print(f"❌ 回退失败: {error}")
        return False


def interactive_rollback():
    """交互式回退操作"""
    # 显示提交历史
    commits = show_commit_history()
    if not commits:
        return
    
    while True:
        try:
            print("\n🎯 选择操作:")
            print("1. 输入序号回退到指定提交")
            print("2. 查看特定提交的详细信息")
            print("3. 刷新提交历史")
            print("4. 退出")
            
            choice = input("\n请选择操作 (1-4): ").strip()
            
            if choice == '1':
                # 回退操作
                commit_num = input(f"\n请输入要回退到的提交序号 (1-{len(commits)}): ").strip()
                try:
                    commit_index = int(commit_num) - 1
                    if 0 <= commit_index < len(commits):
                        commit_hash = commits[commit_index]
                        
                        print("\n🔧 选择回退模式:")
                        print("1. soft - 保留所有更改在暂存区")
                        print("2. mixed - 保留更改在工作区（默认）")
                        print("3. hard - 丢弃所有更改（危险）")
                        
                        mode_choice = input("请选择模式 (1-3, 默认2): ").strip() or '2'
                        mode_map = {'1': 'soft', '2': 'mixed', '3': 'hard'}
                        mode = mode_map.get(mode_choice, 'mixed')
                        
                        if reset_to_commit(commit_hash, mode):
                            if confirm_action("是否继续进行其他操作？"):
                                commits = show_commit_history()  # 刷新历史
                                continue
                            else:
                                break
                    else:
                        print("❌ 无效的序号")
                except ValueError:
                    print("❌ 请输入有效的数字")
            
            elif choice == '2':
                # 查看详细信息
                commit_num = input(f"\n请输入要查看的提交序号 (1-{len(commits)}): ").strip()
                try:
                    commit_index = int(commit_num) - 1
                    if 0 <= commit_index < len(commits):
                        commit_hash = commits[commit_index]
                        print(f"\n📝 提交 {commit_hash} 的详细信息:")
                        print("=" * 80)
                        detail = get_detailed_commit_info(commit_hash)
                        if detail:
                            print(detail)
                        print("=" * 80)
                    else:
                        print("❌ 无效的序号")
                except ValueError:
                    print("❌ 请输入有效的数字")
            
            elif choice == '3':
                # 刷新历史
                commits = show_commit_history()
            
            elif choice == '4':
                # 退出
                print("👋 再见！")
                break
            
            else:
                print("❌ 无效的选择，请输入 1-4")
                
        except KeyboardInterrupt:
            print("\n\n👋 操作已取消，再见！")
            break


def main():
    """主函数"""
    print("🚀 Git提交历史查看和回退工具")
    print("=" * 50)
    
    # 检查是否在Git仓库中
    success, _, _ = run_cmd('git rev-parse --git-dir')
    if not success:
        print("❌ 当前目录不是Git仓库")
        sys.exit(1)
    
    # 检查命令行参数
    if len(sys.argv) > 1:
        if sys.argv[1] in ['-h', '--help']:
            print("用法:")
            print("  python git_rollback.py          # 交互式模式")
            print("  python git_rollback.py -h       # 显示帮助")
            print("  python git_rollback.py --help   # 显示帮助")
            return
    
    # 启动交互式模式
    interactive_rollback()


if __name__ == '__main__':
    main() 