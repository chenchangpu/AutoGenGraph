#!/bin/bash

# Check parameter count
if [ $# -ne 8 ]; then
    echo "Usage: $0 v d o seed1 seed2 p r base_path"
    echo "  v: Vertex count parameter"
    echo "  d: Degree parameter"
    echo "  o: Output filename"
    echo "  seed1: Random seed 1"
    echo "  seed2: Random seed 2"
    echo "  p: Number of neib partitions"
    echo "  r: Neib partition hyperparameter"
    echo "  base_path: Output directory path"
    exit 1
fi

# Get parameters
v=$1
d=$2
o=$3
seed1=$4
seed2=$5
p=$6
r=$7 
base_path=$8

# Check if output directory exists, create if not
if [ ! -d "$base_path" ]; then
    echo "Output directory $base_path does not exist, creating..."
    mkdir -p "$base_path"
else
    echo "Output directory $base_path already exists"
fi

echo "Starting graph data generation..."


# Step 1: Generate graph
echo "Step 1: Generating edges..."
build/third_party/GFLabs/gen_edges -v $v -d $d -o "$base_path/$o" -u --seed1 $seed1 --seed2 $seed2
if [ $? -ne 0 ]; then
    echo "Failed to generate edges!"
    exit 1
fi

# Step 2: Convert graph
echo "Step 2: Converting graph format..."
build/quick_gen_data "$base_path" "$o" $p $r "$base_path"
if [ $? -ne 0 ]; then
    echo "Failed to convert graph format!"
    exit 1
fi