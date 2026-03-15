#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import re
import time
from datetime import datetime


def update_version_and_time(file_path, version_type="patch"):
    """
    更新文件中的版本号和时间戳
    :param file_path: 目标文件路径
    :param version_type: 版本更新类型，可选：patch(补丁，如0.1→0.2)、minor(小版本，如0.1→1.0)、major(大版本，如1.0→2.0)
    :return: 布尔值，更新成功返回True，失败返回False
    """
    # 1. 读取文件内容
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"错误：文件 {file_path} 不存在！")
        return False
    except Exception as e:
        print(f"读取文件失败：{e}")
        return False

    # 2. 正则匹配原有版本和时间字符串（适配格式：版本： vX.X- 年-月-日 时:分:秒）
    pattern = r'版本：v(\d+)\.(\d+) - \d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}(?:\.\d+)?'
    match = re.search(pattern, content)
    if not match:
        print("错误：未找到匹配的版本号格式（格式要求：版本： vX.X - 年-月-日 时:分:秒）")
        return False

    # 3. 提取原有版本号并递增
    major_ver = int(match.group(1))  # 主版本号（如v0.1中的0）
    minor_ver = int(match.group(2))  # 次版本号（如v0.1中的1）

    # 根据版本类型更新
    if version_type == "patch":
        minor_ver += 1  # 补丁版本：v0.1 → v0.2
    elif version_type == "minor":
        major_ver += 1  # 小版本：v0.1 → v1.0
        minor_ver = 0
    elif version_type == "major":
        major_ver += 1  # 大版本：v1.0 → v2.0
        minor_ver = 0
    else:
        print("错误：version_type只能是 patch/minor/major")
        return False

    # 4. 生成新的版本号和当前时间戳
    new_version = f"v{major_ver}.{minor_ver}"
    # 获取当前时间（格式：2026-03-15 19:42:53）
    new_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    # 拼接新的字符串
    new_str = f"版本：{new_version} - {new_time}"

    # 5. 替换文件内容中的旧字符串
    # 先匹配旧的完整字符串（用于替换）
    old_str = match.group(0)
    new_content = content.replace(old_str, new_str)

    # 6. 写入文件
    try:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"更新成功！")
        print(f"旧版本/时间：{old_str}")
        print(f"新版本/时间：{new_str}")
        return True
    except Exception as e:
        print(f"写入文件失败：{e}")
        return False

# ==================== 调用示例 ====================
if __name__ == "__main__":
    # 替换为你的文件路径（如：test.txt、config.ini等）
    FILE_PATH = ".\include\webdata.html"
    
    # 可选版本更新类型：patch(默认)/minor/major
    # patch：补丁更新（v0.1→v0.2）
    # minor：小版本更新（v0.1→v1.0）
    # major：大版本更新（v1.0→v2.0）
    update_version_and_time(FILE_PATH, version_type="patch")