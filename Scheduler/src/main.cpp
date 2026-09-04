#include "linux_resource_manager.hpp"
#include "priority_policy.hpp"
#include "scheduler.hpp"

#include <iostream>
#include <memory>
#include <unistd.h>

int main()
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    auto resource_manager =
        std::make_unique<LinuxResourceManager>(4);
    
    Scheduler scheduler(
        std::move(policy),
        std::move(resource_manager),
        4,
        8192
    );

    scheduler.submit({
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    });

    scheduler.submit({
        2,
        "Inference",
        WorkloadType::INFERENCE,
        10,
        1,
        2048,
        WorkloadState::READY
    });

    scheduler.submit({
        3,
        "DataLoader",
        WorkloadType::DATA_LOADING,
        5,
        1,
        1024,
        WorkloadState::READY
    });

    auto decision =
        scheduler.schedule();

    std::cout
        << "Selected workload: "
        << decision.workload_id
        << '\n';

    std::cout
        << "Action: ";

    if (decision.action ==
        SchedulingAction::RUN)
    {
        std::cout << "RUN\n";
    }
    else
    {
        std::cout << "WAIT\n";
    }

    std::cout
        << "CPU cores: ";

    for (int cpu :
         decision.cpu_cores)
    {
        std::cout << cpu << ' ';
    }

    std::cout << '\n';

    std::cout
        << "Available CPU cores: "
        << scheduler.state()
               .resources()
               .available_cpu_cores()
        << '\n';

    std::cout
        << "Available memory: "
        << scheduler.state()
               .resources()
               .available_memory_mb()
        << " MB\n";

    return 0;
}