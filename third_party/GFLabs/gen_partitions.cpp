
#include <iostream>
#include <string>
#include <fstream>

#include "CLI11/CLI11.hpp"

#include "rmat.hpp"
#include "partition.hpp"




int main(int argc, char const *argv[]) {

    std::string output_path;
    int log_numverts = 0;
    double avg_degree = 4;
    bool is_undirected = false;
    uint64_t userseed1 = 0;
    uint64_t userseed2 = 0;
    float init_a = 0.57f, init_b = 0.19f, init_c = 0.19f;

    int num_partitions = 32;
    int rate = 3;

    CLI::App app;
    app.add_option("-o,--output_path", output_path, "the output path of the partitions")->required();
    app.add_option("-v,--log_numverts", log_numverts, "the log of the number of vertices")->required();
    app.add_option("-d,--avg_degree", avg_degree, "the average degree of the graph (default 4)");
    app.add_flag("-u,--undirect,!--directed", is_undirected, "whether the graph is undirected or directed (default directed)");
    app.add_option("--seed1", userseed1, "the first seed for the random number generator (default 0)");
    app.add_option("--seed2", userseed2, "the second seed for the random number generator (default 0)");
    app.add_option("--init_a", init_a, "the parameter A in generator (default 0.57)");
    app.add_option("--init_b", init_b, "the parameter B in generator (default 0.19)");
    app.add_option("--init_c", init_c, "the parameter C in generator (default 0.19)");
    app.add_option("-p,--num_partitions", num_partitions, "the number of partitions (default 32)");
    app.add_option("-r,--rate", rate, "the rate of the partitioning (default 3)");
    CLI11_PARSE(app, argc, argv);

    // int64_t desired_nedges =  static_cast<int64_t>(static_cast<double>(1LL << (log_numverts * 2)) * std::pow(10., -sparisty));
    int64_t desired_nedges =  static_cast<int64_t>((1LL << log_numverts) * avg_degree);

    std::cout << "log_numverts: " << log_numverts << std::endl;
    std::cout << "desired_nedges: " << desired_nedges << std::endl;

    std::vector<std::pair<int64_t, int64_t>> edges = make_edges(
        log_numverts, desired_nedges, 
        userseed1, userseed2, init_a, init_b, init_c,
        is_undirected, true, false
    );

    std::cout << "nedges: " << edges.size() << std::endl;

    auto make_row_offsets = [](const auto& edges, int64_t n) {
        std::vector<int64_t> row_offset(n + 1, 0);
        
        for(const auto& edge : edges) {
            ++row_offset[edge.first + 1];
        }
        
        for(int i = 1; i <= n; i++) {
            row_offset[i] += row_offset[i-1];
        }
        
        return row_offset;
    };
    auto row_offsets = make_row_offsets(edges, 1LL << log_numverts);
    
    // the method to calculate the cost of the edge
    auto get_cost = [&](const auto& src, const auto& dst) {
        auto src_outdegree = row_offsets[src + 1] - row_offsets[src];
        auto dst_outdegree = row_offsets[dst + 1] - row_offsets[dst];
        return src_outdegree + dst_outdegree;
    };

    auto partitons = make_partitions(edges, get_cost, num_partitions, rate);

    std::cout << "target_cost_per_processor: " << partitons.target_cost_per_processor << std::endl;
    std::cout << "max_cost_per_processor: " << partitons.max_cost_per_processor << std::endl;
    std::cout << "min_cost_per_processor: " << partitons.min_cost_per_processor << std::endl;
    std::cout << "(max_cost - min_cost) / avg_cost: " 
        << 100.0 * (partitons.max_cost_per_processor - partitons.min_cost_per_processor) / partitons.target_cost_per_processor 
        << "%" << std::endl;
    for (int i = 0; i < num_partitions; ++i) {
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