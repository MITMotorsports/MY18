import matplotlib.pyplot as plt
import csv


def parse_serial_output(filename):
    start_ticks = []
    end_ticks = []
    with open(filename, 'r') as f:
        for line in f.readlines():
            if line.startswith("Start tick: "):
                start_ticks.append(int(line[12:-1]))  # "Start tick: " is 12 letters
            elif line.startswith("End tick: "):
                end_ticks.append(int(line[10:-1]))  # "End tick: " is 12 letters
    tick_nums = [i for i in range(len(start_ticks))]

    differences = [end - start for start, end in zip(start_ticks, end_ticks)]

    plt.plot(tick_nums, differences, 'b-')
    plt.xlabel("Number of iterations")
    plt.ylabel("Difference between start tick and end tick")

    avg_diff = sum(differences)/len(differences)
    plt.title("Avg num difference between start and end ticks: " + str(avg_diff))
    plt.show()


def parse_csv_output(filename):
    tick_nums = []
    start_ticks = []
    end_ticks = []
    with open(filename) as csvfile:
        for row in csv.reader(csvfile):
            tick_nums.append(int(row[0]))
            start_ticks.append(int(row[1]))
            end_ticks.append(int(row[2]))
    differences = [end - start for start, end in zip(start_ticks, end_ticks)]

    plt.plot(tick_nums, differences, 'b-')
    plt.xlabel("Number of iterations")
    plt.ylabel("Difference between start tick and end tick")

    avg_diff = sum(differences)/len(differences)
    plt.title("Avg num difference between start and end ticks: " + str(avg_diff))
    plt.show()


if __name__ == "__main__":
    parse_csv_output("log_delay.csv")
