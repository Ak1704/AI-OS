#include "cpu_manager.hpp"

#include <sched.h>

CpuManager::CpuManager(
    std::size_t total_cpu_cores
)
    : total_cpu_cores_(total_cpu_cores)
{
}

bool CpuManager::cpu_id_valid(
    int cpu_id
) const
{
    return
        cpu_id >= 0 &&
        static_cast<std::size_t>(cpu_id)
            < total_cpu_cores_;
}

std::size_t CpuManager::total_cpu_cores() const
{
    return total_cpu_cores_;
}

bool CpuManager::apply_affinity(
    int process_id,
    const std::vector<int>& cpu_ids
) const
{
    if (cpu_ids.empty())
    {
        return false;
    }

    cpu_set_t cpu_set;

    CPU_ZERO(&cpu_set);

    for (int cpu_id : cpu_ids)
    {
        if (!cpu_id_valid(cpu_id))
        {
            return false;
        }

        CPU_SET(
            cpu_id,
            &cpu_set
        );
    }

    return
        sched_setaffinity(
            process_id,
            sizeof(cpu_set_t),
            &cpu_set
        ) == 0;
}