# AutoGenGraph rmat分支使用

## 用法

```
./gen_data.sh v d o seed1 seed2 p r base_path
```

### 参数说明：
- **v**: 生成rmat图的顶点数为$2^v$
- **d**: 生成rmat图顶点的平均度数为$2^d$
- **o**: 生成rmat图文件的名称
- **seed1**: 生成rmat图的随机种子1
- **seed2**: 生成rmat图的随机种子2
- **p**: neib的划分个数，一般为1、2或4，根据FPGA kernel的情况而定
- **r**: neib划分的超参数，一般为2或3
- **base_path**: 输出目录路径

## 使用方法
执行gen_data.sh脚本，生成所需的数据文件和元数据

## 文件说明

- **edge_table**: 边表文件
- **neib_table**: 邻居表文件
- **meta_file**: 元数据文件

## 示例

假设我们生成rmat顶点数为$2^8$，顶点平均度数为$2^4$，需要将neib划分为2份，
生成rmat图文件命名为rmat_v8_d4_c2.txt，随机种子设置为0，
划分时的超参数r设置为2，则执行gen_data.sh脚本命令为：
    ```
    ./gen_data.sh 8 4 rmat_v8_d4_c2 0 0 2 2 data/v8_d4_c2
    ```
