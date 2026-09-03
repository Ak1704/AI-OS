#include "ready_queue.hpp"

#include <algorithm>

void ReadyQueue::push(
    const WorkloadDescriptor& workload
)
{
    workloads_.push_back(workload);
}

bool ReadyQueue::empty() const
{
    return workloads_.empty();
}

std::size_t ReadyQueue::size() const
{
    return workloads_.size();
}

const std::vector<WorkloadDescriptor>&
ReadyQueue::workloads() const
{
    return workloads_;
}

void ReadyQueue::remove(
    std::uint64_t workload_id
)
{
    workloads_.erase(
        std::remove_if(
            workloads_.begin(),
            workloads_.end(),
            [workload_id](const WorkloadDescriptor& workload) {
                return workload.id == workload_id;
            }
        ),
        workloads_.end()
    );
}

void ReadyQueue::clear()
{
    workloads_.clear();
}