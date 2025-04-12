#include <chrono>
#include "graph_converter.hpp"
#include "gen_partitions.hpp"

// Remove suffix
std::string remove_any_suffix(const std::string& filename) {
    size_t pos = filename.rfind('.');
    if (pos != std::string::npos) {
        return filename.substr(0, pos);
    }
    return filename;  // Return original string if no suffix
}

// <main> <base_path_in> <rmat_file> <partition_num> <partition_rate> <base_path_out>
int main(int argc, char **argv){
    if(argc != 6){
        std::cout << "Usage: " << argv[0] << "base_path_in" << "rmat_file" << "partition_num" << "partition_rate" << "base_path_out" << std::endl;
    }
    // argv[1] = "../data/v18_d16_c4/";
    // argv[2] = "v18_d16_c4.bin";
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

    auto start = std::chrono::high_resolution_clock::now();             // Record start time
    std::string suffix = (rmat_file.size() >= 4) ? rmat_file.substr(rmat_file.size() - 4) : "";
    std::cout << "Reading rmat file..." << std::endl;
    if(suffix == ".txt"){
        if(!g.readFromTxtFile(rmat_path)){
            std::cout << "Failed to read rmat file!" << std::endl;
            return 1;
        }
    } else if(suffix == ".bin"){
        if(!g.readFromBinFile(rmat_path)){
            std::cout << "Failed to read rmat file!" << std::endl;
            return 1;
        }
    } else{
        std::cout << "Unsupported rmat file format!" << std::endl;
        return 1;
    }

    std::cout << "Converting edge_table..." << std::endl;
    if(!g.generateEdgeTable()){
        std::cout << "Failed to convert edge_table!" << std::endl;
        return 1;
    }

    std::cout << "Converting neib_table..." << std::endl;
    if(!g.generateNeibTable()){
        std::cout << "Failed to convert neib_table!" << std::endl;
        return 1;
    }

    std::vector<int> p;
    std::cout << "Partitioning neib_table..." << std::endl;
    if(!gen_partiotions(g, p, partition_num, partition_rate)){
        std::cout << "Failed to partition neib_table!" << std::endl;
        return 1;
    }

    // Output bin
    // meta_graph
    std::cout << "Outputting bin files" << std::endl;
    std::string graph_meta_file = "graph_meta_" + remove_any_suffix(rmat_file) + ".bin";
    std::string graph_meta_path = base_path_out + '/' + graph_meta_file;

    std::ofstream graph_meta(graph_meta_path, std::ios::binary);
    if(!graph_meta.is_open()){
        std::cerr << "Cannot open file: " << graph_meta_path << std::endl;
        return 1;
    }
    graph_meta.write(reinterpret_cast<const char*>(&g.num_edges), sizeof(int));
    graph_meta.write(reinterpret_cast<const char*>(p.data()), p.size() * sizeof(int));

    // edge_table
    std::string edge_table_file = "edge_table_" + remove_any_suffix(rmat_file) + ".bin";
    std::string edge_table_path = base_path_out + '/' + edge_table_file;

    std::ofstream edge_table(edge_table_path, std::ios::binary);
    if(!edge_table.is_open()){
        std::cerr << "Cannot open file: " << edge_table_path << std::endl;
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
            std::cerr << "Cannot open file: " << neib_table_path << std::endl;
            return 1;
        }  

        neib_table.write(reinterpret_cast<const char*>(g.neib_table.data() + idx), p[i] * sizeof(Neib)); ///////////////////
        idx += p[i];
    }

    auto end = std::chrono::high_resolution_clock::now();               // Record end time
    std::chrono::duration<double, std::milli> elapsed = end - start;    // Time in milliseconds

    std::cout << "Data processing time: " << elapsed.count() << " ms" << std::endl;

    return 0;
}