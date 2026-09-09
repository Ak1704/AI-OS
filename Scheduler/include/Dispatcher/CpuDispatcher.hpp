#pragma once

#include "cpu_manager.hpp"
#include "Dispatcher/Dispatcher.h"


namespace aios {

class CpuDispatcher : public Dispatcher {
public:
    explicit CpuDispatcher(
        CpuManager& cpuManager
    );

    bool start(
        ProcessHandle& process
    ) override;

    bool pause(
        ProcessHandle& process
    ) override;

    bool resume(
        ProcessHandle& process
    ) override;

    bool preempt(
        ProcessHandle& process
    ) override;

    bool terminate(
        ProcessHandle& process
    ) override;

    bool apply_cpu_assignment(
        ProcessHandle& process
    ) override;

private:
    CpuManager& cpuManager_;
};

} // namespace aios