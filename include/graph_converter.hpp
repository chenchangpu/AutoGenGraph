#ifndef __GRAPH_CONVERTER__
#define __GRAPH_CONVERTER__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>

// 定义edge结构
struct Edge {
    int src, dst;
};

// 定义neib结构
struct Neib{
    int src_start;
    int src_end;
    int dst_start;
    int dst_end;
};


// 定义图的类
class Graph {
public:
    int num_vertices;
    int num_edges;
    std::vector<Edge> edges;                // rmat的edge对
    std::vector<std::vector<int>> adj_list; // 邻接表
    std::vector<int> edge_table;
    std::vector<Neib> neib_table;

    // 从文件读取rmat图数据
    bool readFromTxtFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "无法打开文件: " << filename << std::endl;
            return false;
        }

        // 读取顶点数和边数
        file >> num_vertices >> num_edges;

        // 初始化邻接表
        adj_list.resize(num_vertices);

        // 读取边
        for(int i=0; i<num_edges; i++){
            int src, dst;
            file >> src >> dst;                 
            if(src >= dst) continue;                    // 假设已经去重 src < dst
            edges.emplace_back(Edge{src, dst});
            adj_list[src].emplace_back(dst);
        }
        num_edges = edges.size();                       // 去重后的边数

        // 对每个顶点的邻接表排序
        for (auto& neighbors : adj_list) {
            std::sort(neighbors.begin(), neighbors.end());
        }

        file.close();
        return true;
    }

    bool readFromBinFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "无法打开文件: " << filename << std::endl;
            return false;
        }

        // 读取顶点数和边数
        file.read(reinterpret_cast<char*>(&num_vertices), sizeof(int));
        file.read(reinterpret_cast<char*>(&num_edges), sizeof(int));

        edges.resize(num_edges);
        file.read(reinterpret_cast<char*>(edges.data()), num_edges * sizeof(Edge));   // bin文件已经去重，直接读入

        // 初始化邻接表
        adj_list.resize(num_vertices);

        // 读取边
        for(auto e: edges){
            int src = e.src, dst = e.dst;
            if(src >= dst) continue;  // src < dst
            adj_list[src].emplace_back(dst);
        }

        // 对每个顶点的邻接表排序
        // for (auto& neighbors : adj_list) {
        //     std::sort(neighbors.begin(), neighbors.end());                             // dahu: sort
        // }

        file.close();
        return true;
    }

    // 生成边表
    bool generateEdgeTable() {
        // 写入edge_table
        for (int v = 0; v < num_vertices; v++) {
            if(!adj_list[v].empty()){
                edge_table.insert(edge_table.end(), adj_list[v].begin(), adj_list[v].end());
            }
        }

        return true;
    }

    // 生成邻接顶点对表
    bool generateNeibTable() {
        // 计算每个顶点在边表中的起始位置
        std::vector<int> vertex_offset(num_vertices + 1, 0);
        for (int i = 0; i < num_vertices; i++) {
            vertex_offset[i + 1] = vertex_offset[i] + adj_list[i].size();
        }

        // 为每条边生成邻接顶点对的offset
        for (const Edge& edge : edges) {
            int src = edge.src;
            int dst = edge.dst;
            
            // 获取源顶点的offset范围
            int src_start = vertex_offset[src];
            int src_end = vertex_offset[src + 1];
            
            // 获取目标顶点的offset范围
            int dst_start = vertex_offset[dst];
            int dst_end = vertex_offset[dst + 1];

            // 判断neib对是否有意义
            if(src_start == src_end || dst_start == dst_end){   // dahu: v3相比v2，去掉无意义neib对，注意需要修改partition
                continue;
            }

            neib_table.emplace_back(Neib{src_start, src_end, dst_start, dst_end});
        }

        return true;
    }
};

#endif