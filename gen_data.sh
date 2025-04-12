#!/bin/bash

# 检查参数数量
if [ $# -ne 8 ]; then
    echo "用法: $0 v d o seed1 seed2 p r base_path"
    echo "  v: 顶点数量参数"
    echo "  d: 度数参数"
    echo "  o: 输出文件名"
    echo "  seed1: 随机种子1"
    echo "  seed2: 随机种子2"
    echo "  p: neib划分个数"
    echo "  r: neib划分超参"
    echo "  base_path: 输出目录路径"
    exit 1
fi

# 获取参数
v=$1
d=$2
o=$3
seed1=$4
seed2=$5
p=$6
r=$7 
base_path=$8

# 检查输出目录是否存在，若不存在则创建
if [ ! -d "$base_path" ]; then
    echo "输出目录 $base_path 不存在，正在创建..."
    mkdir -p "$base_path"
else
    echo "输出目录 $base_path 已存在"
fi

echo "开始生成图数据..."


# 步骤1: 生成图
echo "步骤1: 生成边..."
build/third_party/GFLabs/gen_edges -v $v -d $d -o "$base_path/$o" -u --seed1 $seed1 --seed2 $seed2
if [ $? -ne 0 ]; then
    echo "生成边失败！"
    exit 1
fi

# 步骤2: 转换图
echo "步骤2: 转换图..."
build/quick_gen_data "$base_path" "$o" $p $r "$base_path"
if [ $? -ne 0 ]; then
    echo "转换图失败！"
    exit 1
fi