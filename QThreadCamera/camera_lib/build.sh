#!/bin/bash

# 安装依赖（若未安装）
echo "正在安装依赖..."
sudo apt-get install libjpeg-dev v4l-utils

# 检查上一步是否成功执行
if [ $? -ne 0 ]; then
    echo "依赖安装失败，请检查错误信息"
    exit 1
fi

# 编译动态库
echo "开始编译动态库..."
make

# 检查编译是否成功
if [ $? -ne 0 ]; then
    echo "编译失败，请检查错误信息"
    exit 1
fi

echo "所有操作完成"
