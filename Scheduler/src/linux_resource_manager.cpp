#include "linux_resource_manager.hpp"

LinuxResourceManager::LinuxResourceManager(
    std::size_t total_cpu_cores
)
    : cpu_manager_(total_cpu_cores)
{
}

bool LinuxResourceManager::apply(
    const SchedulingDecision& decision,
    const WorkloadDescriptor& workload,
    const WorkloadRuntime& runtime
)
{
    if (decision.action != SchedulingAction::RUN)
    {
        return false;
    }

    if (decision.workload_id != workload.id)
    {
        return false;
    }

    if (workload.cpu_cores_required <= 0)
    {
        return false;
    }

    return cpu_manager_.apply_affinity(
        runtime.process_id,
        decision.cpu_cores
    );
}

void LinuxResourceManager::release(
    const WorkloadDescriptor& workload,
    const WorkloadRuntime& runtime
)
{
    (void)workload;
    (void)runtime;
}