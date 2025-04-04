#!/bin/bash

# 检查参数数量
if [ $# -ne 4 ]; then
    echo "用法: $0 base_path i c r"
    echo "  base_path: 输入/输出目录路径"
    echo "  i: 输入图像文件名"
    echo "  c: neib划分个数"
    echo "  r: neib划分超参(一般等于2或3)"

    exit 1
fi

# 获取参数
base_path=$1
# o = i, 给定输入图
o=$2  
# p = c, neib划分个数   
p=$3
r=$4 


# 检查输出目录是否存在，若不存在则创建
if [ -f "$base_path/$o" ]; then
    echo "图文件存在：$base_path/$o"
else
    echo "文件不存在：$base_path/$o"
    exit 1
fi

echo "开始转换图数据..."

o_no_ext="${o%.txt}"

# 步骤1: 去除重复边
echo "步骤1: 去除重复边..."
# 创建临时文件
temp_file="$base_path/temp_$o"
cp "$base_path/$o" "$temp_file"
build/src/unique_edges "$temp_file" "$base_path/$o"
if [ $? -ne 0 ]; then
    echo "去除重复边失败！"
    exit 1
fi
rm "$temp_file"  # 删除临时文件


# 步骤2: 转换图格式
echo "步骤2: 转换图格式..."
# 构建输出文件名
edge_table="$base_path/edge_table_${o_no_ext}_c${p}.txt"
neib_table="$base_path/neib_table_${o_no_ext}_c${p}.txt"
build/src/graph_converter "$base_path/$o" "$edge_table" "$neib_table"
if [ $? -ne 0 ]; then
    echo "转换图格式失败！"
    exit 1
fi


# 步骤3：生成划分元数据文件
echo "步骤3: 生成划分元数据文件..."
# 构建输出文件
temp_rmat_part="$base_path/temp_${o_no_ext}_c${p}_r${r}_part.txt"
build/third_party/GFLabs/gen_partitions_2 -i "$base_path/$o" -o "$temp_rmat_part" -p $p -r $r 
# 获取neib的行数
neib_lines=$(wc -l < "$neib_table")
# 检查temp_rmat_part的最后一个元素是否大于neib_lines
last_element=$(tail -n 1 "$temp_rmat_part")
if [ "$last_element" -gt "$neib_lines" ]; then
    echo "发现最后一个分区元素 ($last_element) 大于neib行数 ($neib_lines)，正在修正..."
    # 创建临时文件
    temp_fixed_file="${temp_rmat_part}.fixed"
    # 复制除最后一行外的所有行
    head -n -1 "$temp_rmat_part" > "$temp_fixed_file"
    # 添加修正后的最后一行
    echo "$neib_lines" >> "$temp_fixed_file"
    # 替换原文件
    mv "$temp_fixed_file" "$temp_rmat_part"
    echo "分区元数据已修正"
fi
rmat_part="$base_path/${o_no_ext}_c${p}_r${r}_part.txt"
build/src/gen_meta_partition $p "$temp_rmat_part" "$rmat_part"


# 步骤4：划分数据
echo "步骤4: 划分数据..."
build/src/split_neib "$base_path" "neib_table_${o_no_ext}_c${p}.txt" "temp_${o_no_ext}_c${p}_r${r}_part.txt"


# 步骤5: 生成元数据文件
echo "步骤5: 生成元数据文件..."
meta_file="$base_path/graph_meta_${o_no_ext}_c${p}.txt"
# 计算行数并写入元数据文件
edge_lines=$(wc -l < "$edge_table")
echo "$edge_lines" > "$meta_file"
# 将rmat_part文件的内容追加到meta_file
cat "$rmat_part" >> "$meta_file"
echo "元数据文件已生成: $meta_file (包含边数和neib分区信息)"


# 步骤6: 转换为二进制格式
echo "步骤6: 转换为二进制格式..."
# 转换边表文件
edge_table_base="${edge_table%.txt}"
echo "转换边表文件: $edge_table_base.txt"
build/src/txt2binary "$edge_table_base"
# 转换元数据文件
meta_file_base="${meta_file%.txt}"
echo "转换元数据文件: $meta_file_base.txt"
build/src/txt2binary "$meta_file_base"

# 转换分区邻居表文件
echo "转换分区邻居表文件..."
for ((i=1; i<=$p; i++)); do
    part_neib_file="$base_path/neib_table_${o_no_ext}_c${p}_${i}"
    if [ -f "${part_neib_file}.txt" ]; then
        echo "转换: ${part_neib_file}.txt"
        build/src/txt2binary "$part_neib_file"
    else
        echo "警告: 文件 ${part_neib_file}.txt 不存在，跳过"
    fi
done
echo "所有文件转换完成！"

echo "所有数据处理完成！"

exit 0 