
#include "priority_policy.hpp"
#include "scheduler.hpp"

#include <gtest/gtest.h>

#include <memory>

// ------------------------------------------------------------
// Test 1:
// Scheduler should select the highest-priority READY
// workload using PriorityPolicy.
// ------------------------------------------------------------

TEST(SchedulerTest, SelectsHighestPriority)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    Scheduler scheduler(
        std::move(policy),
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

    EXPECT_EQ(
        decision.workload_id,
        2
    );

    EXPECT_EQ(
        decision.action,
        SchedulingAction::RUN
    );
}


// ------------------------------------------------------------
// Test 2:
// Scheduler should return the CPU cores required by
// the selected workload.
// ------------------------------------------------------------

TEST(SchedulerTest, ReturnsRequiredCpuCores)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    Scheduler scheduler(
        std::move(policy),
        4,
        8192
    );

    scheduler.submit({
        1,
        "Training",
        WorkloadType::TRAINING,
        10,
        2,
        4096,
        WorkloadState::READY
    });

    auto decision =
        scheduler.schedule();

    ASSERT_EQ(
        decision.action,
        SchedulingAction::RUN
    );

    ASSERT_EQ(
        decision.cpu_cores.size(),
        2
    );

    EXPECT_EQ(
        decision.cpu_cores[0],
        0
    );

    EXPECT_EQ(
        decision.cpu_cores[1],
        1
    );
}


// ------------------------------------------------------------
// Test 3:
// Scheduler should return WAIT when the selected workload
// requires more CPU cores than are available.
// ------------------------------------------------------------

TEST(SchedulerTest, WaitsWhenCpuResourcesAreUnavailable)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    Scheduler scheduler(
        std::move(policy),
        2,
        8192
    );

    scheduler.submit({
        1,
        "LargeTraining",
        WorkloadType::TRAINING,
        10,
        4,
        4096,
        WorkloadState::READY
    });

    auto decision =
        scheduler.schedule();

    EXPECT_EQ(
        decision.workload_id,
        1
    );

    EXPECT_EQ(
        decision.action,
        SchedulingAction::WAIT
    );

    EXPECT_TRUE(
        decision.cpu_cores.empty()
    );

    EXPECT_EQ(
        decision.reason,
        "Insufficient resources"
    );
}


// ------------------------------------------------------------
// Test 4:
// Scheduler should return WAIT when memory is insufficient.
// ------------------------------------------------------------

TEST(SchedulerTest, WaitsWhenMemoryIsUnavailable)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    Scheduler scheduler(
        std::move(policy),
        4,
        2048
    );

    scheduler.submit({
        1,
        "LargeTraining",
        WorkloadType::TRAINING,
        10,
        2,
        4096,
        WorkloadState::READY
    });

    auto decision =
        scheduler.schedule();

    EXPECT_EQ(
        decision.workload_id,
        1
    );

    EXPECT_EQ(
        decision.action,
        SchedulingAction::WAIT
    );

    EXPECT_TRUE(
        decision.cpu_cores.empty()
    );
}


// ------------------------------------------------------------
// Test 5:
// Scheduler should reject a non-READY workload.
// ------------------------------------------------------------

TEST(SchedulerTest, RejectsNonReadyWorkload)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    Scheduler scheduler(
        std::move(policy),
        4,
        8192
    );

    WorkloadDescriptor running_workload{
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::RUNNING
    };

    EXPECT_THROW(
        scheduler.submit(running_workload),
        std::invalid_argument
    );
}


// ------------------------------------------------------------
// Test 6:
// Scheduling with no workloads should throw an exception.
// ------------------------------------------------------------

TEST(SchedulerTest, ThrowsWhenNoWorkloadsExist)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    Scheduler scheduler(
        std::move(policy),
        4,
        8192
    );

    EXPECT_THROW(
        scheduler.schedule(),
        std::runtime_error
    );
}


// ------------------------------------------------------------
// Test 7:
// Scheduler should reject a null policy.
// ------------------------------------------------------------

TEST(SchedulerTest, RejectsNullPolicy)
{
    std::unique_ptr<SchedulingPolicy> policy =
        nullptr;

    EXPECT_THROW(
        Scheduler scheduler(
            std::move(policy),
            4,
            8192
        ),
        std::invalid_argument
    );
}


// ------------------------------------------------------------
// Test 8:
// Scheduler state should contain submitted workloads.
// ------------------------------------------------------------

TEST(SchedulerTest, StoresSubmittedWorkloadsInReadyQueue)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    Scheduler scheduler(
        std::move(policy),
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

    const auto& state =
        scheduler.state();

    EXPECT_EQ(
        state.ready_queue().size(),
        2
    );

    ASSERT_EQ(
        state.ready_queue().workloads().size(),
        2
    );

    EXPECT_EQ(
        state.ready_queue().workloads()[0].id,
        1
    );

    EXPECT_EQ(
        state.ready_queue().workloads()[1].id,
        2
    );
}


// ------------------------------------------------------------
// Test 9:
// Scheduler should expose the configured total resources.
// ------------------------------------------------------------

TEST(SchedulerTest, ExposesResourceState)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    Scheduler scheduler(
        std::move(policy),
        8,
        16384
    );

    const auto& resources =
        scheduler.state().resources();

    EXPECT_EQ(
        resources.total_cpu_cores(),
        8
    );

    EXPECT_EQ(
        resources.available_cpu_cores(),
        8
    );

    EXPECT_EQ(
        resources.total_memory_mb(),
        16384
    );

    EXPECT_EQ(
        resources.available_memory_mb(),
        16384
    );
}


// ------------------------------------------------------------
// Test 10:
// Highest priority should win regardless of insertion order.
// ------------------------------------------------------------

TEST(SchedulerTest, PriorityOverridesInsertionOrder)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    Scheduler scheduler(
        std::move(policy),
        8,
        16384
    );

    scheduler.submit({
        1,
        "Background",
        WorkloadType::BACKGROUND,
        1,
        1,
        512,
        WorkloadState::READY
    });

    scheduler.submit({
        2,
        "Training",
        WorkloadType::TRAINING,
        7,
        4,
        8192,
        WorkloadState::READY
    });

    scheduler.submit({
        3,
        "Inference",
        WorkloadType::INFERENCE,
        10,
        2,
        2048,
        WorkloadState::READY
    });

    auto decision =
        scheduler.schedule();

    EXPECT_EQ(
        decision.workload_id,
        3
    );

    EXPECT_EQ(
        decision.action,
        SchedulingAction::RUN
    );
}


// ------------------------------------------------------------
// Test 11:
// If the highest-priority workload cannot fit into the
// available resources, the current Phase 2 scheduler
// should return WAIT rather than automatically selecting
// a lower-priority workload.
// ------------------------------------------------------------

TEST(SchedulerTest, HighestPriorityResourceFailureReturnsWait)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    Scheduler scheduler(
        std::move(policy),
        2,
        4096
    );

    scheduler.submit({
        1,
        "LargeInference",
        WorkloadType::INFERENCE,
        10,
        4,
        8192,
        WorkloadState::READY
    });

    scheduler.submit({
        2,
        "SmallBackground",
        WorkloadType::BACKGROUND,
        1,
        1,
        512,
        WorkloadState::READY
    });

    auto decision =
        scheduler.schedule();

    EXPECT_EQ(
        decision.workload_id,
        1
    );

    EXPECT_EQ(
        decision.action,
        SchedulingAction::WAIT
    );
}


// ------------------------------------------------------------
// Test 12:
// Scheduling a workload should not itself consume resources.
// Resource allocation is still logically separate from
// scheduling in Phase 2.
// ------------------------------------------------------------

TEST(SchedulerTest, SchedulingDoesNotConsumeResources)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    Scheduler scheduler(
        std::move(policy),
        4,
        8192
    );

    scheduler.submit({
        1,
        "Training",
        WorkloadType::TRAINING,
        10,
        2,
        4096,
        WorkloadState::READY
    });

    auto decision =
        scheduler.schedule();

    ASSERT_EQ(
        decision.action,
        SchedulingAction::RUN
    );

    EXPECT_EQ(
        scheduler.state()
            .resources()
            .available_cpu_cores(),
        4
    );

    EXPECT_EQ(
        scheduler.state()
            .resources()
            .available_memory_mb(),
        8192
    );
}

