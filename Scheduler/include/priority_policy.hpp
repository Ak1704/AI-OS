#pragma once

#include "policy.hpp"

class PriorityPolicy : public SchedulingPolicy {
public:
    int select_workload(
        const std::vector<WorkloadDescriptor>& workloads
    ) override;
};