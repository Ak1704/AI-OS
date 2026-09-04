#pragma once

#include <cstddef>
#include <vector>

class CpuManager {
public:
    explicit CpuManager(
        std::size_t total_cpu_cores
    );

    bool apply_affinity(
        int process_id,
        const std::vector<int>& cpu_ids
    ) const;

    bool cpu_id_valid(
        int cpu_id
    ) const;

    std::size_t total_cpu_cores() const;

private:
    std::size_t total_cpu_cores_;
};