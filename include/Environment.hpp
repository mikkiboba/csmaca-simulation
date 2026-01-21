#pragma once

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <iomanip>
#include <cmath>

#include "Channel.hpp"
#include "Agent.hpp"


class Environment {
public:
    Environment(size_t nNodes, float pPacketGen, int strategy, double epsilon);

    void run(uint32_t maxTicks);

private:

    

    void saveData(std::string fileName, uint32_t ticks);
    double_t calculateThroughput(uint32_t ticks);
    double_t calculatePDR();

    size_t nNodes;
    float pPacketGen;
    int strategy;
    double epsilon;

    std::vector<std::unique_ptr<Agent>> nodes;
    std::unique_ptr<Channel> channel;
};