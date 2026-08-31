#pragma once

#include "policy.hpp"
#include "scheduling_decision.hpp"
#include "workload.hpp"

#include <memory>
#include <vector>

class Scheduler {
public:
    explicit Scheduler(std::unique_ptr<SchedulingPolicy> policy);

    void submit(const WorkloadDescriptor& workload);

    SchedulingDecision schedule();

    const std::vector<WorkloadDescriptor>& workloads() const;

private:
    std::vector<WorkloadDescriptor> workloads_;

    std::unique_ptr<SchedulingPolicy> policy_;
};