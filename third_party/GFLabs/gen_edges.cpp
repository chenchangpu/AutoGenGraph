
#include <iostream>
#include <string>
#include <fstream>

#include "CLI11/CLI11.hpp"

#include "rmat.hpp"

int main(int argc, char const *argv[]) {

    std::string output_path;

    int log_numverts = 16;
    double avg_degree = 4;
    uint64_t userseed1 = 0;
    uint64_t userseed2 = 0;
    int64_t desired_nedges =  0;

    CLI::App app;
    app.add_option("-o,--output_path", output_path, "the output path of the edges")->required();
    app.add_option("-v,--log_numverts", log_numverts, "the log of the number of vertices")->required();
    app.add_option("-e,--desired_nedges", desired_nedges, "the desired number of edges");
    app.add_option("-d,--avg_degree", avg_degree, "the average degree of the graph");
    app.add_option("--seed1", userseed1, "the first seed for the random number generator");
    app.add_option("--seed2", userseed2, "the second seed for the random number generator");
    CLI11_PARSE(app, argc, argv);

    if (desired_nedges == 0) {
        desired_nedges =  static_cast<int64_t>((1LL << log_numverts) * avg_degree);
    }

    std::cout << "output_path: " << output_path << std::endl;
    std::cout << "log_numverts: " << log_numverts << std::endl;
    std::cout << "desired_nedges: " << desired_nedges << std::endl;

    auto edges = make_edges(log_numverts, desired_nedges, userseed1, userseed2);

    std::cout << "nedges: " << edges.size() << std::endl;

    std::ofstream fout(output_path);

    fout << (1LL << log_numverts) << " " << edges.size() << std::endl;
    for (auto& [v1, v2] : edges) {
        fout <<  v1 << " " << v2 << std::endl;
    }

    return 0;
}