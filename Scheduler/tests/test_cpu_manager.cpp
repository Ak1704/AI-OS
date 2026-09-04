#include "cpu_manager.hpp"

#include <gtest/gtest.h>

#include <unistd.h>

#include <vector>


TEST(CpuManagerTest, ReportsTotalCpuCores)
{
    CpuManager manager(4);

    EXPECT_EQ(
        manager.total_cpu_cores(),
        4
    );
}


TEST(CpuManagerTest, AcceptsValidCpuIds)
{
    CpuManager manager(4);

    EXPECT_TRUE(
        manager.cpu_id_valid(0)
    );

    EXPECT_TRUE(
        manager.cpu_id_valid(1)
    );

    EXPECT_TRUE(
        manager.cpu_id_valid(2)
    );

    EXPECT_TRUE(
        manager.cpu_id_valid(3)
    );
}


TEST(CpuManagerTest, RejectsNegativeCpuId)
{
    CpuManager manager(4);

    EXPECT_FALSE(
        manager.cpu_id_valid(-1)
    );
}


TEST(CpuManagerTest, RejectsCpuIdOutsideRange)
{
    CpuManager manager(4);

    EXPECT_FALSE(
        manager.cpu_id_valid(4)
    );

    EXPECT_FALSE(
        manager.cpu_id_valid(10)
    );
}


TEST(CpuManagerTest, RejectsEmptyCpuSet)
{
    CpuManager manager(4);

    std::vector<int> cpu_ids;

    EXPECT_FALSE(
        manager.apply_affinity(
            static_cast<int>(getpid()),
            cpu_ids
        )
    );
}


TEST(CpuManagerTest, RejectsInvalidCpuSet)
{
    CpuManager manager(4);

    std::vector<int> cpu_ids{
        0,
        4
    };

    EXPECT_FALSE(
        manager.apply_affinity(
            static_cast<int>(getpid()),
            cpu_ids
        )
    );
}