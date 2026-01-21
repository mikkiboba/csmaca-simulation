import pandas as pd
import matplotlib.pyplot as plt
from enum import Enum
import os

class Subject(Enum):
    Node = "nodes"
    Pgen = "pGen"
    Epsilon = "epsilon"

class X(Enum):
    Node = "NODES"
    Pgen = "P_PACK_GEN"
    Epsilon = "EPSILON"

class Y(Enum):
    Throughput = "THROUGHPUT"
    PDR = "PDR"


def plot(files: list[str], strategies: list[str], subject: Subject, x_axis: X, y_axis: Y, x_axis_text: str, y_axis_text: str):

    subject = subject.value
    x_axis = x_axis.value
    y_axis = y_axis.value


    plt.figure(figsize=(15, 8))

    for idx, file in enumerate(files):
        file = f"python/result/{subject}/{file}AVERAGED.csv"
        df = pd.read_csv(file, quotechar='"', skipinitialspace=True)
        df.columns = [c.strip().replace('#', '') for c in df.columns]

        strategy = strategies[idx]
        plt.plot(df[x_axis], df[y_axis], marker='s', label=strategy, linewidth=2)

    plt.title(f"{x_axis} vs {y_axis}", fontsize=18, fontweight='bold')

    if (x_axis == "P_PACK_GEN"):
        plt.xscale('log')

    plt.xlabel(x_axis_text, fontsize=16, fontweight='bold')
    plt.ylabel(y_axis_text, fontsize=16, fontweight='bold')
    plt.legend() 
    plt.grid(True, alpha=0.3)
    plt.savefig(f"img/{subject}/{y_axis}.png", dpi=300)





def main():
    files: list[str] = ["baseline", "MABEGreedy", "MABUCB1", "QLearningFixedAlpha", "QLearningDynamicAlpha"]
    strategies: list[str] = ["Baseline", "Multi-Armed Bandit (epsilon-greedy)", "Multi-Armed Bandit (UCB1)", "Q-Learning (fixed alpha)", "Q-Learning (dynamic alpha)"]
    
    plot(files=files, strategies=strategies, subject=Subject.Node, x_axis=X.Node, y_axis=Y.Throughput, x_axis_text="Number of nodes", y_axis_text="Throughput (Mbps)")
    plot(files=files, strategies=strategies, subject=Subject.Node, x_axis=X.Node, y_axis=Y.PDR, x_axis_text="Number of nodes", y_axis_text="PDR (%)")

    plot(files=files, strategies=strategies, subject=Subject.Pgen, x_axis=X.Pgen, y_axis=Y.Throughput, x_axis_text="Probability to generate a packet", y_axis_text="Throughput (Mbps)")
    plot(files=files, strategies=strategies, subject=Subject.Pgen, x_axis=X.Pgen, y_axis=Y.PDR, x_axis_text="Probability to generate a packet", y_axis_text="PDR (%)")

    plot(files=["EpsilonMABEGreedy"], strategies=["Multi-Armed Bandit (epsilon-greedy)"], subject=Subject.Epsilon, x_axis=X.Epsilon, y_axis=Y.Throughput, x_axis_text="Epsilon", y_axis_text="Throughput (Mbps)")
    plot(files=["EpsilonMABEGreedy"], strategies=["Multi-Armed Bandit (epsilon-greedy)"], subject=Subject.Epsilon, x_axis=X.Epsilon, y_axis=Y.PDR, x_axis_text="Epsilon", y_axis_text="PDR (%)")



if __name__ == "__main__":
    main()