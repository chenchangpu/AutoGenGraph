#include "graph_converter.hpp"
#include "gen_partitions.hpp"

// 去除后缀
std::string remove_any_suffix(const std::string& filename) {
    size_t pos = filename.rfind('.');
    if (pos != std::string::npos) {
        return filename.substr(0, pos);
    }
    return filename;  // 没有后缀就返回原字符串
}

// <main> <base_path_in> <rmat_file> <partition_num> <partition_rate> <base_path_out>
int main(int argc, char **argv){
    if(argc != 6){
        std::cout << "Usage: " << argv[0] << "base_path_in" << "rmat_file" << "partition_num" << "partition_rate" << "base_path_out" << std::endl;
    }
    // argv[1] = "../data/v18_d16_c4/";
    // argv[2] = "v18_d16_c4.txt";
    // argv[3] = "4";
    // argv[4] = "2";
    // argv[5] = "../data/v18_d16_c4/";

    std::string base_path_in = argv[1];
    std::string rmat_file = argv[2];

    std::string rmat_path = base_path_in + '/' + rmat_file;
    int partition_num = std::stoi(argv[3]);
    int partition_rate = std::stoi(argv[4]);
    std::string base_path_out = argv[5];

    Graph g;
    if(!g.readFromFile(rmat_path)){
        std::cout << "读取rmat文件失败！" << std::endl;
    }

    if(!g.generateEdgeTable()){
        std::cout << "转换edge_table失败！" << std::endl;
    }

    if(!g.generateNeibTable()){
        std::cout << "转换neib_table失败！" << std::endl;
    }

    std::vector<int> p;
    if(!gen_partiotions(g, p, partition_num, partition_rate)){
        std::cout << "划分neib_table失败！" << std::endl;
    }

    // 输出bin
    // meta_graph
    std::string graph_meta_file = "graph_meta_" + remove_any_suffix(rmat_file) + ".bin";
    std::string graph_meta_path = base_path_out + '/' + graph_meta_file;

    std::ofstream graph_meta(graph_meta_path, std::ios::binary);
    if(!graph_meta.is_open()){
        std::cerr << "无法打开文件: " << graph_meta_path << std::endl;
        return 1;
    }
    graph_meta.write(reinterpret_cast<const char*>(&g.num_edges), sizeof(int));
    graph_meta.write(reinterpret_cast<const char*>(p.data()), p.size() * sizeof(int));

    // edge_table
    std::string edge_table_file = "edge_table_" + remove_any_suffix(rmat_file) + ".bin";
    std::string edge_table_path = base_path_out + '/' + edge_table_file;

    std::ofstream edge_table(edge_table_path, std::ios::binary);
    if(!edge_table.is_open()){
        std::cerr << "无法打开文件: " << edge_table_path << std::endl;
        return 1;
    }
    edge_table.write(reinterpret_cast<const char*>(g.edge_table.data()), g.edge_table.size() * sizeof(int));

    // neib_table
    int idx = 0;
    #pragma unroll
    for(int i = 0; i < partition_num; ++i){
        std::string neib_table_file = "neib_table_" + remove_any_suffix(rmat_file) + "_" + std::to_string(i+1) + ".bin";
        std::string neib_table_path = base_path_out + '/' + neib_table_file;

        std::ofstream neib_table(neib_table_path, std::ios::binary);
        if(!neib_table.is_open()){
            std::cerr << "无法打开文件: " << neib_table_path << std::endl;
            return 1;
        }  

        neib_table.write(reinterpret_cast<const char*>(g.neib_table.data() + idx), p[i] * sizeof(Neib)); ///////////////////
        idx += p[i];
    }

    return 0;
}