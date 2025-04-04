#pragma once 

#include <cstdint>
#include <vector>
#include <limits>


struct Partition {
    int64_t target_cost_per_processor;
    int64_t max_cost_per_processor;
    int64_t min_cost_per_processor;
    // 每个分区的起始位置
    std::vector<int64_t> partition_starts;
    // 每个分区的实际负载
    std::vector<int64_t> partition_loads;

    Partition(int target_cost_per_processor) : 
        target_cost_per_processor(target_cost_per_processor),
        max_cost_per_processor(0LL),
        min_cost_per_processor(std::numeric_limits<int64_t>::max()) {}
};


template<typename T>
Partition make_partitions(const std::vector<std::pair<T,T>>& edges, 
                          const std::vector<int64_t>& row_offsets, 
                          int num_processors,
                          int rate = 2) {
    int num_edges = edges.size();

    auto get_cost = [&](const T& src, const T& dst) {
        int src_outdegree = row_offsets[src + 1] - row_offsets[src];
        int dst_outdegree = row_offsets[dst + 1] - row_offsets[dst];
        return src_outdegree + dst_outdegree;
    };


    int64_t total_cost = 0;
    for(const auto& edge : edges) {
        total_cost += get_cost(edge.first, edge.second);
    }
    int64_t avg_cost = (total_cost + num_edges - 1) / num_edges;
    
    int64_t target_cost_per_processor = (total_cost + num_processors - 1) / num_processors;
    

    Partition result(target_cost_per_processor);
    result.partition_starts.push_back(0);

    auto update_partition = [](Partition& part, int64_t start, int64_t load) {
        part.partition_starts.push_back(start);
        part.partition_loads.push_back(load);
        part.max_cost_per_processor = std::max(part.max_cost_per_processor, load);
        part.min_cost_per_processor = std::min(part.min_cost_per_processor, load);
    };


    {
        int64_t current_sum = 0;
        int current_processor = 0;    
        for(int i = 0; i < num_edges; ++i) {
            int edge_cost = get_cost(edges[i].first, edges[i].second);
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
