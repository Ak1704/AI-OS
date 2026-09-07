#include <gtest/gtest.h>

#include "Dispatcher.h"

namespace {

class TestDispatcher final : public aios::Dispatcher {
public:
    bool dispatch(std::uint64_t workloadId) override {
        lastWorkloadId = workloadId;
        dispatched = true;
        return true;
    }

    std::uint64_t lastWorkloadId = 0;
    bool dispatched = false;
};

} // namespace

TEST(DispatcherTest, CanDispatchWorkload) {
    TestDispatcher dispatcher;

    const std::uint64_t workloadId = 42;

    const bool result = dispatcher.dispatch(workloadId);

    EXPECT_TRUE(result);
    EXPECT_TRUE(dispatcher.dispatched);
    EXPECT_EQ(dispatcher.lastWorkloadId, workloadId);
}

TEST(DispatcherTest, CanDispatchMultipleWorkloads) {
    TestDispatcher dispatcher;

    EXPECT_TRUE(dispatcher.dispatch(1));
    EXPECT_EQ(dispatcher.lastWorkloadId, 1);

    EXPECT_TRUE(dispatcher.dispatch(2));
    EXPECT_EQ(dispatcher.lastWorkloadId, 2);

    EXPECT_TRUE(dispatcher.dispatch(3));
    EXPECT_EQ(dispatcher.lastWorkloadId, 3);
}