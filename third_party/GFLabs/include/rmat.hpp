#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>

#include "rmat_generator/make_graph.h"

template <typename T = int64_t>
std::vector<std::pair<T,T>> make_edges(
    int log_numverts, 
    int64_t desired_nedges, 
    uint64_t userseed1 = 0ULL, 
    uint64_t userseed2 = 0ULL,
    bool sorted = true
) {
    int64_t nedges;
    packed_edge* result;

    make_graph(log_numverts, desired_nedges, userseed1, userseed2, &nedges, &result);

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
    for (int i = 0; i < nedges; i++) {
        T v1 = get_vid(get_v0_from_edge(&result[i]));
        T v2 = get_vid(get_v1_from_edge(&result[i]));
        edges.emplace_back(v1, v2);
    }

    if (sorted) {
        std::sort(edges.begin(), edges.end());
    }

    free(result);

    return edges;
}


template<typename T>
std::vector<int64_t> make_row_offsets(const std::vector<std::pair<T,T>>& edges, int64_t n) {
    std::vector<int64_t> row_offset(n + 1, 0);
    
    for(const auto& edge : edges) {
        row_offset[edge.first + 1]++;
    }
    
    for(int i = 1; i <= n; i++) {
        row_offset[i] += row_offset[i-1];
    }
    
    return row_offset;
}