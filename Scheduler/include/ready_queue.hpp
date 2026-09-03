#pragma once

#include "workload.hpp"
#include "cstddef"
#include <vector>

class ReadyQueue{
public:
    void push(const WorkloadDescriptor& workload);
    bool empty() const;

    std::size_t size() const;

    const std::vector<WorkloadDescriptor>& workloads() const;
    void remove(std::uint64_t workload_id);
    void clear();
private:
    std::vector<WorkloadDescriptor> workloads_;
};