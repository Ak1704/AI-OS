#include "priority_policy.hpp"
#include "resource_manager.hpp"
#include "scheduler.hpp"

#include <gtest/gtest.h>

#include <memory>


class TestResourceManager : public ResourceManager
{
public:

    bool apply(
        const SchedulingDecision& decision,
        const WorkloadDescriptor& workload,
        const WorkloadRuntime& runtime
    ) override
    {
        apply_called_ = true;

        last_decision_ =
            decision;

        last_workload_id_ =
            workload.id;

        last_process_id_ =
            runtime.process_id;

        return should_apply_;
    }

    void release(
        const WorkloadDescriptor& workload,
        const WorkloadRuntime& runtime
    ) override
    {
        release_called_ = true;

        last_workload_id_ =
            workload.id;

        last_process_id_ =
            runtime.process_id;
    }

    void set_apply_result(
        bool result
    )
    {
        should_apply_ = result;
    }

    bool apply_called() const
    {
        return apply_called_;
    }

    bool release_called() const
    {
        return release_called_;
    }

    const SchedulingDecision&
    last_decision() const
    {
        return last_decision_;
    }

    std::uint64_t last_workload_id() const
    {
        return last_workload_id_;
    }

    int last_process_id() const
    {
        return last_process_id_;
    }

private:

    bool should_apply_ = true;

    bool apply_called_ = false;

    bool release_called_ = false;

    SchedulingDecision last_decision_;

    std::uint64_t last_workload_id_ = 0;

    int last_process_id_ = -1;
};


TEST(
    SchedulerTest,
    RejectsNullPolicy
)
{
    auto resource_manager =
        std::make_unique<TestResourceManager>();

    EXPECT_THROW(
        Scheduler(
            nullptr,
            std::move(resource_manager),
            4,
            8192
        ),
        std::invalid_argument
    );
}


TEST(
    SchedulerTest,
    RejectsNullResourceManager
)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    EXPECT_THROW(
        Scheduler(
            std::move(policy),
            nullptr,
            4,
            8192
        ),
        std::invalid_argument
    );
}


TEST(
    SchedulerTest,
    CanSubmitReadyWorkload
)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    auto resource_manager =
        std::make_unique<TestResourceManager>();

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

    EXPECT_EQ(
        scheduler.state()
            .ready_queue()
            .size(),
        1
    );
}


TEST(
    SchedulerTest,
    RejectsNonReadyWorkload
)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    auto resource_manager =
        std::make_unique<TestResourceManager>();

    Scheduler scheduler(
        std::move(policy),
        std::move(resource_manager),
        4,
        8192
    );

    EXPECT_THROW(
        scheduler.submit({
            1,
            "Training",
            WorkloadType::TRAINING,
            8,
            2,
            4096,
            WorkloadState::RUNNING
        }),
        std::invalid_argument
    );
}


TEST(
    SchedulerTest,
    SelectsHighestPriorityWorkload
)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    auto resource_manager =
        std::make_unique<TestResourceManager>();

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

    scheduler.register_runtime({
        1,
        1001
    });

    scheduler.register_runtime({
        2,
        1002
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


TEST(
    SchedulerTest,
    WaitsWhenRuntimeIsMissing
)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    auto resource_manager =
        std::make_unique<TestResourceManager>();

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

    EXPECT_EQ(
        decision.reason,
        "Workload has no registered runtime"
    );
}


TEST(
    SchedulerTest,
    AppliesDecisionThroughResourceManager
)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    auto resource_manager =
        std::make_unique<TestResourceManager>();

    TestResourceManager* manager_ptr =
        resource_manager.get();

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

    scheduler.register_runtime({
        1,
        1234
    });

    auto decision =
        scheduler.schedule();

    EXPECT_EQ(
        decision.action,
        SchedulingAction::RUN
    );

    EXPECT_TRUE(
        manager_ptr->apply_called()
    );

    EXPECT_EQ(
        manager_ptr->last_workload_id(),
        1
    );

    EXPECT_EQ(
        manager_ptr->last_process_id(),
        1234
    );
}


TEST(
    SchedulerTest,
    WaitsWhenResourceManagerFails
)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    auto resource_manager =
        std::make_unique<TestResourceManager>();

    TestResourceManager* manager_ptr =
        resource_manager.get();

    manager_ptr->set_apply_result(false);

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

    scheduler.register_runtime({
        1,
        1234
    });

    auto decision =
        scheduler.schedule();

    EXPECT_EQ(
        decision.action,
        SchedulingAction::WAIT
    );

    EXPECT_EQ(
        decision.reason,
        "Resource manager failed to apply decision"
    );

    EXPECT_TRUE(
        manager_ptr->apply_called()
    );
}


TEST(
    SchedulerTest,
    WaitsWhenResourcesAreInsufficient
)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    auto resource_manager =
        std::make_unique<TestResourceManager>();

    TestResourceManager* manager_ptr =
        resource_manager.get();

    Scheduler scheduler(
        std::move(policy),
        std::move(resource_manager),
        2,
        4096
    );

    scheduler.submit({
        1,
        "LargeTraining",
        WorkloadType::TRAINING,
        10,
        4,
        8192,
        WorkloadState::READY
    });

    scheduler.register_runtime({
        1,
        1234
    });

    auto decision =
        scheduler.schedule();

    EXPECT_EQ(
        decision.action,
        SchedulingAction::WAIT
    );

    EXPECT_EQ(
        decision.reason,
        "Insufficient resources"
    );

    EXPECT_FALSE(
        manager_ptr->apply_called()
    );
}


TEST(
    SchedulerTest,
    DecisionContainsRequiredCpuCores
)
{
    auto policy =
        std::make_unique<PriorityPolicy>();

    auto resource_manager =
        std::make_unique<TestResourceManager>();

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

    scheduler.register_runtime({
        1,
        1234
    });

    auto decision =
        scheduler.schedule();

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