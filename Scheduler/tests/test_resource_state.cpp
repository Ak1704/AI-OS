
#include "resource_state.hpp"

#include <gtest/gtest.h>

#include <cstdint>

// ------------------------------------------------------------
// Test 1:
// ResourceState should initialize with the specified
// CPU and memory resources.
// ------------------------------------------------------------

TEST(ResourceStateTest, InitializesResources)
{
    ResourceState resources(
        4,
        8192
    );

    EXPECT_EQ(
        resources.total_cpu_cores(),
        4
    );

    EXPECT_EQ(
        resources.available_cpu_cores(),
        4
    );

    EXPECT_EQ(
        resources.total_memory_mb(),
        8192
    );

    EXPECT_EQ(
        resources.available_memory_mb(),
        8192
    );
}


// ------------------------------------------------------------
// Test 2:
// All CPU IDs should initially be available.
// ------------------------------------------------------------

TEST(ResourceStateTest, InitializesCpuIds)
{
    ResourceState resources(
        4,
        8192
    );

    const auto& cpu_ids =
        resources.available_cpu_ids();

    ASSERT_EQ(cpu_ids.size(), 4);

    EXPECT_EQ(cpu_ids[0], 0);
    EXPECT_EQ(cpu_ids[1], 1);
    EXPECT_EQ(cpu_ids[2], 2);
    EXPECT_EQ(cpu_ids[3], 3);
}


// ------------------------------------------------------------
// Test 3:
// A workload that fits within the available resources
// should be accepted.
// ------------------------------------------------------------

TEST(ResourceStateTest, DetectsAvailableResources)
{
    ResourceState resources(
        4,
        8192
    );

    WorkloadDescriptor workload{
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    };

    EXPECT_TRUE(
        resources.can_allocate(workload)
    );
}


// ------------------------------------------------------------
// Test 4:
// A workload requiring too many CPU cores should be
// rejected.
// ------------------------------------------------------------

TEST(ResourceStateTest, RejectsInsufficientCpu)
{
    ResourceState resources(
        2,
        8192
    );

    WorkloadDescriptor workload{
        1,
        "LargeTraining",
        WorkloadType::TRAINING,
        8,
        4,
        4096,
        WorkloadState::READY
    };

    EXPECT_FALSE(
        resources.can_allocate(workload)
    );
}


// ------------------------------------------------------------
// Test 5:
// A workload requiring too much memory should be
// rejected.
// ------------------------------------------------------------

TEST(ResourceStateTest, RejectsInsufficientMemory)
{
    ResourceState resources(
        4,
        4096
    );

    WorkloadDescriptor workload{
        1,
        "LargeTraining",
        WorkloadType::TRAINING,
        8,
        2,
        8192,
        WorkloadState::READY
    };

    EXPECT_FALSE(
        resources.can_allocate(workload)
    );
}


// ------------------------------------------------------------
// Test 6:
// A workload that fits should consume the requested
// resources when allocated.
// ------------------------------------------------------------

TEST(ResourceStateTest, AllocationConsumesResources)
{
    ResourceState resources(
        4,
        8192
    );

    WorkloadDescriptor workload{
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    };

    ASSERT_TRUE(
        resources.allocate(workload)
    );

    EXPECT_EQ(
        resources.available_cpu_cores(),
        2
    );

    EXPECT_EQ(
        resources.available_memory_mb(),
        4096
    );

    EXPECT_EQ(
        resources.available_cpu_ids().size(),
        2
    );
}


// ------------------------------------------------------------
// Test 7:
// Allocation should fail if resources are insufficient.
// Failed allocation must not consume resources.
// ------------------------------------------------------------

TEST(ResourceStateTest, FailedAllocationDoesNotConsumeResources)
{
    ResourceState resources(
        2,
        4096
    );

    WorkloadDescriptor workload{
        1,
        "LargeTraining",
        WorkloadType::TRAINING,
        8,
        4,
        8192,
        WorkloadState::READY
    };

    EXPECT_FALSE(
        resources.allocate(workload)
    );

    EXPECT_EQ(
        resources.available_cpu_cores(),
        2
    );

    EXPECT_EQ(
        resources.available_memory_mb(),
        4096
    );

    EXPECT_EQ(
        resources.available_cpu_ids().size(),
        2
    );
}


// ------------------------------------------------------------
// Test 8:
// Releasing an allocated workload should restore the
// resource counts.
// ------------------------------------------------------------

TEST(ResourceStateTest, ReleaseRestoresResources)
{
    ResourceState resources(
        4,
        8192
    );

    WorkloadDescriptor workload{
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    };

    ASSERT_TRUE(
        resources.allocate(workload)
    );

    EXPECT_EQ(
        resources.available_cpu_cores(),
        2
    );

    EXPECT_EQ(
        resources.available_memory_mb(),
        4096
    );

    resources.release(workload);

    EXPECT_EQ(
        resources.available_cpu_cores(),
        4
    );

    EXPECT_EQ(
        resources.available_memory_mb(),
        8192
    );

    EXPECT_EQ(
        resources.available_cpu_ids().size(),
        4
    );
}


// ------------------------------------------------------------
// Test 9:
// A workload requiring exactly all available resources
// should be accepted.
// ------------------------------------------------------------

TEST(ResourceStateTest, CanAllocateAllResources)
{
    ResourceState resources(
        4,
        8192
    );

    WorkloadDescriptor workload{
        1,
        "FullSystemTraining",
        WorkloadType::TRAINING,
        10,
        4,
        8192,
        WorkloadState::READY
    };

    EXPECT_TRUE(
        resources.can_allocate(workload)
    );

    ASSERT_TRUE(
        resources.allocate(workload)
    );

    EXPECT_EQ(
        resources.available_cpu_cores(),
        0
    );

    EXPECT_EQ(
        resources.available_memory_mb(),
        0
    );
}


// ------------------------------------------------------------
// Test 10:
// Once all resources are consumed, another workload
// should not be allocatable.
// ------------------------------------------------------------

TEST(ResourceStateTest, CannotAllocateWhenResourcesAreExhausted)
{
    ResourceState resources(
        4,
        8192
    );

    WorkloadDescriptor first{
        1,
        "Training",
        WorkloadType::TRAINING,
        10,
        4,
        8192,
        WorkloadState::READY
    };

    WorkloadDescriptor second{
        2,
        "Inference",
        WorkloadType::INFERENCE,
        9,
        1,
        1024,
        WorkloadState::READY
    };

    ASSERT_TRUE(
        resources.allocate(first)
    );

    EXPECT_FALSE(
        resources.can_allocate(second)
    );
}


// ------------------------------------------------------------
// Test 11:
// A workload that fits exactly within the remaining
// resources should be accepted.
// ------------------------------------------------------------

TEST(ResourceStateTest, CanAllocateRemainingResources)
{
    ResourceState resources(
        4,
        8192
    );

    WorkloadDescriptor first{
        1,
        "Training",
        WorkloadType::TRAINING,
        8,
        2,
        4096,
        WorkloadState::READY
    };

    WorkloadDescriptor second{
        2,
        "Inference",
        WorkloadType::INFERENCE,
        10,
        2,
        4096,
        WorkloadState::READY
    };

    ASSERT_TRUE(
        resources.allocate(first)
    );

    EXPECT_TRUE(
        resources.can_allocate(second)
    );

    ASSERT_TRUE(
        resources.allocate(second)
    );

    EXPECT_EQ(
        resources.available_cpu_cores(),
        0
    );

    EXPECT_EQ(
        resources.available_memory_mb(),
        0
    );
}

