import csv
from enum import Enum

class Mode(Enum):
    NODES = 1
    PGEN = 3
    EPSILON = 4


def elaborateFile(filename:str, inputFile: str, mode: Mode):
    averagedOutput: list[list[str]] = []
    outputFile = "python/result/" + filename + "AVERAGED.csv"


    with open(inputFile, 'r') as file:
        reader = csv.reader(file)
        header = next(reader)
        while True:
            try:
                strategy = ""
                nodes = ""
                ticks = ""
                pgen = ""
                epsilon = ""
                sum_throughput = 0.0
                sum_pdr = 0.0
                sum_succ_trans = 0
                sum_tot_trans = 0
                for _ in range(3):
                    row = next(reader)
                    print(row)
                    strategy = row[0]
                    nodes = row[1]
                    ticks = row[2]
                    pgen = row[3]
                    epsilon = row[4]

                    sum_succ_trans += int(row[5])
                    sum_tot_trans += int(row[6])

                    sum_throughput += float(row[7])
                    sum_pdr += float(row[8])
                appendolo: list[str] = [strategy, nodes, ticks, pgen, epsilon, str(round(sum_succ_trans/3)), str(round(sum_tot_trans/3)), str(round(sum_throughput/3, 3)), str(round(sum_pdr/3,3))]
                averagedOutput.append(appendolo)


                with open(outputFile, 'w') as file:
                    writer = csv.writer(file)
                    writer.writerow(header)
                    for row in averagedOutput:
                        writer.writerow(row)
            except:
                break


def main():

    mode: Mode = Mode.EPSILON
    files: list[str] = ["baseline", "MABEGreedy", "MABUCB1", "QLearningDynamicAlpha", "QLearningFixedAlpha"]

    folder: str = "data/finalData/"
    if mode == Mode.NODES:
        folder += "nodes/"
    elif mode == Mode.PGEN:
        folder += "pgen/"
    elif mode == Mode.EPSILON:
        files = ["EpsilonMABEGreedy"]
        folder += "epsilon/"

    for file in files:
        url: str = folder + file + ".csv"
        elaborateFile(file, url, mode)



if __name__ == "__main__":
    main()