#include "priority_policy.hpp"
#include "scheduler.hpp"

#include <iostream>
#include <memory>

int main()
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    Scheduler scheduler(std::move(policy));

    scheduler.submit({
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        4,
        8192,
        WorkloadState::READY
    });

    scheduler.submit({
        2,
        "Inference",
        WorkloadType::INFERENCE,
        10,
        2,
        2048,
        WorkloadState::READY
    });

    scheduler.submit({
        3,
        "DataLoader",
        WorkloadType::DATA_LOADING,
        5,
        2,
        1024,
        WorkloadState::READY
    });

    auto decision = scheduler.schedule();

    std::cout
        << "Selected workload: "
        << decision.workload_id
        << '\n';

    std::cout
        << "Reason: "
        << decision.reason
        << '\n';

    return 0;
}