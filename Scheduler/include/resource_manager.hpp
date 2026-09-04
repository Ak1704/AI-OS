#pragma once

#include "scheduling_decision.hpp"
#include "workload.hpp"
#include "workload_runtime.hpp"

#include <cstdint>

class ResourceManager {
public:
    virtual ~ResourceManager() = default;

    virtual bool apply(
        const SchedulingDecision& decision,
        const WorkloadDescriptor& workload,
        const WorkloadRuntime& runtime
    ) = 0;

    virtual void release(
        const WorkloadDescriptor& workload,
        const WorkloadRuntime& runtime
    ) = 0;
};