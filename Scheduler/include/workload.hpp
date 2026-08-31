#pragma once

#include <cstdint>
#include <string>

enum class WorkloadType {
    TRAINING,
    INFERENCE,
    DATA_LOADING,
    PREPROCESSING,
    BACKGROUND
};

enum class WorkloadState {
    CREATED,
    READY,
    RUNNING,
    WAITING,
    COMPLETED,
    FAILED
};

struct WorkloadDescriptor {
    std::uint64_t id;

    std::string name;

    WorkloadType type;

    int priority;

    int cpu_cores_required;

    std::uint64_t memory_mb_required;

    WorkloadState state;
};