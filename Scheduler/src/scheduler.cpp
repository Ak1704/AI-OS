#include "scheduler.hpp"

#include <stdexcept>
#include <utility>

Scheduler::Scheduler(
    std::unique_ptr<SchedulingPolicy> policy
)
    : policy_(std::move(policy))
{
}

void Scheduler::submit(
    const WorkloadDescriptor& workload
)
{
    workloads_.push_back(workload);
}

SchedulingDecision Scheduler::schedule()
{
    if (workloads_.empty()) {
        throw std::runtime_error(
            "No workloads available for scheduling"
        );
    }

    int selected_index =
        policy_->select_workload(workloads_);

    if (selected_index < 0 ||
        selected_index >=
            static_cast<int>(workloads_.size())) {

        throw std::runtime_error(
            "Policy returned invalid workload index"
        );
    }

    const auto& workload =
        workloads_[selected_index];

    SchedulingDecision decision;

    decision.workload_id = workload.id;
    decision.action = SchedulingAction::RUN;
    decision.reason =
        "Selected by scheduling policy";

    return decision;
}

const std::vector<WorkloadDescriptor>&
Scheduler::workloads() const
{
    return workloads_;
}