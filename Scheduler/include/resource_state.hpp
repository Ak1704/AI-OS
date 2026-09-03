#pragma once

#include "workload.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

class ResourceState {
public:
    ResourceState(
        std::size_t total_cpu_cores,
        std::uint64_t total_memory_mb
    );

    bool can_allocate(
        const WorkloadDescriptor& workload
    ) const;

    bool allocate(
        const WorkloadDescriptor& workload
    );

    void release(
        const WorkloadDescriptor& workload
    );

    std::size_t total_cpu_cores() const;

    std::size_t available_cpu_cores() const;

    std::uint64_t total_memory_mb() const;

    std::uint64_t available_memory_mb() const;

    const std::vector<int>& available_cpu_ids() const;

private:
    std::size_t total_cpu_cores_;

    std::size_t available_cpu_cores_;

    std::uint64_t total_memory_mb_;

    std::uint64_t available_memory_mb_;

    std::vector<int> available_cpu_ids_;
};