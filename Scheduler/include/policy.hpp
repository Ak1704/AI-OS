#pragma once

#include "workload.hpp"
#include <vector>

class SchedulingPolicy {
public:
    virtual ~SchedulingPolicy() = default;

    virtual int select_workload(
        const std::vector<WorkloadDescriptor>& workloads
    ) = 0;
};