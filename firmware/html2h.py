#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
将二进制文件转换为C语言头文件（.h）
用法：python bin2h.py 输入二进制文件 输出h文件 [数组名(可选)]
示例：python bin2h.py logo.bin logo.h logo_data
"""
import sys

def get_filename(path_str):
    # 按反斜杠分割，过滤空字符串（避免开头/结尾的\导致空元素）
    parts = [p for p in path_str.split("\\") if p]
    # 返回最后一个元素（文件名）
    return parts[-1]


def bin_to_h(bin_file_path=".\include\webdata.html", h_file_path=".\include\webdata.h", array_name="message_root"):
    try:
        # 读取二进制文件
        with open(bin_file_path, "rb") as bin_file:
            bin_data = bin_file.read()
        
        # 生成头文件内容
        h_content = []
        # 头文件保护宏（避免重复包含）
        # guard_macro = f"{array_name.upper()}_H_"
        guard_macro = get_filename(h_file_path)
        guard_macro = guard_macro.rstrip(".h")
        guard_macro = guard_macro.upper()
        guard_macro = f"{guard_macro}_H_"
        h_content.append(f"#ifndef {guard_macro}\n")
        h_content.append(f"#define {guard_macro}\n")
        
        # 数据长度定义
        h_content.append(f"#define {array_name.upper()}_LEN {len(bin_data)}\n")
         
        # 二进制数据数组（十六进制格式，每行16个元素）
        h_content.append(f"const char {array_name}[] = {{")
        for i, byte in enumerate(bin_data):
            # 每16个字节换行，增加可读性
            if i % 16 == 0:
                h_content.append("\n    ")
            # 转换为十六进制（0xXX格式）
            h_content.append(f"0x{byte:02x}, ")
        # 移除最后一个多余的逗号，补全数组
        h_content[-1] = h_content[-1].rstrip(", ")
        # 添加一个0x00 结尾
        h_content.append(", 0x00")
        h_content.append("\n};\n")
        
        # 结束保护宏
        h_content.append(f"#endif // {guard_macro}")
        
        # 写入头文件
        with open(h_file_path, "w", encoding="utf-8") as h_file:
            h_file.write("".join(h_content))
        
        print(f"转换成功！")
        print(f"二进制文件：{bin_file_path} (大小：{len(bin_data)} 字节)")
        print(f"头文件输出：{h_file_path}")
        print(f"数组名：{array_name}，长度宏：{array_name}_LEN")
        
    except FileNotFoundError:
        print(f"错误：找不到文件 {bin_file_path}")
    except Exception as e:
        print(f"错误：{e}")

if __name__ == "__main__":
    # 命令行参数处理
    # if len(sys.argv) < 3:
    #     print("用法：python bin2h.py <输入二进制文件> <输出h文件> [数组名]")
    #     print("示例：python bin2h.py test.bin test.h my_data")
    #     sys.exit(1)
    FILE_PATH = ".\include\webdata.html"
    H_FILE_PATH = ".\include\webdata.h"
    
    bin_to_h(FILE_PATH, H_FILE_PATH, array_name="message_root")