#pragma once

#include <cstdint>
#include <string>
#include <vector>

enum class SchedulingAction {
    RUN,
    WAIT,
    PREEMPT
};

struct SchedulingDecision {
    std::uint64_t workload_id;

    SchedulingAction action;

    std::vector<int> cpu_cores;

    std::string reason;
};