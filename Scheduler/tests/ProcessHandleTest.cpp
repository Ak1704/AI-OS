#include <gtest/gtest.h>

#include "Dispatcher/ProcessHandle.h"

using namespace aios;

TEST(ProcessHandleTest, DefaultConstruction) {
    ProcessHandle process;

    EXPECT_EQ(
        process.getWorkloadId(),
        0
    );

    EXPECT_EQ(
        process.getPid(),
        -1
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::CREATED
    );

    EXPECT_TRUE(
        process.getAssignedCpus().empty()
    );
}

TEST(ProcessHandleTest, StoresWorkloadId) {
    ProcessHandle process(42);

    EXPECT_EQ(
        process.getWorkloadId(),
        42
    );
}

TEST(ProcessHandleTest, StoresPid) {
    ProcessHandle process(42, 1234);

    EXPECT_EQ(
        process.getWorkloadId(),
        42
    );

    EXPECT_EQ(
        process.getPid(),
        1234
    );
}

TEST(ProcessHandleTest, InitialStateIsCreated) {
    ProcessHandle process(42, 1234);

    EXPECT_EQ(
        process.getState(),
        ProcessState::CREATED
    );
}

TEST(ProcessHandleTest, StateCanBeChanged) {
    ProcessHandle process(42, 1234);

    process.setState(
        ProcessState::RUNNING
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::RUNNING
    );

    EXPECT_TRUE(
        process.isRunning()
    );
}

TEST(ProcessHandleTest, PausedState) {
    ProcessHandle process(42, 1234);

    process.setState(
        ProcessState::PAUSED
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::PAUSED
    );

    EXPECT_FALSE(
        process.isRunning()
    );
}

TEST(ProcessHandleTest, WaitingState) {
    ProcessHandle process(42, 1234);

    process.setState(
        ProcessState::WAITING
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::WAITING
    );
}

TEST(ProcessHandleTest, TerminatedState) {
    ProcessHandle process(42, 1234);

    process.setState(
        ProcessState::TERMINATED
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::TERMINATED
    );

    EXPECT_TRUE(
        process.isTerminated()
    );
}

TEST(ProcessHandleTest, StoresAssignedCpus) {
    ProcessHandle process(42, 1234);

    std::vector<int> cpus{
        0,
        1,
        2
    };

    process.setAssignedCpus(cpus);

    EXPECT_EQ(
        process.getAssignedCpus(),
        cpus
    );
}

TEST(ProcessHandleTest, ValidProcess) {
    ProcessHandle process(42, 1234);

    EXPECT_TRUE(
        process.isValid()
    );
}

TEST(ProcessHandleTest, InvalidWithoutWorkloadId) {
    ProcessHandle process(0, 1234);

    EXPECT_FALSE(
        process.isValid()
    );
}

TEST(ProcessHandleTest, InvalidWithoutPid) {
    ProcessHandle process(42, -1);

    EXPECT_FALSE(
        process.isValid()
    );
}