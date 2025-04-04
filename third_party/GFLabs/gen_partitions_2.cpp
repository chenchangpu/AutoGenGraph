
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "CLI11/CLI11.hpp"

#include "rmat.hpp"
#include "partition.hpp"

int main(int argc, char const *argv[]) {

    std::string output_path;
    std::string input_path;

    int num_processors = 10;
    int rate = 3;

    // |E| = |V|^2 * 10^(-sparisty)
    // int sparisty = 4;

    CLI::App app;
    app.add_option("-i,--input_path", input_path, "the intput path of the rmat file")->required();
    app.add_option("-o,--output_path", output_path, "the output path of the partitions")->required();
    app.add_option("-p,--num_processors", num_processors, "the number of processors");
    app.add_option("-r,--rate", rate, "the rate of the partitioning");
    
    CLI11_PARSE(app, argc, argv);

    std::ifstream rmat_file(input_path);
    if(!rmat_file){
        std::cout << "Can not open file: " << input_path << std::endl;
    }

    int64_t num_v, num_e;
    rmat_file >> num_v >> num_e;
    std::cout << "number of vertices: " << num_v << std::endl;
    std::cout << "number of edges: " << num_e << std::endl;

    std::vector<std::pair<int64_t, int64_t>> edges;
    edges.resize(num_e);
    for(int i = 0; i < num_e; ++i){
        int64_t src, dst;
        rmat_file >> src >> dst;
        edges[i] = std::make_pair(src, dst);
    }

    auto row_offset = make_row_offsets(edges, num_v);

    Partition partitons = make_partitions(edges, row_offset, num_processors, rate);

    std::cout << "target_cost_per_processor: " << partitons.target_cost_per_processor << std::endl;
    std::cout << "max_cost_per_processor: " << partitons.max_cost_per_processor << std::endl;
    std::cout << "min_cost_per_processor: " << partitons.min_cost_per_processor << std::endl;
    std::cout << "(max_cost - min_cost) / avg_cost: " 
        << 100.0 * (partitons.max_cost_per_processor - partitons.min_cost_per_processor) / partitons.target_cost_per_processor 
        << "%" << std::endl;
    for (int i = 0; i < num_processors; i++) {
        std::cout << "partition[" << i << "]: "
                  << "start: " << partitons.partition_starts[i] << "  "
                  << "load: " << partitons.partition_loads[i] << std::endl;
    }

    if (!output_path.empty()) {
        std::cout << "save to output_path: " << output_path << std::endl;
        std::ofstream fout(output_path);

        for (int64_t p: partitons.partition_starts) {
            fout << p << std::endl;
        }
    }

    return 0;
}