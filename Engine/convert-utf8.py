import os
import chardet
 
# 功能：遍历脚本所在目录及其子目录中的所有.c和.h文件，并将它们的内容转换为 UTF-8 编码
# 使用方法：将该脚本放置在你想要处理的文件夹中，cmd中输入python convert_to_utf8.py
# 注意：请先备份要处理的文件，以防意外发生


def convert_to_utf8(file_path):
    # 读取文件并检测编码
    with open(file_path, 'rb') as file:
        raw_data = file.read()
        result = chardet.detect(raw_data)
        encoding = result['encoding']
 
    # 如果编码不是 UTF-8，则进行转换
    if encoding != 'utf-8':
        with open(file_path, 'r', encoding=encoding, errors='ignore') as file:
            content = file.read()
 
        with open(file_path, 'w', encoding='utf-8') as file:
            file.write(content)
 
        print(f'Converted {file_path} to UTF-8')
    else:
        print(f'{file_path} is already in UTF-8')


def process_directory(directory):
    # 遍历指定目录及其子目录
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.cpp') or file.endswith('.h'):
                file_path = os.path.join(root, file)
                convert_to_utf8(file_path)
 
if __name__ == '__main__':
    # 获取脚本所在目录
    # script_dir = os.path.dirname(os.path.abspath(__file__))
    process_directory('Src')