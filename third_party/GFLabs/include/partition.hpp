#pragma once 

#include <cstdint>
#include <vector>
#include <limits>
#include <type_traits>

template <typename T>
struct Partition {
    int64_t target_cost_per_processor;
    T max_cost_per_processor;
    T min_cost_per_processor;
    // 每个分区的起始位置
    std::vector<int64_t> partition_starts;
    // 每个分区的实际负载
    std::vector<T> partition_loads;

    Partition(int target_cost_per_processor) : 
        target_cost_per_processor(target_cost_per_processor),
        max_cost_per_processor(0),
        min_cost_per_processor(std::numeric_limits<T>::max()) {}
};


template<typename T, typename COST_FUNC>
auto make_partitions(const std::vector<std::pair<T,T>>& edges, 
                     COST_FUNC edge_cost_func, // edge_cost_func
                     int num_processors,
                     int rate = 2) {
    using cost_t = std::invoke_result_t<COST_FUNC, T, T>;

    int num_edges = edges.size();

    cost_t total_cost = 0;
    for(const auto& edge : edges) {
        total_cost += edge_cost_func(edge.first, edge.second);
    }
    cost_t avg_cost = (total_cost + num_edges - 1) / num_edges;
    cost_t target_cost_per_processor = (total_cost + num_processors - 1) / num_processors;

    Partition<cost_t> result(target_cost_per_processor);
    result.partition_starts.push_back(0);

    auto update_partition = [](Partition<cost_t>& part, int64_t start, cost_t load) {
        part.partition_starts.push_back(start);
        part.partition_loads.push_back(load);
        part.max_cost_per_processor = std::max(part.max_cost_per_processor, load);
        part.min_cost_per_processor = std::min(part.min_cost_per_processor, load);
    };


    {
        cost_t current_sum = 0;
        int current_processor = 0;    
        for(int i = 0; i < num_edges; ++i) {
            cost_t edge_cost = edge_cost_func(edges[i].first, edges[i].second);
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
