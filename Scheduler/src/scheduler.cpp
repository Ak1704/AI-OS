#include "scheduler.hpp"

#include <stdexcept>
#include <utility>

Scheduler::Scheduler(
    std::unique_ptr<SchedulingPolicy> policy,
    std::size_t total_cpu_cores,
    std::uint64_t total_memory_mb
)
    : state_(
          total_cpu_cores,
          total_memory_mb
      ),
      policy_(std::move(policy))
{
    if (!policy_)
    {
        throw std::invalid_argument(
            "Scheduler requires a policy"
        );
    }
}

void Scheduler::submit(
    const WorkloadDescriptor& workload
)
{
    if (workload.state != WorkloadState::READY)
    {
        throw std::invalid_argument(
            "Only READY workloads can enter the ready queue"
        );
    }

    state_.ready_queue().push(workload);
}

SchedulingDecision Scheduler::schedule()
{
    const auto& workloads =
        state_.ready_queue().workloads();

    if (workloads.empty())
    {
        throw std::runtime_error(
            "No workloads available for scheduling"
        );
    }

    int selected_index =
        policy_->select_workload(workloads);

    if (selected_index < 0 ||
        selected_index >=
            static_cast<int>(workloads.size()))
    {
        throw std::runtime_error(
            "Policy returned invalid workload index"
        );
    }

    const auto& workload =
        workloads[selected_index];

    if (!state_.resources().can_allocate(workload))
    {
        SchedulingDecision decision;

        decision.workload_id =
            workload.id;

        decision.action =
            SchedulingAction::WAIT;

        decision.reason =
            "Insufficient resources";

        return decision;
    }

    SchedulingDecision decision;

    decision.workload_id =
        workload.id;

    decision.action =
        SchedulingAction::RUN;

    decision.cpu_cores =
        state_.resources().available_cpu_ids();

    decision.cpu_cores.resize(
        workload.cpu_cores_required
    );

    decision.reason =
        "Selected by scheduling policy";

    return decision;
}

const SchedulerState&
Scheduler::state() const
{
    return state_;
}