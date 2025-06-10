# -*- coding: utf-8 -*-
"""
安全地提交并推送本地修改到远程的脚本。
会尝试多次执行 git push，如果失败则等待一段时间后重试。
"""

import subprocess
import time
import sys


def run_cmd(cmd):
    """Run a shell command and return True if it succeeds."""
    result = subprocess.run(cmd, shell=True)
    return result.returncode == 0


def push_local_changes(remote="origin", branch="main", commit_msg="Auto commit",
                       retries=100, delay=5):
    """Commit local changes and push to the remote branch with retries."""
    if not run_cmd('git add -A'):
        return False

    if not run_cmd(f'git commit -m "{commit_msg}"'):
        print('No changes to commit or commit failed.')
        return False

    for attempt in range(1, retries + 1):
        print(f'Attempt {attempt} to push to {remote}/{branch}...')
        if run_cmd(f'git push {remote} {branch}'):
            print('Push successful.')
            return True
        else:
            print('Push failed. Retrying...')
            time.sleep(delay)
    print('Failed to push after several attempts.')
    return False


if __name__ == '__main__':
    remote = sys.argv[1] if len(sys.argv) > 1 else 'origin'
    branch = sys.argv[2] if len(sys.argv) > 2 else 'main'
    commit_msg = sys.argv[3] if len(sys.argv) > 3 else 'Auto commit'
    success = push_local_changes(remote, branch, commit_msg)
    sys.exit(0 if success else 1)
