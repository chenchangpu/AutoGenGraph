/*
v3相比v2，去掉无意义neib对，注意需要修改partition
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>

// 定义边的结构
struct Edge {
    int src, dst;
    Edge(int s, int d) : src(s), dst(d) {}
};

// 定义图的类
class Graph {
private:
    int num_vertices;
    int num_edges;
    std::vector<Edge> edges;
    std::vector<std::vector<int>> adj_list; // 邻接表

public:
    // 从文件读取图数据
    bool readFromFile(const std::string& filename) {
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
        for(int i=0;i<num_edges;i++){
            int src, dst;
            file >> src >> dst;                 // 注意rmat已经去重，所以 src < dst
            edges.emplace_back(src, dst);
            adj_list[src].push_back(dst);
        }

        // 对每个顶点的邻接表排序
        for (auto& neighbors : adj_list) {
            std::sort(neighbors.begin(), neighbors.end());
        }

        file.close();
        return true;
    }

    // 生成边表
    bool generateEdgeTable(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "无法创建文件: " << filename << std::endl;
            return false;
        }

        // 写入边表
        for (int v = 0; v < num_vertices; v++) {
            for (int neighbor : adj_list[v]) {
                file << neighbor << std::endl;
            }
        }

        file.close();
        return true;
    }

    // 生成邻接顶点对表
    bool generateNeibTable(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "无法创建文件: " << filename << std::endl;
            return false;
        }

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

            // 写入一行邻接顶点对的offset
            file << src_start << " " << src_end << " " 
                 << dst_start << " " << dst_end << std::endl;  //dahu: 
        }

        file.close();
        return true;
    }
};


int main(int argc, char ** argv) {
    if(argc != 4){
        std::cout << "Usage: " << argv[0] << "input_rmat_file.txt" << "output_edge_table.txt" 
                    << "output_neib_table.txt" << std::endl;
    }

    Graph graph;

    if (!graph.readFromFile(argv[1])) {
        return 1;
    }

    if (!graph.generateEdgeTable(argv[2])) {
        return 1;
    }

    if (!graph.generateNeibTable(argv[3])) {
        return 1;
    }

    std::cout << "数据结构转换完成！" << std::endl;
    return 0;
}