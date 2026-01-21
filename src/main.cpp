#ifndef NODES
#define NODES 100
#endif

#ifndef TICKS 
#define TICKS 500000
#endif

#ifndef PGEN
#define PGEN 0.02
#endif

#ifndef EPSILON
#define EPSILON 0.1
#endif

#ifndef STRATEGY
#define STRATEGY 0
#endif

/*
 * Strategy:
 * 0 -> baseline
 * 1 -> MAB (UCB1)
 * 2 -> MAB (e-greedy)
 * 3 -> QLearning (Fixed alpha)
 * 4 -> QLearning (Dynamic alpha)
 * 
*/


#include <iostream>
#include <cstdlib>

#include "Environment.hpp"


void nodeVersion() {
    uint32_t nodeCount = 500;
    while (nodeCount <= NODES) {
        for (size_t i = 0; i < 3; i++) {
            Environment env = Environment(nodeCount, PGEN, STRATEGY, EPSILON);
            env.run(TICKS);
        }
        nodeCount += 500;
    }
}


void pGenVersion() {
    std::vector<double> p = { 0.0002, 0.0010, 0.0050, 0.0100, 0.0500, 0.1, 0.2, 0.5, 1.0 };

    double pGenCount = 0.0002;
    for (double pGen : p) {
        for (size_t i = 0; i < 3; i++) {
            Environment env = Environment(NODES, pGen, STRATEGY, EPSILON);
            env.run(TICKS);
        }
        pGenCount += 0.0002;
    }
}


void epsilonVersion() {
    double epsilonCount = EPSILON;
    
    while (epsilonCount <= 1.0) {
        for (size_t i = 0; i < 3; i++) {
            Environment env = Environment(NODES, PGEN, 2, epsilonCount);
            env.run(TICKS);
        }
        epsilonCount += 0.1;
    }
}


void expVersion() {
    Environment env = Environment(NODES, PGEN, STRATEGY, EPSILON);
    env.run(TICKS);
}


int main(int argc, char* argv[]) {
    std::cout << "nNodes: " << NODES << std::endl;

    expVersion();
    return 0;
}


