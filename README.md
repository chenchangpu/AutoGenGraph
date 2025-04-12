# AutoGenGraph rmat-quick分支使用方法

## 作用
将标准格式（且已经去重）的图数据转换为FPGA能处理的格式

## 使用要求
* Linux (Ubuntu)
* C++ (GCC) >= 9.5.0
* CMake     >= 3.17
* OpenMP    >= 4.5

## 用法

```
./gen_data.sh v d rmat_file seed1 seed2 partition_num partition_rate base_path_out
```

### 参数说明：
- **v**: 生成rmat图的顶点数的对数
- **d**: 生成rmat图的边数
- **rmat_file**: 生成的rmat图的文件名，后缀为"_c{$partition_num}.txt" 或 "_c{$partition_num}.bin"
- **seed1**: 生成rmat图的随机种子1
- **seed2**: 生成rmat图的随机种子2
- **partition_num**: neib的划分个数，一般为1、2、4或8，最大为10，根据FPGA kernel的情况而定
- **partition_rate**: neib划分的超参数，一般为2或3
- **base_path_out**: 输出base目录

## 示例

```
./gen_data.sh 18 16 rmat_v18_d16_c4.bin 0 0 4 2 data/rmat_v18_d16_c4

./gen_data.sh 18 16 rmat_v18_d16_c4.txt 0 0 4 2 data/rmat_v18_d16_c4
```
