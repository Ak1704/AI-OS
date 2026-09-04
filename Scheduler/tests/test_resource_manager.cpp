#include "resource_manager.hpp"

#include <gtest/gtest.h>

class TestResourceManager : public ResourceManager
{
public:

    bool apply(
        const SchedulingDecision& decision,
        const WorkloadDescriptor& workload,
        const WorkloadRuntime& runtime
    ) override
    {
        applied_ = true;

        last_workload_id_ =
            workload.id;

        last_runtime_workload_id_ =
            runtime.workload_id;

        last_process_id_ =
            runtime.process_id;

        last_action_ =
            decision.action;

        return true;
    }

    void release(
        const WorkloadDescriptor& workload,
        const WorkloadRuntime& runtime
    ) override
    {
        released_ = true;

        last_workload_id_ =
            workload.id;

        last_runtime_workload_id_ =
            runtime.workload_id;

        last_process_id_ =
            runtime.process_id;
    }

    bool applied() const
    {
        return applied_;
    }

    bool released() const
    {
        return released_;
    }

    std::uint64_t last_workload_id() const
    {
        return last_workload_id_;
    }

    std::uint64_t last_runtime_workload_id() const
    {
        return last_runtime_workload_id_;
    }

    int last_process_id() const
    {
        return last_process_id_;
    }

    SchedulingAction last_action() const
    {
        return last_action_;
    }

private:

    bool applied_ = false;

    bool released_ = false;

    std::uint64_t last_workload_id_ = 0;

    std::uint64_t last_runtime_workload_id_ = 0;

    int last_process_id_ = -1;

    SchedulingAction last_action_ =
        SchedulingAction::WAIT;
};


TEST(ResourceManagerTest, CanApplyDecision)
{
    TestResourceManager manager;

    SchedulingDecision decision;

    decision.workload_id = 1;

    decision.action =
        SchedulingAction::RUN;

    decision.cpu_cores = {
        0,
        1
    };

    decision.reason =
        "Selected by scheduling policy";

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

    EXPECT_TRUE(
        manager.apply(
            decision,
            workload,
            runtime
        )
    );

    EXPECT_TRUE(
        manager.applied()
    );

    EXPECT_EQ(
        manager.last_workload_id(),
        1
    );

    EXPECT_EQ(
        manager.last_runtime_workload_id(),
        1
    );

    EXPECT_EQ(
        manager.last_process_id(),
        1234
    );

    EXPECT_EQ(
        manager.last_action(),
        SchedulingAction::RUN
    );
}


TEST(ResourceManagerTest, CanReleaseResources)
{
    TestResourceManager manager;

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

    manager.release(
        workload,
        runtime
    );

    EXPECT_TRUE(
        manager.released()
    );

    EXPECT_EQ(
        manager.last_workload_id(),
        1
    );

    EXPECT_EQ(
        manager.last_runtime_workload_id(),
        1
    );

    EXPECT_EQ(
        manager.last_process_id(),
        1234
    );
}


TEST(ResourceManagerTest, SupportsPolymorphism)
{
    std::unique_ptr<ResourceManager>
        manager =
            std::make_unique<TestResourceManager>();

    SchedulingDecision decision;

    decision.workload_id = 5;

    decision.action =
        SchedulingAction::RUN;

    decision.cpu_cores = {
        0
    };

    WorkloadDescriptor workload{
        5,
        "Inference",
        WorkloadType::INFERENCE,
        10,
        1,
        2048,
        WorkloadState::READY
    };

    WorkloadRuntime runtime{
        5,
        5678
    };

    EXPECT_TRUE(
        manager->apply(
            decision,
            workload,
            runtime
        )
    );
}