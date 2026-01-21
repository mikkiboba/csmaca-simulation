#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>


class Agent;

class Channel {

public:

    Channel(size_t maxNodes);

    bool isIdle();
    void tickBehaviour();
    void addTransmittingNode(Agent* node);

    size_t successfulTransmissions {0};
    size_t totalTransmissions {0};
    size_t bitTransmitted {0};

    size_t nCollisions {0};

    static constexpr double TICK_DURATION {10e-6};
    static constexpr double PHY_RATE {6e6};

private:
    size_t calculateBitTransmitted(uint32_t duration);

    bool busy;
    uint32_t transmissionDuration;

    std::vector<Agent*> transmittingNodes;

};