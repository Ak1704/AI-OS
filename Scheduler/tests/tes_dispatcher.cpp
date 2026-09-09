#include <gtest/gtest.h>

#include "Dispatcher/CpuDispatcher.hpp"
#include "cpu_manager.hpp"

#include <sched.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace aios;

namespace {

int FindAvailableCpu() {
    cpu_set_t allowed;

    CPU_ZERO(&allowed);

    if (sched_getaffinity(
            0,
            sizeof(cpu_set_t),
            &allowed
        ) != 0) {

        return -1;
    }

    for (int cpu = 0;
         cpu < CPU_SETSIZE;
         ++cpu) {

        if (CPU_ISSET(cpu, &allowed)) {
            return cpu;
        }
    }

    return -1;
}

pid_t CreateTestProcess() {
    pid_t pid = fork();

    if (pid == 0) {
        while (true) {
            usleep(10000);
        }

        _exit(0);
    }

    return pid;
}

void KillAndReap(pid_t pid) {
    if (pid <= 0) {
        return;
    }

    kill(
        pid,
        SIGKILL
    );

    waitpid(
        pid,
        nullptr,
        0
    );
}

CpuManager CreateCpuManagerForCurrentSystem() {
    int cpu = FindAvailableCpu();

    if (cpu < 0) {
        return CpuManager(0);
    }

    return CpuManager(
        static_cast<std::size_t>(
            cpu + 1
        )
    );
}

} // namespace


TEST(CpuDispatcherTest, CanBeConstructed) {
    CpuManager cpuManager(4);

    CpuDispatcher dispatcher(
        cpuManager
    );

    SUCCEED();
}


TEST(CpuDispatcherTest, RejectsInvalidProcess) {
    CpuManager cpuManager(4);

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process;

    EXPECT_FALSE(
        dispatcher.start(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::CREATED
    );
}


TEST(CpuDispatcherTest, RejectsProcessWithoutCpuAssignment) {
    CpuManager cpuManager(4);

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            getpid()
        )
    );

    CpuDispatcher dispatcher(
        cpuManager
    );

    EXPECT_FALSE(
        dispatcher.start(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::CREATED
    );
}


TEST(CpuDispatcherTest, RejectsPauseBeforeStart) {
    CpuManager cpuManager(4);

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            getpid()
        )
    );

    EXPECT_FALSE(
        dispatcher.pause(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::CREATED
    );
}


TEST(CpuDispatcherTest, RejectsPreemptBeforeStart) {
    CpuManager cpuManager(4);

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            getpid()
        )
    );

    EXPECT_FALSE(
        dispatcher.preempt(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::CREATED
    );
}


TEST(CpuDispatcherTest, RejectsResumeBeforeStart) {
    CpuManager cpuManager(4);

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            getpid()
        )
    );

    EXPECT_FALSE(
        dispatcher.resume(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::CREATED
    );
}


TEST(CpuDispatcherTest, RejectsTerminateBeforeStart) {
    CpuManager cpuManager(4);

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            getpid()
        )
    );

    EXPECT_FALSE(
        dispatcher.terminate(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::CREATED
    );
}


TEST(CpuDispatcherTest, PauseRunningProcess) {
    int cpu = FindAvailableCpu();

    ASSERT_GE(
        cpu,
        0
    );

    pid_t pid = CreateTestProcess();

    ASSERT_GT(
        pid,
        0
    );

    CpuManager cpuManager(
        static_cast<std::size_t>(
            cpu + 1
        )
    );

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            pid
        )
    );

    process.setAssignedCpus({
        cpu
    });

    ASSERT_TRUE(
        dispatcher.start(process)
    );

    ASSERT_TRUE(
        process.isRunning()
    );

    EXPECT_TRUE(
        dispatcher.pause(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::PAUSED
    );

    KillAndReap(pid);
}


TEST(CpuDispatcherTest, ResumePausedProcess) {
    int cpu = FindAvailableCpu();

    ASSERT_GE(
        cpu,
        0
    );

    pid_t pid = CreateTestProcess();

    ASSERT_GT(
        pid,
        0
    );

    CpuManager cpuManager(
        static_cast<std::size_t>(
            cpu + 1
        )
    );

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            pid
        )
    );

    process.setAssignedCpus({
        cpu
    });

    ASSERT_TRUE(
        dispatcher.start(process)
    );

    ASSERT_TRUE(
        dispatcher.pause(process)
    );

    EXPECT_TRUE(
        dispatcher.resume(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::RUNNING
    );

    KillAndReap(pid);
}


TEST(CpuDispatcherTest, PreemptRunningProcess) {
    int cpu = FindAvailableCpu();

    ASSERT_GE(
        cpu,
        0
    );

    pid_t pid = CreateTestProcess();

    ASSERT_GT(
        pid,
        0
    );

    CpuManager cpuManager(
        static_cast<std::size_t>(
            cpu + 1
        )
    );

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            pid
        )
    );

    process.setAssignedCpus({
        cpu
    });

    ASSERT_TRUE(
        dispatcher.start(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::RUNNING
    );

    EXPECT_TRUE(
        dispatcher.preempt(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::WAITING
    );

    KillAndReap(pid);
}


TEST(CpuDispatcherTest, ResumePreemptedProcess) {
    int cpu = FindAvailableCpu();

    ASSERT_GE(
        cpu,
        0
    );

    pid_t pid = CreateTestProcess();

    ASSERT_GT(
        pid,
        0
    );

    CpuManager cpuManager(
        static_cast<std::size_t>(
            cpu + 1
        )
    );

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            pid
        )
    );

    process.setAssignedCpus({
        cpu
    });

    ASSERT_TRUE(
        dispatcher.start(process)
    );

    ASSERT_TRUE(
        dispatcher.preempt(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::WAITING
    );

    EXPECT_TRUE(
        dispatcher.resume(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::RUNNING
    );

    KillAndReap(pid);
}


TEST(CpuDispatcherTest, CannotPreemptPausedProcess) {
    int cpu = FindAvailableCpu();

    ASSERT_GE(
        cpu,
        0
    );

    pid_t pid = CreateTestProcess();

    ASSERT_GT(
        pid,
        0
    );

    CpuManager cpuManager(
        static_cast<std::size_t>(
            cpu + 1
        )
    );

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            pid
        )
    );

    process.setAssignedCpus({
        cpu
    });

    ASSERT_TRUE(
        dispatcher.start(process)
    );

    ASSERT_TRUE(
        dispatcher.pause(process)
    );

    EXPECT_FALSE(
        dispatcher.preempt(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::PAUSED
    );

    KillAndReap(pid);
}


TEST(CpuDispatcherTest, CannotPauseWaitingProcess) {
    int cpu = FindAvailableCpu();

    ASSERT_GE(
        cpu,
        0
    );

    pid_t pid = CreateTestProcess();

    ASSERT_GT(
        pid,
        0
    );

    CpuManager cpuManager(
        static_cast<std::size_t>(
            cpu + 1
        )
    );

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            pid
        )
    );

    process.setAssignedCpus({
        cpu
    });

    ASSERT_TRUE(
        dispatcher.start(process)
    );

    ASSERT_TRUE(
        dispatcher.preempt(process)
    );

    EXPECT_FALSE(
        dispatcher.pause(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::WAITING
    );

    KillAndReap(pid);
}


TEST(CpuDispatcherTest, TerminateRunningProcess) {
    int cpu = FindAvailableCpu();

    ASSERT_GE(
        cpu,
        0
    );

    pid_t pid = CreateTestProcess();

    ASSERT_GT(
        pid,
        0
    );

    CpuManager cpuManager(
        static_cast<std::size_t>(
            cpu + 1
        )
    );

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            pid
        )
    );

    process.setAssignedCpus({
        cpu
    });

    ASSERT_TRUE(
        dispatcher.start(process)
    );

    EXPECT_TRUE(
        dispatcher.terminate(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::TERMINATED
    );

    int status = 0;

    ASSERT_EQ(
        waitpid(
            pid,
            &status,
            0
        ),
        pid
    );

    EXPECT_TRUE(
        WIFSIGNALED(status)
    );
}


TEST(CpuDispatcherTest, TerminatePausedProcess) {
    int cpu = FindAvailableCpu();

    ASSERT_GE(
        cpu,
        0
    );

    pid_t pid = CreateTestProcess();

    ASSERT_GT(
        pid,
        0
    );

    CpuManager cpuManager(
        static_cast<std::size_t>(
            cpu + 1
        )
    );

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            pid
        )
    );

    process.setAssignedCpus({
        cpu
    });

    ASSERT_TRUE(
        dispatcher.start(process)
    );

    ASSERT_TRUE(
        dispatcher.pause(process)
    );

    EXPECT_TRUE(
        dispatcher.terminate(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::TERMINATED
    );

    int status = 0;

    ASSERT_EQ(
        waitpid(
            pid,
            &status,
            0
        ),
        pid
    );

    EXPECT_TRUE(
        WIFSIGNALED(status)
    );
}


TEST(CpuDispatcherTest, TerminatePreemptedProcess) {
    int cpu = FindAvailableCpu();

    ASSERT_GE(
        cpu,
        0
    );

    pid_t pid = CreateTestProcess();

    ASSERT_GT(
        pid,
        0
    );

    CpuManager cpuManager(
        static_cast<std::size_t>(
            cpu + 1
        )
    );

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            pid
        )
    );

    process.setAssignedCpus({
        cpu
    });

    ASSERT_TRUE(
        dispatcher.start(process)
    );

    ASSERT_TRUE(
        dispatcher.preempt(process)
    );

    EXPECT_TRUE(
        dispatcher.terminate(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::TERMINATED
    );

    int status = 0;

    ASSERT_EQ(
        waitpid(
            pid,
            &status,
            0
        ),
        pid
    );

    EXPECT_TRUE(
        WIFSIGNALED(status)
    );
}


TEST(CpuDispatcherTest, CannotTerminateTerminatedProcess) {
    int cpu = FindAvailableCpu();

    ASSERT_GE(
        cpu,
        0
    );

    pid_t pid = CreateTestProcess();

    ASSERT_GT(
        pid,
        0
    );

    CpuManager cpuManager(
        static_cast<std::size_t>(
            cpu + 1
        )
    );

    CpuDispatcher dispatcher(
        cpuManager
    );

    ProcessHandle process(
        1,
        static_cast<std::int64_t>(
            pid
        )
    );

    process.setAssignedCpus({
        cpu
    });

    ASSERT_TRUE(
        dispatcher.start(process)
    );

    ASSERT_TRUE(
        dispatcher.terminate(process)
    );

    EXPECT_FALSE(
        dispatcher.terminate(process)
    );

    EXPECT_EQ(
        process.getState(),
        ProcessState::TERMINATED
    );

    int status = 0;

    ASSERT_EQ(
        waitpid(
            pid,
            &status,
            0
        ),
        pid
    );
}