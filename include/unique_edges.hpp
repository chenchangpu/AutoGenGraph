#ifndef __UNIQUE_EDGES__
#define __UNIQUE_EDGES__

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <set>


/*
输入：未去重的rmat txt文件
输出：去重的rmat txt文件
*/
void unique_edges(std::string infile_path, std::string outfile_path){
    std::ifstream infile(infile_path, std::ios::in);
    if(!infile){
        std::cout << "无法打开输入文件：" << infile_path << std::endl;
        exit(1);  // 添加退出
    }

    // 先读取所有数据
    int num_v, num_e;
    int self_loop = 0;
    infile >> num_v >> num_e;
    std::set<std::pair<int, int>> ues;
    for(int i = 0; i < num_e; ++i){
        int x, y;
        infile >> x >> y;
        if(x > y){
            std::swap(x, y);  // x <= y
        }
        if(x == y){
            ++self_loop;
            continue;
        }
        ues.insert(std::pair<int, int>(x, y));
    }
    infile.close();  // 关闭输入文件

    // 然后再写入
    std::ofstream outfile(outfile_path, std::ios::out);
    if(!outfile){
        std::cout << "无法打开输出文件：" << outfile_path << std::endl;
        exit(1);  // 添加退出
    }

    int cnt = ues.size();
    printf("自环数量：%d\n", self_loop);
    printf("重边数量：%d\n", num_e - cnt - self_loop);

    outfile << num_v << " " << cnt << std::endl;
    for(auto e: ues){
        outfile << e.first << " " << e.second << std::endl;
    }
    outfile.close();
}



#endif