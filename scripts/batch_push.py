# -*- coding: utf-8 -*-
"""
安全分批提交并推送本地修改到远程仓库的脚本。
支持按目录结构或文件大小将文件分成小批次，逐批提交和推送，避免一次性提交过多内容导致的问题。
"""

import subprocess
import time
import sys
import os
import math
from pathlib import Path
from collections import defaultdict


def run_cmd(cmd, capture_output=False):
    """执行shell命令并返回是否成功。"""
    try:
        if capture_output:
            result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
            return result.returncode == 0, result.stdout.strip()
        else:
            result = subprocess.run(cmd, shell=True)
            return result.returncode == 0
    except Exception as e:
        print(f"命令执行错误: {e}")
        if capture_output:
            return False, ""
        return False


def get_current_branch():
    """获取当前分支名。"""
    success, branch = run_cmd('git branch --show-current', capture_output=True)
    if success and branch:
        return branch
    # 备用方法
    success, output = run_cmd('git rev-parse --abbrev-ref HEAD', capture_output=True)
    if success:
        return output
    return None


def get_remote_branches(remote="origin"):
    """获取远程分支列表。"""
    success, output = run_cmd(f'git ls-remote --heads {remote}', capture_output=True)
    if not success:
        return []
    
    branches = []
    for line in output.split('\n'):
        if line.strip():
            # 解析格式: hash refs/heads/branch-name
            parts = line.split('\t')
            if len(parts) >= 2:
                ref = parts[1]
                if ref.startswith('refs/heads/'):
                    branch_name = ref[len('refs/heads/'):]
                    branches.append(branch_name)
    return branches


def check_remote_exists(remote="origin"):
    """检查远程仓库是否存在。"""
    success, output = run_cmd('git remote', capture_output=True)
    if success and output:
        remotes = output.split('\n')
        return remote in remotes
    return False


def setup_remote_repository():
    """设置远程仓库。"""
    print("📡 当前没有配置远程仓库")
    
    repo_url = input("请输入GitHub仓库URL (例如: https://github.com/username/repo.git): ").strip()
    if not repo_url:
        print("❌ 未提供仓库URL")
        return False
    
    # 添加远程仓库
    if run_cmd(f'git remote add origin "{repo_url}"'):
        print("✅ 远程仓库添加成功")
        return True
    else:
        print("❌ 添加远程仓库失败")
        return False


def check_and_setup_branch(remote="origin", target_branch=None):
    """检查并设置正确的分支。"""
    # 检查远程仓库是否存在
    if not check_remote_exists(remote):
        print(f"⚠️  远程仓库 {remote} 不存在")
        
        if remote == "origin":
            if not setup_remote_repository():
                return None, None
        else:
            print(f"❌ 请先添加远程仓库: git remote add {remote} <url>")
            return None, None
    
    # 获取当前分支
    current_branch = get_current_branch()
    if not current_branch:
        print("❌ 无法获取当前分支信息")
        return None, None
    
    print(f"当前分支: {current_branch}")
    
    # 如果没有指定目标分支，使用当前分支
    if not target_branch:
        target_branch = current_branch
    
    # 获取远程分支列表
    print(f"检查远程仓库 {remote} 的分支...")
    remote_branches = get_remote_branches(remote)
    
    if not remote_branches:
        print(f"⚠️  远程仓库可能为空或无法访问")
        choice = input(f"是否继续推送到 {remote}/{target_branch} (这将创建新分支)? (y/N): ").strip().lower()
        if choice not in ['y', 'yes', '是']:
            print("❌ 操作取消")
            return None, None
        print(f"将创建新的远程分支: {target_branch}")
        return current_branch, target_branch
    
    print(f"远程分支: {', '.join(remote_branches)}")
    
    # 检查目标分支是否存在于远程
    if target_branch not in remote_branches:
        print(f"⚠️  远程分支 {target_branch} 不存在")
        
        # 如果远程有main分支但目标不是main，询问是否切换
        if 'main' in remote_branches and target_branch != 'main':
            choice = input(f"发现远程有 main 分支，是否切换到 main 分支? (y/N): ").strip().lower()
            if choice in ['y', 'yes', '是']:
                target_branch = 'main'
                if current_branch != 'main':
                    print("正在切换到 main 分支...")
                    # 检查是否有未提交的更改
                    success, status = run_cmd('git status --porcelain', capture_output=True)
                    if success and status.strip():
                        print("⚠️  检测到未提交的更改，请先提交或储藏更改后再切换分支")
                        return None, None
                    
                    if run_cmd('git checkout main'):
                        current_branch = 'main'
                        print("✓ 已切换到 main 分支")
                    else:
                        print("切换分支失败，将创建新的远程分支")
        
        # 如果远程有master分支但目标不是master，询问是否切换
        elif 'master' in remote_branches and target_branch != 'master':
            choice = input(f"发现远程有 master 分支，是否切换到 master 分支? (y/N): ").strip().lower()
            if choice in ['y', 'yes', '是']:
                target_branch = 'master'
                if current_branch != 'master':
                    print("正在切换到 master 分支...")
                    # 检查是否有未提交的更改
                    success, status = run_cmd('git status --porcelain', capture_output=True)
                    if success and status.strip():
                        print("⚠️  检测到未提交的更改，请先提交或储藏更改后再切换分支")
                        return None, None
                    
                    if run_cmd('git checkout master'):
                        current_branch = 'master'
                        print("✓ 已切换到 master 分支")
                    else:
                        print("切换分支失败，将创建新的远程分支")
        
        # 否则询问是否创建新的远程分支
        else:
            choice = input(f"是否创建新的远程分支 {target_branch}? (y/N): ").strip().lower()
            if choice not in ['y', 'yes', '是']:
                print("❌ 操作取消")
                return None, None
            print(f"将创建新的远程分支: {target_branch}")
    
    return current_branch, target_branch


def get_all_files():
    """获取项目中所有文件（包括已跟踪和未跟踪的）。"""
    all_files = []
    
    # 获取未跟踪的文件
    success, output = run_cmd('git ls-files --others --exclude-standard', capture_output=True)
    if success and output:
        untracked_files = [f for f in output.split('\n') if f.strip()]
        all_files.extend(untracked_files)
    
    # 获取已修改的文件
    success, output = run_cmd('git diff --name-only', capture_output=True)
    if success and output:
        modified_files = [f for f in output.split('\n') if f.strip()]
        all_files.extend(modified_files)
    
    # 获取已暂存的文件
    success, output = run_cmd('git diff --cached --name-only', capture_output=True)
    if success and output:
        staged_files = [f for f in output.split('\n') if f.strip()]
        all_files.extend(staged_files)
    
    # 去重并过滤空字符串
    return list(set(f for f in all_files if f.strip()))


def get_changed_files():
    """获取所有待提交的文件列表。"""
    success, output = run_cmd('git status --porcelain', capture_output=True)
    if not success:
        print("无法获取git状态")
        return []
    
    files = []
    for line in output.split('\n'):
        if line.strip():
            # 解析git status输出格式
            status = line[:2]
            filename = line[2:].strip()
            if filename:  # 忽略空文件名
                files.append(filename)
    return files


def get_file_size(filepath):
    """获取文件大小（字节）。"""
    try:
        return os.path.getsize(filepath)
    except:
        return 0


def group_files_by_directory(files, max_batch_size_mb=50, directory_depth=1):
    """按目录结构将文件分组。
    
    Args:
        files: 文件列表
        max_batch_size_mb: 每批最大大小(MB)
        directory_depth: 目录深度，1表示顶级目录，2表示二级目录等
    """
    max_batch_size = max_batch_size_mb * 1024 * 1024  # 转换为字节
    dir_groups = defaultdict(list)
    
    # 按目录分组
    for filepath in files:
        path_parts = Path(filepath).parts
        if len(path_parts) >= directory_depth:
            # 取指定深度的目录路径
            dir_key = '/'.join(path_parts[:directory_depth])
        else:
            # 如果文件深度不够，使用其父目录
            dir_key = '/'.join(path_parts[:-1]) if len(path_parts) > 1 else 'root'
        
        dir_groups[dir_key].append(filepath)
    
    batches = []
    
    # 为每个目录组创建批次
    for dir_name, dir_files in dir_groups.items():
        # 计算目录总大小
        total_size = sum(get_file_size(f) for f in dir_files)
        
        if total_size <= max_batch_size or len(dir_files) <= 1:
            # 如果目录大小合适或只有一个文件，作为一个批次
            batches.append(dir_files)
        else:
            # 如果目录太大，按文件大小进一步拆分
            sub_batches = group_files_by_size(dir_files, max_batch_size_mb)
            batches.extend(sub_batches)
    
    return batches


def group_files_by_size(files, max_batch_size_mb=50):
    """按大小将文件分组，每组不超过指定大小。"""
    max_batch_size = max_batch_size_mb * 1024 * 1024  # 转换为字节
    batches = []
    current_batch = []
    current_size = 0
    
    # 按文件大小排序，小文件优先
    files_with_size = [(f, get_file_size(f)) for f in files]
    files_with_size.sort(key=lambda x: x[1])
    
    for filepath, file_size in files_with_size:
        # 如果单个文件就超过批次大小，单独成批
        if file_size > max_batch_size:
            if current_batch:
                batches.append(current_batch)
                current_batch = []
                current_size = 0
            batches.append([filepath])
            continue
        
        # 如果添加此文件会超过批次大小，开始新批次
        if current_size + file_size > max_batch_size and current_batch:
            batches.append(current_batch)
            current_batch = [filepath]
            current_size = file_size
        else:
            current_batch.append(filepath)
            current_size += file_size
    
    # 添加最后一批
    if current_batch:
        batches.append(current_batch)
    
    return batches


def get_batch_description(files, batch_num, total_batches):
    """获取批次描述信息。"""
    # 统计目录信息
    dirs = set()
    for f in files:
        dir_path = str(Path(f).parent)
        if dir_path != '.':
            dirs.add(dir_path)
    
    total_size = sum(get_file_size(f) for f in files) / (1024 * 1024)
    
    desc = f"第 {batch_num}/{total_batches} 批: {len(files)} 个文件, 约 {total_size:.1f}MB"
    if dirs:
        if len(dirs) <= 3:
            desc += f", 目录: {', '.join(sorted(dirs))}"
        else:
            desc += f", 涉及 {len(dirs)} 个目录"
    
    return desc


def commit_and_push_batch(files, batch_num, total_batches, commit_prefix="分批提交",
                         remote="origin", branch="master", retries=3, delay=5, first_push=False):
    """提交并推送一批文件。"""
    if not files:
        return True
    
    print(f"\n=== 处理第 {batch_num}/{total_batches} 批文件 ===")
    print(get_batch_description(files, batch_num, total_batches))
    
    # 显示文件列表（限制显示数量）
    print("包含文件:")
    for i, f in enumerate(files):
        if i < 10:  # 只显示前10个文件
            print(f"  - {f}")
        elif i == 10:
            print(f"  ... 还有 {len(files) - 10} 个文件")
            break
    
    # 添加文件到暂存区
    for file in files:
        if not run_cmd(f'git add "{file}"'):
            print(f"添加文件失败: {file}")
            return False
    
    # 提交
    commit_msg = f"{commit_prefix} ({batch_num}/{total_batches})"
    # 如果是按目录分组，在提交信息中包含主要目录
    dirs = set(str(Path(f).parent) for f in files if str(Path(f).parent) != '.')
    if dirs:
        main_dirs = sorted(dirs)[:3]  # 取前3个目录
        dir_info = ', '.join(main_dirs)
        if len(dirs) > 3:
            dir_info += f" 等{len(dirs)}个目录"
        commit_msg += f" - {dir_info}"
    
    if not run_cmd(f'git commit -m "{commit_msg}"'):
        print("提交失败，可能没有更改或出现错误")
        return False
    
    # 推送（带重试机制）
    for attempt in range(1, retries + 1):
        print(f"  尝试推送... (第 {attempt} 次)")
        
        # 如果是第一次推送且可能是新分支，使用 -u 参数
        push_cmd = f'git push {remote} {branch}'
        if first_push and batch_num == 1:
            push_cmd = f'git push -u {remote} {branch}'
        
        if run_cmd(push_cmd):
            print(f"  ✓ 第 {batch_num} 批推送成功")
            return True
        else:
            if attempt < retries:
                print(f"  ✗ 推送失败，{delay}秒后重试...")
                time.sleep(delay)
            else:
                print(f"  ✗ 第 {batch_num} 批推送失败，已重试 {retries} 次")
                
                # 在最后一次失败时，尝试提供更详细的错误信息
                try:
                    result = subprocess.run(f'git push {remote} {branch}', shell=True, capture_output=True, text=True)
                    if result.stderr:
                        print(f"  错误详情: {result.stderr.strip()}")
                    elif result.stdout:
                        print(f"  输出信息: {result.stdout.strip()}")
                except Exception:
                    pass
                return False
    
    return False


def batch_push_changes(batch_size_mb=50, commit_prefix="分批提交", 
                      remote="origin", branch=None, retries=3, delay=5,
                      grouping_method="directory", directory_depth=1, include_all=False):
    """分批提交并推送所有本地更改。
    
    Args:
        batch_size_mb: 每批最大大小(MB)
        commit_prefix: 提交信息前缀
        remote: 远程仓库名
        branch: 分支名
        retries: 推送重试次数
        delay: 重试间隔
        grouping_method: 分组方法 ('directory' 或 'size')
        directory_depth: 目录深度（仅当grouping_method为'directory'时有效）
        include_all: 是否包含项目中的所有文件，而不仅仅是已修改的文件
    """
    print("开始分批提交流程...")
    
    # 检查是否在git仓库中
    if not run_cmd('git rev-parse --is-inside-work-tree', capture_output=True)[0]:
        print("❌ 错误: 当前目录不是git仓库")
        return False
    
    # 检查并设置正确的分支
    print("\n🔍 检查分支状态...")
    current_branch, target_branch = check_and_setup_branch(remote, branch)
    if not current_branch or not target_branch:
        return False
    
    print(f"✓ 将推送到: {remote}/{target_branch}")
    
    # 获取文件列表
    print(f"\n📋 正在获取文件列表...")
    if include_all:
        print("  模式: 包含项目中的所有文件")
        files_to_process = get_all_files()
    else:
        print("  模式: 仅包含已修改的文件")
        files_to_process = get_changed_files()
    
    if not files_to_process:
        if include_all:
            print("✓ 项目中没有文件")
        else:
            print("✓ 没有待提交的文件")
        return True
    
    print(f"📁 找到 {len(files_to_process)} 个文件")
    
    # 按指定方法分组文件
    print(f"📦 分组方法: {grouping_method}")
    if grouping_method == "directory":
        print(f"  目录深度: {directory_depth}")
        batches = group_files_by_directory(files_to_process, batch_size_mb, directory_depth)
    else:
        print(f"  按大小分组，每批最大 {batch_size_mb}MB")
        batches = group_files_by_size(files_to_process, batch_size_mb)
    
    print(f"共分为 {len(batches)} 批:")
    for i, batch in enumerate(batches, 1):
        print(f"  {get_batch_description(batch, i, len(batches))}")
    
    # 确认是否继续
    confirm = input(f"\n🚀 是否继续分批提交到 {remote}/{target_branch}? (y/N): ").strip().lower()
    if confirm not in ['y', 'yes', '是']:
        print("❌ 取消操作")
        return False
    
    # 检查远程分支是否存在（用于判断是否需要设置上游分支）
    remote_branches = get_remote_branches(remote)
    is_first_push = target_branch not in remote_branches
    
    # 逐批处理
    success_count = 0
    for i, batch in enumerate(batches, 1):
        if commit_and_push_batch(batch, i, len(batches), commit_prefix, 
                                remote, target_branch, retries, delay, is_first_push):
            success_count += 1
            is_first_push = False  # 第一次成功后，后续不需要 -u 参数
        else:
            print(f"\n❌ 第 {i} 批处理失败，停止后续操作")
            break
        
        # 批次间暂停
        if i < len(batches):
            print(f"⏰ 等待 {delay} 秒后处理下一批...")
            time.sleep(delay)
    
    print(f"\n=== 🎉 完成 ===")
    print(f"✅ 成功处理: {success_count}/{len(batches)} 批")
    
    if success_count == len(batches):
        print(f"🎊 所有文件已成功推送到 {remote}/{target_branch}")
    
    return success_count == len(batches)


if __name__ == '__main__':
    import argparse
    
    parser = argparse.ArgumentParser(description='安全分批提交并推送到远程仓库')
    parser.add_argument('--batch-size', type=int, default=50, 
                       help='每批最大大小(MB)，默认50MB')
    parser.add_argument('--remote', default='origin', 
                       help='远程仓库名，默认origin')
    parser.add_argument('--branch', default=None, 
                       help='分支名，默认使用当前分支')
    parser.add_argument('--commit-prefix', default='分批提交', 
                       help='提交信息前缀')
    parser.add_argument('--retries', type=int, default=3, 
                       help='推送失败重试次数，默认3次')
    parser.add_argument('--delay', type=int, default=5, 
                       help='重试间隔秒数，默认5秒')
    parser.add_argument('--grouping', choices=['directory', 'size'], default='directory',
                       help='分组方法：directory(按目录) 或 size(按大小)，默认directory')
    parser.add_argument('--depth', type=int, default=1,
                       help='目录深度（仅在按目录分组时有效），默认1')
    parser.add_argument('--all', action='store_true',
                       help='包含项目中的所有文件，而不仅仅是已修改的文件')
    
    args = parser.parse_args()
    
    success = batch_push_changes(
        batch_size_mb=args.batch_size,
        commit_prefix=args.commit_prefix,
        remote=args.remote,
        branch=args.branch,
        retries=args.retries,
        delay=args.delay,
        grouping_method=args.grouping,
        directory_depth=args.depth,
        include_all=args.all
    )
    
    sys.exit(0 if success else 1) 