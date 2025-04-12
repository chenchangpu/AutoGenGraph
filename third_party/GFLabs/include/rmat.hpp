#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <iostream>

#include "rmat_generator/make_graph.h"

template <typename T = int64_t>
std::vector<std::pair<T,T>> make_edges(
    int log_numverts, 
    int64_t desired_nedges, 
    uint64_t userseed1 = 0ULL, 
    uint64_t userseed2 = 0ULL,
#ifdef CUSTOM_INITIATOR
    float initiator_a = 0.57f,
    float initiator_b = 0.19f,
    float initiator_c = 0.19f,
#endif
    bool undirected = false,
    bool sorted = true,
    bool do_print = true
) {
    int64_t nedges;
    packed_edge* result;

    if (do_print) {
        std::cout << "log_numverts: " << log_numverts << std::endl;
        std::cout << "desired_nedges: " << desired_nedges << std::endl;
        std::cout << "seed1: " << userseed1 << ", seed2: " << userseed2 << std::endl;
        std::cout << "undirected: " << undirected << ", sorted: " << sorted << std::endl;
    }
#ifdef CUSTOM_INITIATOR
    assert(initiator_a > 0.f && initiator_a < 1.f);
    assert(initiator_b > 0.f && initiator_b < 1.f);
    assert(initiator_c > 0.f && initiator_c < 1.f);
    assert(initiator_a + initiator_b + initiator_c < 1.f);
    if (do_print) {
        std::cout << "init_a: " << initiator_a << ", init_b: " << initiator_b << ", init_c: " << initiator_c << std::endl;
    }

    make_graph(log_numverts, desired_nedges, initiator_a, initiator_b, initiator_c, userseed1, userseed2, &nedges, &result);
#else
    make_graph(log_numverts, desired_nedges, userseed1, userseed2, &nedges, &result);
#endif

    // std::unordered_map<int64_t, T> vertex_map;
    // T vid = 0;
    // auto get_vid = [&vertex_map, &vid](int64_t v) {
    //     if (auto it = vertex_map.find(v); it != vertex_map.end()) {
    //         return it->second;
    //     }
    //     return vertex_map[v] = vid++;;
    // };

    auto get_vid = [](int64_t v) {
        return static_cast<T>(v);
    };

    std::vector<std::pair<T, T>> edges;
    edges.reserve(nedges);

    for (int i = 0; i < nedges; i++) {
        T v1 = get_vid(get_v0_from_edge(&result[i]));
        T v2 = get_vid(get_v1_from_edge(&result[i]));
        edges.emplace_back(v1, v2);
    }
    free(result);

    if (undirected) {
        if (do_print) {
            std::cout << "changing to undirected edges..." << std::endl;
        }

        auto new_end = std::remove_if(edges.begin(), edges.end(), [](auto& edge) {
            auto& [v1, v2] = edge;
            if (v1 > v2) {
                std::swap(v1, v2);
            }
            return edge.first == edge.second;
        });
        auto self_loops = std::distance(new_end, edges.end());
        edges.erase(new_end, edges.end());
        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        auto repeat_edges = std::distance(last, edges.end());
        edges.erase(last, edges.end());

        if (do_print) {
            std::cout << "self loops: " << self_loops << std::endl;
            std::cout << "repeated edges: " << repeat_edges << std::endl;
        }
    } else if (sorted) {
        std::sort(edges.begin(), edges.end());
    }


    return edges;
}
