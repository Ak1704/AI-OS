#pragma once

#include <cstdint>
#include <vector>

namespace aios {

enum class ProcessState {
    CREATED,
    RUNNING,
    PAUSED,
    WAITING,
    TERMINATED
};

class ProcessHandle {
public:
    ProcessHandle();

    explicit ProcessHandle(
        std::uint64_t workloadId
    );

    ProcessHandle(
        std::uint64_t workloadId,
        std::int64_t pid
    );

    std::uint64_t getWorkloadId() const;

    std::int64_t getPid() const;

    ProcessState getState() const;

    void setState(
        ProcessState state
    );

    const std::vector<int>&
    getAssignedCpus() const;

    void setAssignedCpus(
        const std::vector<int>& cpus
    );

    bool isValid() const;

    bool isRunning() const;

    bool isTerminated() const;

private:
    std::uint64_t workloadId_;
    std::int64_t pid_;
    ProcessState state_;
    std::vector<int> assignedCpus_;
};

} // namespace aios