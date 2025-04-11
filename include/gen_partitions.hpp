#ifndef __GEN_PARTITIONS__
#define __GEN_PARTITIONS__

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <vector>
#include <limits>
#include "graph_converter.hpp"

struct Partition {
    int target_cost_per_processor;
    int max_cost_per_processor;
    int min_cost_per_processor;
    // 每个分区的起始位置
    std::vector<int> partition_starts;
    // 每个分区的实际负载
    std::vector<int> partition_loads;

    Partition(int target_cost_per_processor) : 
        target_cost_per_processor(target_cost_per_processor),
        max_cost_per_processor(0LL),
        min_cost_per_processor(std::numeric_limits<int>::max()) {}
};


Partition make_partitions(const std::vector<Edge>& edges, 
                          const std::vector<int>& row_offsets, 
                          int num_processors,
                          int rate = 2) {
    int num_edges = edges.size();

    auto get_cost = [&](const int& src, const int& dst) {
        int src_outdegree = row_offsets[src + 1] - row_offsets[src];
        int dst_outdegree = row_offsets[dst + 1] - row_offsets[dst];
        return src_outdegree + dst_outdegree;
    };


    int total_cost = 0;
    for(const auto& edge : edges) {
        total_cost += get_cost(edge.src, edge.dst);
    }
    int avg_cost = (total_cost + num_edges - 1) / num_edges;
    
    int target_cost_per_processor = (total_cost + num_processors - 1) / num_processors;
    

    Partition result(target_cost_per_processor);
    result.partition_starts.push_back(0);

    auto update_partition = [](Partition& part, int start, int load) {
        part.partition_starts.push_back(start);
        part.partition_loads.push_back(load);
        part.max_cost_per_processor = std::max(part.max_cost_per_processor, load);
        part.min_cost_per_processor = std::min(part.min_cost_per_processor, load);
    };


    {
        int current_sum = 0;
        int current_processor = 0;    
        for(int i = 0; i < num_edges; ++i) {
            int edge_cost = get_cost(edges[i].src, edges[i].dst);
            current_sum += edge_cost;
            
            if(current_processor < num_processors - 1 && 
                current_sum > target_cost_per_processor - avg_cost * rate) {
                update_partition(result, i + 1, current_sum);
                
                current_sum = 0;
                ++current_processor;
            }

        }

        update_partition(result, num_edges, current_sum);
    }

    return result;
}

bool gen_partiotions(const Graph &g, std::vector<int> &p, int num_processors, int rate){
    if(!p.empty()){
        p.clear();
    }

    std::vector<int> row_offset(g.num_vertices, 0);;
    for(int i = 1; i <= g.num_vertices + 1; ++i) {
        row_offset[i] += row_offset[i-1] + g.adj_list[i].size();
    }

    Partition partitons = make_partitions(g.edges, row_offset, num_processors, rate);

    std::vector<int> tmp_p;
    for (auto x: partitons.partition_starts) {
        tmp_p.emplace_back(x);
    }

    // 修正最后一个元素
    if(tmp_p.back() > g.neib_table.size()){
        tmp_p.back() = g.neib_table.size();
    }

    for(int i = 1; i <= num_processors; ++i){
        if(tmp_p[i] - tmp_p[i-1] < 0){
            std::cout << "neib划分子集小于0，不符合要求！" << std::endl;
            return false;
        }
        p.emplace_back(tmp_p[i] - tmp_p[i-1]);
    }

    return true;
}

#endif