#include "Channel.hpp"
#include "Agent.hpp"

#include <iostream>
#include <algorithm>

Channel::Channel(size_t maxNodes) {
    busy = false;
    transmissionDuration = 0;

    // * instantiate size of vector
    transmittingNodes.reserve(maxNodes);
}


bool Channel::isIdle() { return (transmissionDuration <= 0); }


void Channel::tickBehaviour() {
    if (transmissionDuration > 0) {
        transmissionDuration--;
        return;
    }

    // * there is no transmitting node
    if (transmittingNodes.empty()) {
        return;
    }

    if (transmittingNodes.size() == 1) {
        // * only one node tries to transmit -> transmit
        Agent* node = transmittingNodes[0];
        transmissionDuration = node->getPacketDuration();
        
        successfulTransmissions++;
        totalTransmissions++;
        bitTransmitted += calculateBitTransmitted(transmissionDuration);

        node->onSuccess();

        transmittingNodes.clear();
    } else {
        // * collision
        nCollisions++;
        int32_t maxDuration = 0;
        for (Agent* n : transmittingNodes) {
            totalTransmissions++;
            n->onCollision();
            if ((int32_t)n->getPacketDuration() > maxDuration) {
                maxDuration = (int32_t)n->getPacketDuration();
            }
        }

        transmissionDuration = maxDuration;
        transmittingNodes.clear();
    }
}


void Channel::addTransmittingNode(Agent* node) {
    transmittingNodes.push_back(node);
}


size_t Channel::calculateBitTransmitted(uint32_t duration) {
    size_t bitsPerTick = static_cast<size_t>(PHY_RATE * TICK_DURATION);
    return duration * bitsPerTick;
}