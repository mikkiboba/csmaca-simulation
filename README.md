# CSMA/CA Simulation with Reinforcement Learning
A C++ project submitted for the Autonomous Networking course. It's the simulation of a network of nodes that want to transmit over the same shared channel.

## Transmission Strategies
- Baseline
- Multi-Armed Bandit (Upper Confidence Bound 1)
- Multi-Armed Bandit (Epsilon-Greedy)
- Q-Learning (with fixed learning rate)
- Q-Learning (with dynamic learning rate)

## Run the project
The project is made using CMake and has a few variables that can be set by the user when building the project. 

- `NODES`: the number of nodes in the simulation.
- `TICKS`: the number of ticks the simulation will run for.
- `PGEN`: the probability to generate a packet during a tick.
- `STRATEGY`: the transmission strategy used during the simulation.

An example of build command is the following:

```
cmake -B build -DNODES=1000 -DTICKS=500000 -DPGEN=0.0002 -DSTRATEGY=4
```
