#include "resource_state.hpp"

#include <algorithm>

ResourceState::ResourceState(
    std::size_t total_cpu_cores,
    std::uint64_t total_memory_mb
)
    : total_cpu_cores_(total_cpu_cores),
      available_cpu_cores_(total_cpu_cores),
      total_memory_mb_(total_memory_mb),
      available_memory_mb_(total_memory_mb)
{
    for (std::size_t i = 0;
         i < total_cpu_cores_;
         ++i)
    {
        available_cpu_ids_.push_back(
            static_cast<int>(i)
        );
    }
}

bool ResourceState::can_allocate(
    const WorkloadDescriptor& workload
) const
{
    return
        workload.cpu_cores_required <=
            static_cast<int>(available_cpu_cores_)
        &&
        workload.memory_mb_required <=
            available_memory_mb_;
}

bool ResourceState::allocate(
    const WorkloadDescriptor& workload
)
{
    if (!can_allocate(workload))
        return false;

    available_cpu_cores_ -=
        workload.cpu_cores_required;

    available_memory_mb_ -=
        workload.memory_mb_required;

    available_cpu_ids_.erase(
        available_cpu_ids_.begin(),
        available_cpu_ids_.begin()
            + workload.cpu_cores_required
    );

    return true;
}

void ResourceState::release(
    const WorkloadDescriptor& workload
)
{
    available_cpu_cores_ +=
        workload.cpu_cores_required;

    available_memory_mb_ +=
        workload.memory_mb_required;

    for (int i = 0;
         i < workload.cpu_cores_required;
         ++i)
    {
        available_cpu_ids_.push_back(
            static_cast<int>(
                total_cpu_cores_
                - available_cpu_ids_.size()
                - 1
            )
        );
    }
}

std::size_t ResourceState::total_cpu_cores() const
{
    return total_cpu_cores_;
}

std::size_t ResourceState::available_cpu_cores() const
{
    return available_cpu_cores_;
}

std::uint64_t ResourceState::total_memory_mb() const
{
    return total_memory_mb_;
}

std::uint64_t ResourceState::available_memory_mb() const
{
    return available_memory_mb_;
}

const std::vector<int>&
ResourceState::available_cpu_ids() const
{
    return available_cpu_ids_;
}