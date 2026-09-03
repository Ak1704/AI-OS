#include "priority_policy.hpp"

int PriorityPolicy::select_workload(
    const std::vector<WorkloadDescriptor>& workloads
)
{
    int selected = -1;
    int highest_priority = -1;

    for (int i = 0;
         i < static_cast<int>(workloads.size());
         ++i)
    {
        if (workloads[i].state !=
            WorkloadState::READY)
        {
            continue;
        }

        if (workloads[i].priority >
            highest_priority)
        {
            highest_priority =
                workloads[i].priority;

            selected = i;
        }
    }

    return selected;
}