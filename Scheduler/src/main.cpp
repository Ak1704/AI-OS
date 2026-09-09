#include "cpu_manager.hpp"
#include "Dispatcher/Dispatcher.h"
#include "Dispatcher/CpuDispatcher.hpp"
#include "linux_resource_manager.hpp"
#include "priority_policy.hpp"
#include "scheduler.hpp"

#include <iostream>
#include <memory>
#include <sched.h>
#include <unistd.h>

int main() {
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

    WorkloadDescriptor workload{
        1,
        "example_inference",
        WorkloadType::INFERENCE,
        10,
        1,
        1024,
        WorkloadState::READY
    };

    WorkloadRuntime runtime{
        workload.id,
        static_cast<int>(getpid())
    };

    scheduler.register_runtime(
        runtime
    );

    scheduler.submit(
        workload
    );

    SchedulingDecision decision =
        scheduler.schedule();

    std::cout
        << "Workload: "
        << decision.workload_id
        << '\n';

    std::cout
        << "Action: "
        << (
            decision.action ==
            SchedulingAction::RUN
                ? "RUN"
                : "WAIT"
        )
        << '\n';

    std::cout
        << "Reason: "
        << decision.reason
        << '\n';

    /*
     * Phase 4 / Step 5 demonstration.
     *
     * Use the current process as the ProcessHandle
     * so that we don't create a new process yet.
     */
    if (decision.action ==
        SchedulingAction::RUN) {

        CpuManager cpuManager(4);

        aios::CpuDispatcher dispatcher(
            cpuManager
        );

        aios::ProcessHandle process(
            workload.id,
            static_cast<std::int64_t>(
                getpid()
            )
        );

        process.setAssignedCpus(
            decision.cpu_cores
        );

        if (dispatcher.start(process)) {
            std::cout
                << "Dispatcher: START succeeded\n";

            std::cout
                << "Process state: RUNNING\n";
        }
        else {
            std::cout
                << "Dispatcher: START failed\n";
        }
    }

    return 0;
}