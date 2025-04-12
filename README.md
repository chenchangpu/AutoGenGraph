# AutoGenGraph master-quick分支使用方法

## 作用
将标准格式（且已经去重）的图数据转换为FPGA能处理的格式

## 使用要求
* Linux (Ubuntu)
* C++ (GCC) >= 9.5.0
* CMake     >= 3.17
* OpenMP    >= 4.5

## 用法

```
./quick_gen_data <base_path_in> <rmat_file> <partition_num> <partition_rate> <base_path_out>
```

### 参数说明：
- **base_path_in**: 输入base目录
- **rmat_file**: 输入标准图文件名称
- **partition_num**: neib的划分个数，一般为1、2或4，根据FPGA kernel的情况而定
- **partition_rate**: neib划分的超参数，一般为2或3
- **base_path_out**: 输出base目录

## 示例

./quick_gen_data ../data/v18_d16_c4/ v18_d16_c4.txt 4 2 ../data/v18_d16_c4

