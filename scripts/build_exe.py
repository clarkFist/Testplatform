# -*- coding: utf-8 -*-
"""使用 PyInstaller 将项目打包成可执行文件的脚本。

如果未找到指定的图标文件，会自动生成一个简单的“任务清单”图标。
"""

import subprocess
import sys
from datetime import datetime
from pathlib import Path


DEFAULT_ICON = Path("icon.ico")
DEFAULT_NAME = "todo"


def ensure_icon(icon_path: Path) -> Path:
    """确保图标文件存在，如不存在则自动创建一个。"""
    if icon_path.exists():
        return icon_path

    print(f"未找到图标 {icon_path}，正在自动创建……")
    try:
        from PIL import Image, ImageDraw
    except ImportError:
        print("缺少 Pillow，尝试安装……")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "Pillow"])
        from PIL import Image, ImageDraw

    img = Image.new("RGBA", (64, 64), (255, 255, 255, 0))
    draw = ImageDraw.Draw(img)

    # 画一个简单的任务清单图标
    draw.rectangle([16, 16, 48, 48], outline="black", width=2)
    for i in range(3):
        y = 24 + i * 8
        draw.line([(20, y), (44, y)], fill="black", width=2)

    icon_path.parent.mkdir(parents=True, exist_ok=True)
    img.save(icon_path)
    return icon_path


def build_exe(icon: Path = DEFAULT_ICON, base_name: str = DEFAULT_NAME):
    """调用 PyInstaller 打包项目, 输出文件名带时间戳."""
    icon_file = ensure_icon(icon)

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    name = f"{base_name}_{timestamp}"

    cmd = [
        "pyinstaller",
        "--onefile",
        "--noconsole",
        "--icon",
        str(icon_file),
        "--name",
        name,
        "main.py",
    ]
    print("运行:", " ".join(cmd))
    subprocess.check_call(cmd)


if __name__ == "__main__":
    base = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_NAME
    icon_arg = Path(sys.argv[2]) if len(sys.argv) > 2 else DEFAULT_ICON
    build_exe(icon=icon_arg, base_name=base)
