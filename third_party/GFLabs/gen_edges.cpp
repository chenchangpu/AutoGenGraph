
#include <iostream>
#include <string>
#include <fstream>
#include <set>
#include <vector>
#include <chrono>

#include "CLI11/CLI11.hpp"

#include "rmat.hpp"

using namespace std::chrono;

template<typename EdgeContainer>
void write_edges(const std::string& output_path, const EdgeContainer& edges, 
                 int64_t numverts, bool is_undirected) {
    std::ofstream fout(output_path);

    bool is_mtx = output_path.substr(output_path.size() - 4) == ".mtx";
    bool is_txt = output_path.substr(output_path.size() - 4) == ".txt";
    bool is_bin = output_path.substr(output_path.size() - 4) == ".bin";

    if (!fout.is_open()) {
        throw std::runtime_error("Failed to open file: " + output_path);
    }
    if (is_mtx) {   // matrix makert is 1-based
        fout << "%%MatrixMarket matrix coordinate pattern " << (is_undirected ? "symmetric" : "general") << "\n";
        fout << numverts << " " << numverts << " " << edges.size() << std::endl;
        for (const auto& [v1, v2] : edges) {
            fout <<  v1 + 1 << " " << v2 + 1 << std::endl;
        }
    } else if(is_txt) {
        fout << numverts << " " << edges.size() << std::endl;
        for (const auto& [v1, v2] : edges) {
            fout <<  v1 << " " << v2 << std::endl;
        }
    }
    else {
        std::ofstream fout(output_path, std::ios::binary);
        if (!fout.is_open()) {
            throw std::runtime_error("Failed to open binary file: " + output_path);
        }
        uint32_t num_vert = (uint32_t)(numverts);
        uint32_t edge_size = edges.size();
        
        fout.write(reinterpret_cast<const char*>(&num_vert), sizeof(int));                  
        fout.write(reinterpret_cast<const char*>(&edge_size), sizeof(int));
        fout.write(reinterpret_cast<const char*>(edges.data()), (uint64_t)edge_size * sizeof(EdgeContainer));

        fout.close();
    }

    fout.close();
    std::cout << "Successfully write to file: " << output_path << std::endl;
}

int main(int argc, char const *argv[]) {

    std::string output_path;

    int log_numverts = 0;
    double avg_degree = 4;
    bool is_undirected = false;
    uint64_t userseed1 = 0;
    uint64_t userseed2 = 0;
    int64_t desired_nedges =  0;
    float init_a = 0.57f, init_b = 0.19f, init_c = 0.19f;

    CLI::App app;
    app.add_option("-o,--output_path", output_path, "the output path of the edges")->required();
    app.add_option("-v,--log_numverts", log_numverts, "the log of the number of vertices")->required();
    app.add_option("-e,--desired_nedges", desired_nedges, "the desired number of edges");
    app.add_option("-d,--avg_degree", avg_degree, "the average degree of the graph (default 4)");
    app.add_flag("-u,--undirect,!--directed", is_undirected, "whether the graph is undirected or directed (default directed)");
    app.add_option("--seed1", userseed1, "the first seed for the random number generator (default 0)");
    app.add_option("--seed2", userseed2, "the second seed for the random number generator (default 0)");
    app.add_option("-a,--init_a", init_a, "the parameter A in generator (default 0.57)");
    app.add_option("-b,--init_b", init_b, "the parameter B in generator (default 0.19)");
    app.add_option("-c,--init_c", init_c, "the parameter C in generator (default 0.19)");
    CLI11_PARSE(app, argc, argv);

    if (desired_nedges == 0) {
        desired_nedges =  static_cast<int64_t>((1LL << log_numverts) * avg_degree);
    }

    if ((init_a < 0.f || init_b > 1.f) || (init_b < 0.f || init_b > 1.f) 
        || (init_c < 0.f || init_c > 1.f)) {
        std::cout << "init_a or init_b or init_c should a float between 0.0 ~ 1.0\n";
        return 1;
    }
    if (init_a + init_b + init_c > 1.f) {
        std::cout << "init_a + init_b + init_c should smaller than 1\n";
        return 2;
    }

    auto t_start = high_resolution_clock::now();

    // std::vector<std::pair<int64_t, int64_t>> edges = make_edges(
    //     log_numverts, desired_nedges, 
    //     userseed1, userseed2, init_a, init_b, init_c, 
    //     is_undirected
    // );

    std::vector<std::pair<int, int>> edges = make_edges<int>(               // dahu: overflow
        log_numverts, desired_nedges, 
        userseed1, userseed2, init_a, init_b, init_c, 
        is_undirected
    );
    std::cout << "generated edges: " << edges.size() << std::endl;

    std::cout << "writing edges to file: " << output_path << std::endl;
    write_edges(output_path, edges, (1LL << log_numverts), is_undirected);

    auto t_stop = high_resolution_clock::now();
    auto elapsed = duration_cast<microseconds>(t_stop - t_start).count();

    std::cout << "Elapsed time: " << elapsed / 1e3 << " ms" << std::endl;

    return 0;
}