#include <gtest/gtest.h>

#include "Dispatcher/Dispatcher.h"
using namespace aios;

class FakeDispatcher : public Dispatcher {
public:
    bool startCalled = false;
    bool pauseCalled = false;
    bool resumeCalled = false;
    bool preemptCalled = false;
    bool terminateCalled = false;

    bool startResult = true;
    bool pauseResult = true;
    bool resumeResult = true;
    bool preemptResult = true;
    bool terminateResult = true;

    bool start(
        ProcessHandle& process
    ) override {
        startCalled = true;

        if (startResult) {
            process.setState(
                ProcessState::RUNNING
            );
        }

        return startResult;
    }

    bool pause(
        ProcessHandle& process
    ) override {
        pauseCalled = true;

        if (pauseResult) {
            process.setState(
                ProcessState::PAUSED
            );
        }

        return pauseResult;
    }

    bool resume(
        ProcessHandle& process
    ) override {
        resumeCalled = true;

        if (resumeResult) {
            process.setState(
                ProcessState::RUNNING
            );
        }

        return resumeResult;
    }

    bool preempt(
        ProcessHandle& process
    ) override {
        preemptCalled = true;

        if (preemptResult) {
            process.setState(
                ProcessState::WAITING
            );
        }

        return preemptResult;
    }

    bool terminate(
        ProcessHandle& process
    ) override {
        terminateCalled = true;

        if (terminateResult) {
            process.setState(
                ProcessState::TERMINATED
            );
        }

        return terminateResult;
    }
    bool apply_cpu_assignment(
        ProcessHandle& process
    ) override {}
};

TEST(DispatcherTest, StartIsCalled) {
    FakeDispatcher dispatcher;

    ProcessHandle process(
        1,
        1234
    );

    EXPECT_TRUE(
        dispatcher.start(process)
    );

    EXPECT_TRUE(
        dispatcher.startCalled
    );

    EXPECT_TRUE(
        process.isRunning()
    );
}

TEST(DispatcherTest, PauseIsCalled) {
    FakeDispatcher dispatcher;

    ProcessHandle process(
        1,
        1234
    );

    process.setState(
        ProcessState::RUNNING
    );

    EXPECT_TRUE(
        dispatcher.pause(process)
    );

    EXPECT_TRUE(
        dispatcher.pauseCalled
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::PAUSED
    );
}

TEST(DispatcherTest, ResumeIsCalled) {
    FakeDispatcher dispatcher;

    ProcessHandle process(
        1,
        1234
    );

    process.setState(
        ProcessState::PAUSED
    );

    EXPECT_TRUE(
        dispatcher.resume(process)
    );

    EXPECT_TRUE(
        dispatcher.resumeCalled
    );

    EXPECT_TRUE(
        process.isRunning()
    );
}

TEST(DispatcherTest, PreemptIsCalled) {
    FakeDispatcher dispatcher;

    ProcessHandle process(
        1,
        1234
    );

    process.setState(
        ProcessState::RUNNING
    );

    EXPECT_TRUE(
        dispatcher.preempt(process)
    );

    EXPECT_TRUE(
        dispatcher.preemptCalled
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::WAITING
    );
}

TEST(DispatcherTest, TerminateIsCalled) {
    FakeDispatcher dispatcher;

    ProcessHandle process(
        1,
        1234
    );

    process.setState(
        ProcessState::RUNNING
    );

    EXPECT_TRUE(
        dispatcher.terminate(process)
    );

    EXPECT_TRUE(
        dispatcher.terminateCalled
    );

    EXPECT_TRUE(
        process.isTerminated()
    );
}

TEST(DispatcherTest, FailedStartDoesNotChangeState) {
    FakeDispatcher dispatcher;

    dispatcher.startResult = false;

    ProcessHandle process(
        1,
        1234
    );

    EXPECT_FALSE(
        dispatcher.start(process)
    );

    EXPECT_FALSE(
        dispatcher.startCalled
    == false
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::CREATED
    );
}