
#include <iostream>
#include <string>
#include <fstream>

#include "CLI11/CLI11.hpp"

#include "rmat.hpp"
#include "partition.hpp"

int main(int argc, char const *argv[]) {

    std::string output_path;
    int log_numverts = 16;
    double avg_degree = 4;
    uint64_t userseed1 = 0;
    uint64_t userseed2 = 0;

    int num_processors = 32;
    int rate = 3;

    // |E| = |V|^2 * 10^(-sparisty)
    // int sparisty = 4;

    CLI::App app;
    app.add_option("-o,--output_path", output_path, "the output path of the partitions")->required();
    app.add_option("-v,--log_numverts", log_numverts, "the log of the number of vertices")->required();
    app.add_option("-d,--avg_degree", avg_degree, "the average degree of the graph");
    app.add_option("--seed1", userseed1, "the first seed for the random number generator");
    app.add_option("--seed2", userseed2, "the second seed for the random number generator");
    app.add_option("-p,--num_processors", num_processors, "the number of processors");
    app.add_option("-r,--rate", rate, "the rate of the partitioning");
    CLI11_PARSE(app, argc, argv);

    // int64_t desired_nedges =  static_cast<int64_t>(static_cast<double>(1LL << (log_numverts * 2)) * std::pow(10., -sparisty));
    int64_t desired_nedges =  static_cast<int64_t>((1LL << log_numverts) * avg_degree);

    std::cout << "log_numverts: " << log_numverts << std::endl;
    std::cout << "desired_nedges: " << desired_nedges << std::endl;

    auto edges = make_edges(log_numverts, desired_nedges, userseed1, userseed2);

    std::cout << "nedges: " << edges.size() << std::endl;

    // for (auto& [v1, v2] : edges) {
    //     std::cout <<  v1 << " " << v2 << std::endl;
    // }

    auto row_offset = make_row_offsets(edges, 1LL << log_numverts);
    // for (int i = 0; i < row_offset.size(); i++) {
    //     std::cout << "offset[:" << i << "]:" <<  row_offset[i] << std::endl;
    // }

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