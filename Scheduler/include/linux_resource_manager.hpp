#pragma once

#include "cpu_manager.hpp"
#include "resource_manager.hpp"
#include "workload_runtime.hpp"

#include <cstddef>

class LinuxResourceManager : public ResourceManager {
public:
    explicit LinuxResourceManager(
        std::size_t total_cpu_cores
    );

    bool apply(
        const SchedulingDecision& decision,
        const WorkloadDescriptor& workload,
        const WorkloadRuntime& runtime
    ) override;

    void release(
        const WorkloadDescriptor& workload,
        const WorkloadRuntime& runtime
    ) override;

private:
    CpuManager cpu_manager_;
};