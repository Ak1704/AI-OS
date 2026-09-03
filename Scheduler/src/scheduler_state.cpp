#include "scheduler_state.hpp"

SchedulerState::SchedulerState(
    std::size_t total_cpu_cores,
    std::uint64_t total_memory_mb
)
    : resource_state_(
          total_cpu_cores,
          total_memory_mb
      )
{
}

ReadyQueue&
SchedulerState::ready_queue()
{
    return ready_queue_;
}

const ReadyQueue&
SchedulerState::ready_queue() const
{
    return ready_queue_;
}

ResourceState&
SchedulerState::resources()
{
    return resource_state_;
}

const ResourceState&
SchedulerState::resources() const
{
    return resource_state_;
}

void SchedulerState::set_running_workload(
    std::uint64_t workload_id
)
{
    running_workload_id_ = workload_id;
}

void SchedulerState::clear_running_workload()
{
    running_workload_id_.reset();
}

std::optional<std::uint64_t>
SchedulerState::running_workload() const
{
    return running_workload_id_;
}