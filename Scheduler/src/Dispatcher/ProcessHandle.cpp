#include "Dispatcher/ProcessHandle.h"

namespace aios {

ProcessHandle::ProcessHandle()
    : workloadId_(0),
      pid_(-1),
      state_(ProcessState::CREATED),
      assignedCpus_() {
}

ProcessHandle::ProcessHandle(
    std::uint64_t workloadId
)
    : workloadId_(workloadId),
      pid_(-1),
      state_(ProcessState::CREATED),
      assignedCpus_() {
}

ProcessHandle::ProcessHandle(
    std::uint64_t workloadId,
    std::int64_t pid
)
    : workloadId_(workloadId),
      pid_(pid),
      state_(ProcessState::CREATED),
      assignedCpus_() {
}

std::uint64_t ProcessHandle::getWorkloadId() const {
    return workloadId_;
}

std::int64_t ProcessHandle::getPid() const {
    return pid_;
}

ProcessState ProcessHandle::getState() const {
    return state_;
}

void ProcessHandle::setState(
    ProcessState state
) {
    state_ = state;
}

const std::vector<int>&
ProcessHandle::getAssignedCpus() const {
    return assignedCpus_;
}

void ProcessHandle::setAssignedCpus(
    const std::vector<int>& cpus
) {
    assignedCpus_ = cpus;
}

bool ProcessHandle::isValid() const {
    return workloadId_ != 0 &&
           pid_ > 0;
}

bool ProcessHandle::isRunning() const {
    return state_ == ProcessState::RUNNING;
}

bool ProcessHandle::isTerminated() const {
    return state_ == ProcessState::TERMINATED;
}

} // namespace aios