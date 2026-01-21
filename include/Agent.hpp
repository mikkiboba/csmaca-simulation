#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#include "Rng.hpp"

class Channel;


class Agent {
public:

    enum class State { Idle, IFS, Backoff, Transmitting };
    enum class MABDecisionMode { UCB1, EGreedy };
    enum class QLearningAlpha { Fixed, Dynamic };

    static const int CWLevels = 10;
    static const uint16_t CWTable[CWLevels];
    static const uint8_t DIFS;

    Agent(uint32_t id, float pPacketGen);
    virtual ~Agent() = default;

    bool tickBehaviour(Channel& channel);
    
    uint32_t getPacketDuration() const { return packetDuration; }
    std::string getId() const { return std::to_string(id); }

    virtual void onSuccess() = 0;
    virtual void onCollision() = 0;
    virtual void selectBackoff() = 0;

    uint32_t ticksAlive {0};

protected:

    uint32_t id;
    float pPacketGen;
    RNG rng;

    State state {State::Idle};
    uint32_t packetDuration {0};

    uint16_t contentionWindow;
    int32_t backoffCounter {0};
    int32_t ifsCounter {0};

    bool hasPacket() const { return packetDuration > 0; }
    void generatePacketIfNeeded();

};


class BaselineAgent : public Agent {
public: 
    BaselineAgent(uint32_t id, float pPacketGen) : Agent(id, pPacketGen) {
        contentionWindow = CWTable[0];
    }


    void onSuccess() override;
    void onCollision() override;
    void selectBackoff() override;
};


class MABAgent : public Agent {
    std::vector<double> qValues;
    std::vector<int> counts;
    int currentActionIdx {0};
    double epsilon {0.2};
    double alpha {0.1};
    double explorationConstant {0.0001};

    MABDecisionMode decision { MABDecisionMode::EGreedy };

public:
    MABAgent(uint32_t id, float pPacketGen, double epsilon, MABDecisionMode decision);

    void onSuccess() override;
    void onCollision() override;
    void selectBackoff() override;
    void updateQ(double reward);
};


class QLearningAgent : public Agent {
    std::vector<std::vector<double>> qTable;
    std::vector<std::vector<uint32_t>> visitCount;
    int currentState {0};
    int currentActionIdx {0};

    double epsilon {0.1};
    double alpha {0.1};
    double gamma {0.9};

public:
    QLearningAgent(uint32_t id, float pPacketGen, double epsilon, QLearningAlpha alphaSetting);

    QLearningAlpha alphaSetting { QLearningAlpha::Fixed };

    void onSuccess() override;
    void onCollision() override;
    void selectBackoff() override;
};