#include "Dispatcher/CpuDispatcher.hpp"

#include <csignal>

#include <unistd.h>

namespace aios {

CpuDispatcher::CpuDispatcher(
    CpuManager& cpuManager
)
    : cpuManager_(cpuManager) {
}

bool CpuDispatcher::start(
    ProcessHandle& process
) {
    if (!process.isValid()) {
        return false;
    }

    if (process.getState() !=
        ProcessState::CREATED) {
        return false;
    }

    if (!apply_cpu_assignment(process)) {
        return false;
    }

    process.setState(
        ProcessState::RUNNING
    );

    return true;
}

bool CpuDispatcher::pause(
    ProcessHandle& process
) {
    if (!process.isValid()) {
        return false;
    }

    if (process.getState() !=
        ProcessState::RUNNING) {
        return false;
    }

    if (kill(
            static_cast<pid_t>(
                process.getPid()
            ),
            SIGSTOP
        ) != 0) {

        return false;
    }

    process.setState(
        ProcessState::PAUSED
    );

    return true;
}

bool CpuDispatcher::resume(
    ProcessHandle& process
) {
    if (!process.isValid()) {
        return false;
    }

    /*
     * A process can be resumed after either:
     *
     * PAUSE    -> PAUSED
     * PREEMPT  -> WAITING
     */
    if (process.getState() !=
            ProcessState::PAUSED &&
        process.getState() !=
            ProcessState::WAITING) {

        return false;
    }

    if (kill(
            static_cast<pid_t>(
                process.getPid()
            ),
            SIGCONT
        ) != 0) {

        return false;
    }

    process.setState(
        ProcessState::RUNNING
    );

    return true;
}

bool CpuDispatcher::preempt(
    ProcessHandle& process
) {
    /*
     * PREEMPT is a scheduler-driven operation.
     *
     * Unlike PAUSE, the process becomes WAITING
     * because it is being removed from execution
     * so another workload can use the resources.
     */
    if (!process.isValid()) {
        return false;
    }

    if (process.getState() !=
        ProcessState::RUNNING) {
        return false;
    }

    if (kill(
            static_cast<pid_t>(
                process.getPid()
            ),
            SIGSTOP
        ) != 0) {

        return false;
    }

    process.setState(
        ProcessState::WAITING
    );

    return true;
}

bool CpuDispatcher::terminate(
    ProcessHandle& process
) {
    if (!process.isValid()) {
        return false;
    }

    /*
     * A terminated process cannot be terminated again.
     */
    if (process.getState() ==
        ProcessState::TERMINATED) {
        return false;
    }

    /*
     * Only active lifecycle states can be terminated.
     *
     * CREATED has no running Linux process yet.
     */
    if (process.getState() ==
        ProcessState::CREATED) {
        return false;
    }

    const pid_t pid = static_cast<pid_t>(process.getPid());
    if (process.getState() == ProcessState::PAUSED || 
        process.getState() == ProcessState::WAITING){
            if (kill(pid,SIGCONT)!=0){
                return false;
            }
        }

    /*
     * SIGTERM requests normal process termination.
     */
    if (kill(
            pid,
            SIGTERM
        ) != 0) {

        return false;
    }

    process.setState(
        ProcessState::TERMINATED
    );

    return true;
}

bool CpuDispatcher::apply_cpu_assignment(
    ProcessHandle& process
) {
    if (!process.isValid()) {
        return false;
    }

    const auto& cpus =
        process.getAssignedCpus();

    if (cpus.empty()) {
        return false;
    }

    return cpuManager_.apply_affinity(
        static_cast<int>(
            process.getPid()
        ),
        cpus
    );
}

} // namespace aios