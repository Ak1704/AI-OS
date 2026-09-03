#pragma once

#include "ready_queue.hpp"
#include "resource_state.hpp"

#include <cstdint>
#include <optional>

class SchedulerState {
public:
    SchedulerState(
        std::size_t total_cpu_cores,
        std::uint64_t total_memory_mb
    );

    ReadyQueue& ready_queue();

    const ReadyQueue& ready_queue() const;

    ResourceState& resources();

    const ResourceState& resources() const;

    void set_running_workload(
        std::uint64_t workload_id
    );

    void clear_running_workload();

    std::optional<std::uint64_t>
    running_workload() const;

private:
    ReadyQueue ready_queue_;

    ResourceState resource_state_;

    std::optional<std::uint64_t>
        running_workload_id_;
};