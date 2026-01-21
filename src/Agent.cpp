#include "Agent.hpp"
#include "Channel.hpp"
#include <algorithm>

const uint8_t Agent::DIFS = 3; 
const uint16_t Agent::CWTable[Agent::CWLevels] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 };

Agent::Agent(uint32_t id, float pPacketGen)
    : id(id), pPacketGen(pPacketGen), rng() 
{
    contentionWindow = CWTable[0];
    
    // * random initial backoff to avoid initial collision
    backoffCounter = rng.uniform_int(0, contentionWindow - 1);
}

void Agent::generatePacketIfNeeded() {
    if (hasPacket()) return;
    if (rng.uniform_real() < pPacketGen) {
        packetDuration = rng.uniform_int(2, 5); 
    }
}

bool Agent::tickBehaviour(Channel& channel) {
    generatePacketIfNeeded();
    if (!hasPacket()) return false;

    switch (state) {
    case State::Idle:
        state = State::IFS;
        ifsCounter = DIFS;
        return false;    

    case State::IFS:
        if (!channel.isIdle()) {
            ifsCounter = DIFS; 
            return false;
        }
        if (--ifsCounter > 0) return false;
        selectBackoff(); 
        state = State::Backoff;
        return false;

    case State::Backoff:
        if (!channel.isIdle()) return false; 
        if (--backoffCounter > 0) return false;
        state = State::Transmitting;
        channel.addTransmittingNode(this);
        return true; 

    case State::Transmitting:
        return false;
    }
    return false;
}

// ** BASELINE

void BaselineAgent::selectBackoff() {
    backoffCounter = rng.uniform_int(0, contentionWindow - 1);
}

void BaselineAgent::onSuccess() {
    contentionWindow = CWTable[0]; 
    packetDuration = 0;            
    state = State::Idle;
}

void BaselineAgent::onCollision() {
    uint16_t maxCW = CWTable[Agent::CWLevels - 1];
    contentionWindow = std::min((uint16_t)(contentionWindow * 2), maxCW);
    selectBackoff();      
    state = State::Backoff; 
}


// ** Multi-Armed Bandit

MABAgent::MABAgent(uint32_t id, float pPacketGen, double epsilon, MABDecisionMode decision) 
: Agent(id, pPacketGen), epsilon(epsilon), decision(decision) {
    qValues.assign(CWLevels, 0.0);
    counts.assign(CWLevels, 0);
    ticksAlive = 0;
    
    // * init high values to encourage exploring all windows at least once
    for(int i = 0; i < CWLevels; i++) {
        qValues[i] = 10.0 + (double)i; 
    }
}

void MABAgent::selectBackoff() {
    ticksAlive++;
    int bestAction = 0;

    if (decision == MABDecisionMode::UCB1) {
        double maxBound = -1e9;

        for (int i = 0; i < CWLevels; i++) {
            double score;
            if (counts[i] == 0) {
                // * this forces the exploration of unused windows, thanks gpt
                score = 1e9; 
            } else {
                // * exploration_bonus increases for arms we haven't tried in a while
                double explorationBonus = sqrt(explorationConstant * log((double)ticksAlive) / (double)counts[i]);
                score = qValues[i] + explorationBonus;
            }

            if (score > maxBound) {
                maxBound = score;
                bestAction = i;
            }
        }

        currentActionIdx = bestAction;
        contentionWindow = CWTable[currentActionIdx];
        backoffCounter = rng.uniform_int(0, contentionWindow - 1);
    
    } else if (decision == MABDecisionMode::EGreedy) {
        float randomVal = (float)rng.uniform_real();

        if (randomVal < epsilon) {
            bestAction = rng.uniform_int(0, CWLevels - 1);
        } else {
            double maxQ = -1e9;
            for (int i = 0; i < CWLevels; i++) {
                if (qValues[i] > maxQ) {
                    maxQ = qValues[i];
                    bestAction = i;
                }
            }
        }

        currentActionIdx = bestAction;
        contentionWindow = CWTable[currentActionIdx];
        backoffCounter = rng.uniform_int(0, contentionWindow - 1);
    }
    
}

void MABAgent::updateQ(double reward) {
    counts[currentActionIdx]++;
    // * standard incremental mean update
    double n = (double)counts[currentActionIdx];
    qValues[currentActionIdx] = qValues[currentActionIdx] + (1.0 / n) * (reward - qValues[currentActionIdx]);
}

void MABAgent::onSuccess() {
    // * reward slightly more for larger windows for stability
    double reward = 20.0 + (double)currentActionIdx;
    updateQ(reward);
    
    packetDuration = 0;
    state = State::Idle;
}

void MABAgent::onCollision() {
    // * penalty for collisions, especially at low CW
    double penalty = -10.0 * (CWLevels - currentActionIdx); 
    updateQ(penalty);
    
    selectBackoff();
    state = State::Backoff;
}

// * QLearning

QLearningAgent::QLearningAgent(uint32_t id, float pPacketGen, double epsilon, QLearningAlpha alphaSetting) 
: Agent(id, pPacketGen), epsilon(epsilon), alphaSetting(alphaSetting) {
    currentState = 0;
    alpha = 0.1;   // * LR
    gamma = 0.8;   // * discount factor
    
    qTable.resize(CWLevels, std::vector<double>(CWLevels, 0.0));

    // * this is for dynamic alpha
    visitCount.resize(CWLevels, std::vector<uint32_t>(CWLevels, 0));
    
    // * optimistic initialization for larger windows to encourage safety
    for(int s = 0; s < CWLevels; s++) {
        for(int a = 0; a < CWLevels; a++) {
            qTable[s][a] = (double)a * 2.0; 
        }
    }
}

void QLearningAgent::selectBackoff() {
    // ! Boltzmann Exploration (Softmax)
    double temperature = 1.0; 
    std::vector<double> expValues(CWLevels);
    double sumExp = 0.0;
    
    for(int i = 0; i < CWLevels; i++) {
        expValues[i] = exp(qTable[currentState][i] / temperature);
        sumExp += expValues[i];
    }

    double roll = rng.uniform_real() * sumExp;
    double cumulative = 0.0;
    for(int i = 0; i < CWLevels; i++) {
        cumulative += expValues[i];
        if(roll <= cumulative) {
            currentActionIdx = i;
            break;
        }
    }

    contentionWindow = CWTable[currentActionIdx];
    backoffCounter = rng.uniform_int(0, contentionWindow - 1);
}

void QLearningAgent::onSuccess() {
    double reward = 20.0 + (double)currentActionIdx; 
    
    // * soft Reset: Move back one state instead of jumping to 0
    int nextState = std::max(0, currentState - 1); 
    
    double maxNextQ = *std::max_element(qTable[nextState].begin(), qTable[nextState].end());
    
    // * for dynamic alpha
    if (alphaSetting == QLearningAlpha::Dynamic) {
        visitCount[currentState][currentActionIdx]++;
        double alpha = 1.0 / (1.0 + visitCount[currentState][currentActionIdx]);
    }

    // * bellman equation
    qTable[currentState][currentActionIdx] += alpha * (reward + gamma * maxNextQ - qTable[currentState][currentActionIdx]);

    currentState = nextState;
    packetDuration = 0;
    state = State::Idle;
}

void QLearningAgent::onCollision() {
    // ! makes aggressive windows extremely unattractive
    double penalty = -pow(2, (CWLevels - currentActionIdx)); 

    int nextState = std::min(currentState + 1, CWLevels - 1);
    double maxNextQ = *std::max_element(qTable[nextState].begin(), qTable[nextState].end());

    if (alphaSetting == QLearningAlpha::Dynamic) {
        visitCount[currentState][currentActionIdx]++;
        alpha = 1.0 / (1.0 + visitCount[currentState][currentActionIdx]);
    }

    // * bellman equation
    qTable[currentState][currentActionIdx] += alpha * (penalty + gamma * maxNextQ - qTable[currentState][currentActionIdx]);

    currentState = nextState;
    selectBackoff();
    state = State::Backoff;
}