
#include "ready_queue.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <string>

// ------------------------------------------------------------
// Test 1:
// A newly created ReadyQueue should be empty.
// ------------------------------------------------------------

TEST(ReadyQueueTest, StartsEmpty)
{
    ReadyQueue queue;

    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
}


// ------------------------------------------------------------
// Test 2:
// A workload can be added to the ReadyQueue.
// ------------------------------------------------------------

TEST(ReadyQueueTest, CanInsertWorkload)
{
    ReadyQueue queue;

    WorkloadDescriptor workload{
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    };

    queue.push(workload);

    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 1);

    ASSERT_EQ(queue.workloads().size(), 1);

    EXPECT_EQ(
        queue.workloads()[0].id,
        1
    );

    EXPECT_EQ(
        queue.workloads()[0].name,
        "Training"
    );

    EXPECT_EQ(
        queue.workloads()[0].priority,
        8
    );
}


// ------------------------------------------------------------
// Test 3:
// Multiple workloads should preserve insertion order.
// ------------------------------------------------------------

TEST(ReadyQueueTest, PreservesInsertionOrder)
{
    ReadyQueue queue;

    WorkloadDescriptor training{
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    };

    WorkloadDescriptor inference{
        2,
        "Inference",
        WorkloadType::INFERENCE,
        10,
        1,
        2048,
        WorkloadState::READY
    };

    WorkloadDescriptor data_loader{
        3,
        "DataLoader",
        WorkloadType::DATA_LOADING,
        5,
        1,
        1024,
        WorkloadState::READY
    };

    queue.push(training);
    queue.push(inference);
    queue.push(data_loader);

    EXPECT_EQ(queue.size(), 3);

    ASSERT_EQ(queue.workloads().size(), 3);

    EXPECT_EQ(queue.workloads()[0].id, 1);
    EXPECT_EQ(queue.workloads()[1].id, 2);
    EXPECT_EQ(queue.workloads()[2].id, 3);
}


// ------------------------------------------------------------
// Test 4:
// A workload can be removed by its ID.
// ------------------------------------------------------------

TEST(ReadyQueueTest, CanRemoveWorkload)
{
    ReadyQueue queue;

    queue.push({
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    });

    queue.push({
        2,
        "Inference",
        WorkloadType::INFERENCE,
        10,
        1,
        2048,
        WorkloadState::READY
    });

    ASSERT_EQ(queue.size(), 2);

    queue.remove(1);

    ASSERT_EQ(queue.size(), 1);

    ASSERT_EQ(queue.workloads().size(), 1);

    EXPECT_EQ(
        queue.workloads()[0].id,
        2
    );
}


// ------------------------------------------------------------
// Test 5:
// Removing a workload that does not exist should not
// change the queue.
// ------------------------------------------------------------

TEST(ReadyQueueTest, RemovingUnknownWorkloadDoesNothing)
{
    ReadyQueue queue;

    queue.push({
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    });

    queue.remove(999);

    EXPECT_EQ(queue.size(), 1);

    EXPECT_EQ(
        queue.workloads()[0].id,
        1
    );
}


// ------------------------------------------------------------
// Test 6:
// clear() should remove every workload.
// ------------------------------------------------------------

TEST(ReadyQueueTest, CanClearQueue)
{
    ReadyQueue queue;

    queue.push({
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    });

    queue.push({
        2,
        "Inference",
        WorkloadType::INFERENCE,
        10,
        1,
        2048,
        WorkloadState::READY
    });

    queue.push({
        3,
        "DataLoader",
        WorkloadType::DATA_LOADING,
        5,
        1,
        1024,
        WorkloadState::READY
    });

    EXPECT_EQ(queue.size(), 3);

    queue.clear();

    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
    EXPECT_TRUE(queue.workloads().empty());
}


// ------------------------------------------------------------
// Test 7:
// The queue should correctly handle a single workload.
// ------------------------------------------------------------

TEST(ReadyQueueTest, HandlesSingleWorkload)
{
    ReadyQueue queue;

    queue.push({
        42,
        "ResNet Training",
        WorkloadType::TRAINING,
        8,
        4,
        8192,
        WorkloadState::READY
    });

    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 1);

    EXPECT_EQ(
        queue.workloads()[0].id,
        42
    );

    EXPECT_EQ(
        queue.workloads()[0].cpu_cores_required,
        4
    );

    EXPECT_EQ(
        queue.workloads()[0].memory_mb_required,
        8192
    );
}

