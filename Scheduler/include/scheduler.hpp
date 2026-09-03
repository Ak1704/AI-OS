#pragma once

#include "policy.hpp"
#include "scheduler_state.hpp"
#include "scheduling_decision.hpp"
#include "workload.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>

class Scheduler {
public:
    Scheduler(
        std::unique_ptr<SchedulingPolicy> policy,
        std::size_t total_cpu_cores,
        std::uint64_t total_memory_mb
    );

    void submit(
        const WorkloadDescriptor& workload
    );

    SchedulingDecision schedule();

    const SchedulerState& state() const;

private:
    SchedulerState state_;

    std::unique_ptr<SchedulingPolicy> policy_;
};