# -*- coding: utf-8 -*-
"""
安全地更新仓库代码的脚本。
先清除本地未提交的修改，再从远端拉取最新代码。
如遇网络问题，会自动重试。
"""

import subprocess
import time
import sys


def run_cmd(cmd):
    """Run a shell command and return True if it succeeds."""
    result = subprocess.run(cmd, shell=True)
    return result.returncode == 0


def reset_local():
    """Discard local changes and remove untracked files."""
    run_cmd('git reset --hard')
    run_cmd('git clean -fd')


def update_from_remote(remote='origin', branch='main', retries=100, delay=5):
    """Update the repository from the remote branch with retries."""
    reset_local()
    for attempt in range(1, retries + 1):
        print(f'Attempt {attempt} to update from {remote}/{branch}...')
        if run_cmd(f'git fetch {remote}') and \
                run_cmd(f'git reset --hard {remote}/{branch}'):
            print('Update successful.')
            return True
        else:
            print('Update failed. Retrying...')
            time.sleep(delay)
    print('Failed to update after several attempts.')
    return False


if __name__ == '__main__':
    remote = sys.argv[1] if len(sys.argv) > 1 else 'origin'
    branch = sys.argv[2] if len(sys.argv) > 2 else 'main'
    success = update_from_remote(remote, branch)
    sys.exit(0 if success else 1)
