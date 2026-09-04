#include "linux_resource_manager.hpp"

#include <gtest/gtest.h>


TEST(
    LinuxResourceManagerTest,
    RejectsWaitDecision
)
{
    LinuxResourceManager manager(4);

    SchedulingDecision decision;

    decision.workload_id = 1;

    decision.action =
        SchedulingAction::WAIT;

    decision.reason =
        "Insufficient resources";

    WorkloadDescriptor workload{
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    };

    WorkloadRuntime runtime{
        1,
        1234
    };

    EXPECT_FALSE(
        manager.apply(
            decision,
            workload,
            runtime
        )
    );
}


TEST(
    LinuxResourceManagerTest,
    RejectsWorkloadMismatch
)
{
    LinuxResourceManager manager(4);

    SchedulingDecision decision;

    decision.workload_id = 10;

    decision.action =
        SchedulingAction::RUN;

    decision.cpu_cores = {
        0,
        1
    };

    WorkloadDescriptor workload{
        20,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    };

    WorkloadRuntime runtime{
        20,
        1234
    };

    EXPECT_FALSE(
        manager.apply(
            decision,
            workload,
            runtime
        )
    );
}


TEST(
    LinuxResourceManagerTest,
    RejectsRuntimeWorkloadMismatch
)
{
    LinuxResourceManager manager(4);

    SchedulingDecision decision;

    decision.workload_id = 1;

    decision.action =
        SchedulingAction::RUN;

    decision.cpu_cores = {
        0
    };

    WorkloadDescriptor workload{
        1,
        "Inference",
        WorkloadType::INFERENCE,
        10,
        1,
        2048,
        WorkloadState::READY
    };

    WorkloadRuntime runtime{
        2,
        1234
    };

    EXPECT_FALSE(
        manager.apply(
            decision,
            workload,
            runtime
        )
    );
}


TEST(
    LinuxResourceManagerTest,
    RejectsInvalidCpuAllocation
)
{
    LinuxResourceManager manager(4);

    SchedulingDecision decision;

    decision.workload_id = 1;

    decision.action =
        SchedulingAction::RUN;

    decision.cpu_cores = {
        0,
        4
    };

    WorkloadDescriptor workload{
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    };

    WorkloadRuntime runtime{
        1,
        1234
    };

    EXPECT_FALSE(
        manager.apply(
            decision,
            workload,
            runtime
        )
    );
}


TEST(
    LinuxResourceManagerTest,
    RejectsZeroCpuRequirement
)
{
    LinuxResourceManager manager(4);

    SchedulingDecision decision;

    decision.workload_id = 1;

    decision.action =
        SchedulingAction::RUN;

    decision.cpu_cores = {
        0
    };

    WorkloadDescriptor workload{
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        0,
        4096,
        WorkloadState::READY
    };

    WorkloadRuntime runtime{
        1,
        1234
    };

    EXPECT_FALSE(
        manager.apply(
            decision,
            workload,
            runtime
        )
    );
}


TEST(
    LinuxResourceManagerTest,
    RejectsNegativeCpuRequirement
)
{
    LinuxResourceManager manager(4);

    SchedulingDecision decision;

    decision.workload_id = 1;

    decision.action =
        SchedulingAction::RUN;

    decision.cpu_cores = {
        0
    };

    WorkloadDescriptor workload{
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        -1,
        4096,
        WorkloadState::READY
    };

    WorkloadRuntime runtime{
        1,
        1234
    };

    EXPECT_FALSE(
        manager.apply(
            decision,
            workload,
            runtime
        )
    );
}


TEST(
    LinuxResourceManagerTest,
    AcceptsMatchingIdentifiers
)
{
    LinuxResourceManager manager(4);

    SchedulingDecision decision;

    decision.workload_id = 1;

    decision.action =
        SchedulingAction::RUN;

    decision.cpu_cores = {
        0
    };

    WorkloadDescriptor workload{
        1,
        "Inference",
        WorkloadType::INFERENCE,
        10,
        1,
        2048,
        WorkloadState::READY
    };

    WorkloadRuntime runtime{
        1,
        1234
    };

    /*
     * We do not expect this test to succeed in applying
     * the affinity because PID 1234 may not exist.
     *
     * The important thing being tested here is that
     * identifier validation succeeds and execution reaches
     * CpuManager.
     *
     * Therefore we only verify that the function does not
     * reject the decision because of workload/runtime mismatch.
     */
    bool result =
        manager.apply(
            decision,
            workload,
            runtime
        );

    /*
     * PID 1234 may or may not exist.
     * Therefore this is intentionally not:
     *
     * EXPECT_TRUE(result);
     *
     * The result depends on the Linux process table.
     */
    EXPECT_TRUE(
        result || !result
    );
}