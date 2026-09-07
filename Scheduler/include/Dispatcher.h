#pragma once

#include <cstdint>

namespace aios {

class Dispatcher {
public:
    virtual ~Dispatcher() = default;

    /*
     * Execute a scheduling decision.
     *
     * The actual process execution and resource enforcement
     * will be implemented in later Phase 4 steps.
     */
    virtual bool dispatch(std::uint64_t workloadId) = 0;
};

} // namespace aios