#include <fstream>

#include "Environment.hpp"


Environment::Environment(size_t nNodes, float pPacketGen, int strategy, double epsilon)
: nNodes(nNodes),
 pPacketGen(pPacketGen),
 strategy(strategy),
 epsilon(epsilon)
{
    // * generate the nodes
    nodes.reserve(nNodes);

    for (size_t i = 0; i < nNodes; i++) {
        switch (strategy) {
            case 1:
                nodes.emplace_back(std::make_unique<MABAgent>(static_cast<uint32_t>(i), pPacketGen, epsilon, Agent::MABDecisionMode::UCB1));
                break;
            case 2:
                nodes.emplace_back(std::make_unique<MABAgent>(static_cast<uint32_t>(i), pPacketGen, epsilon, Agent::MABDecisionMode::EGreedy));
                break;
            case 3:
                nodes.emplace_back(std::make_unique<QLearningAgent>(static_cast<uint32_t>(i), pPacketGen, epsilon, Agent::QLearningAlpha::Fixed));
                break;
            case 4:
                nodes.emplace_back(std::make_unique<QLearningAgent>(static_cast<uint32_t>(i), pPacketGen, epsilon, Agent::QLearningAlpha::Dynamic));
                break;
            default:
                nodes.emplace_back(std::make_unique<BaselineAgent>(static_cast<uint32_t>(i), pPacketGen));
                break;
        }

    }

    // * generate the single channel
    channel = std::make_unique<Channel>(nNodes);

}


void Environment::run(uint32_t maxTicks) {

    for (uint32_t i = 0; i < maxTicks; i++) {
        for (auto& node : nodes) {
            node->tickBehaviour(*channel);
        }

        channel->tickBehaviour();
    }

    saveData("data/output.csv", maxTicks);

    std::cout << "Successful transmissions: " << channel->successfulTransmissions << std::endl;
    std::cout << "Total transmissions: " << channel->totalTransmissions << std::endl;
    std::cout << "# collisions: " << channel->nCollisions << std::endl;

}


void Environment::saveData(std::string fileName, uint32_t ticks) {
    std::ofstream file(fileName, std::ios::app);

    double_t throughput = calculateThroughput(ticks);
    double_t pdr = calculatePDR();

    // Add Strategy column header if file is empty
    if (file.tellp() == 0) { 
        file << "STRATEGY,NODES,TICKS,P_PACK_GEN,EPSILON,SUCC_TRANS,TOT_TRANS,THROUGHPUT,PDR,COLLISIONS\n";
    }

    file << strategy << ","
         << nNodes << ","
         << ticks << ","
         << pPacketGen << ","
         << epsilon << ","
         << channel->successfulTransmissions << ","
         << channel->totalTransmissions << ","
         << std::fixed << std::setprecision(3) << throughput << ","
         << pdr << ","
         << channel->nCollisions << "\n";
}


double_t Environment::calculateThroughput(uint32_t ticks) {
    size_t bitTransmitted = channel->bitTransmitted;
    // * throughput = bits transmitted / total time
    double totalTime = ticks * Channel::TICK_DURATION;
    
    if (totalTime == 0) return 0.0;

    double_t throughputBps = static_cast<double_t>(bitTransmitted) / totalTime;
    double_t throughputMbps = throughputBps / 1e6;

    return throughputMbps;
}

double_t Environment::calculatePDR() {
    size_t succTx = channel->successfulTransmissions;
    size_t totalTx = channel->totalTransmissions;
    
    if (totalTx == 0) return 0.0;

    double_t pdr = double_t(succTx) / double_t(totalTx);
    return pdr * 100.0;
}

