#pragma once

#include "cpu_manager.hpp"
#include "Dispatcher/ProcessHandle.h"

namespace aios {

class Dispatcher {
public:
    Dispatcher() = default;

    virtual ~Dispatcher() = default;

    /*
     * Start an existing process represented by
     * ProcessHandle.
     *
     * The concrete Dispatcher is responsible for:
     *
     * 1. Validating the process.
     * 2. Applying required resources.
     * 3. Transitioning the process to RUNNING.
     */
    virtual bool start(
        ProcessHandle& process
    ) = 0;

    virtual bool pause(
        ProcessHandle& process
    ) = 0;

    virtual bool resume(
        ProcessHandle& process
    ) = 0;

    virtual bool preempt(
        ProcessHandle& process
    ) = 0;

    virtual bool terminate(
        ProcessHandle& process
    ) = 0;

    /*
     * Apply CPU resources assigned to the process.
     */
    virtual bool apply_cpu_assignment(
        ProcessHandle& process
    ) = 0;
};

} // namespace aios