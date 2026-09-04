#pragma once

#include "policy.hpp"
#include "resource_manager.hpp"
#include "scheduler_state.hpp"
#include "scheduling_decision.hpp"
#include "workload.hpp"
#include "workload_runtime.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>

class Scheduler {
public:
    Scheduler(
        std::unique_ptr<SchedulingPolicy> policy,
        std::unique_ptr<ResourceManager> resource_manager,
        std::size_t total_cpu_cores,
        std::uint64_t total_memory_mb
    );

    void submit(
        const WorkloadDescriptor& workload
    );

    void register_runtime(
        const WorkloadRuntime& runtime
    );

    SchedulingDecision schedule();

    const SchedulerState& state() const;

private:
    SchedulerState state_;

    std::unique_ptr<SchedulingPolicy> policy_;

    std::unique_ptr<ResourceManager>
        resource_manager_;

    std::unordered_map<
        std::uint64_t,
        WorkloadRuntime
    > runtimes_;
};