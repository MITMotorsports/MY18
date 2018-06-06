import matplotlib.pyplot as plt
import csv
import argparse


def isNum(str):
    try:
        int(str)
        return True
    except ValueError:
        return False


def parse_csv_output(filename, avg_speed_threshold=None, keep_zeros=False, setpoint=1500):
    if not avg_speed_threshold:
        avg_speed_threshold = setpoint - 1000

    print("Filename: " + filename)
    print("Avg speed thresh: " + str(avg_speed_threshold))
    print("Keep zeros: " + str(keep_zeros))
    print("Setpoint: " + str(setpoint))

    tick_nums = []
    speeds = []
    torques = []
    actual_cmds = []
    actual_ramped_cmds = []

    avg_ticks = []
    avg_speeds = []

    # Bascially just declaring these variables
    period = 1
    k_p = 1

    with open(filename) as csvfile:
        non_zero_found = keep_zeros
        lines = list(csv.reader(csvfile))

        numeric_lines = []
        for line in lines:
            if len(line) == 0:
                continue
            for item in line:
                if not isNum(item):
                    break
            else:  # We went through the whole loop w/o breaking
                numeric_lines.append(line)

        with open("newtest.csv", 'w') as f:
            for line in numeric_lines:
                for i in line[:-1]:
                    f.write(str(i))
                    f.write(',')
                f.write(str(line[-1]))
                f.write('\n')

        period = numeric_lines[0][0]
        k_p = numeric_lines[1][0]

        for row in numeric_lines[2:]:
            if(int(row[1]) != 0 or int(row[2]) != 0):
                non_zero_found = True
            if not non_zero_found:
                continue
            tick_nums.append(int(row[0]))
            speeds.append(int(row[1]))

            if(int(row[1]) > avg_speed_threshold):
                avg_ticks.append(int(row[0]))
                avg_speeds.append(int(row[1]))

            torques.append(int(row[2]))
            actual_cmds.append(int(row[3]))
            actual_ramped_cmds.append(int(row[4]))

    processed_ticks = [tick - min(tick_nums) for tick in tick_nums]
    processed_avg_ticks = [tick - min(tick_nums) for tick in avg_ticks]
    setpoints = [setpoint] * len(processed_ticks)

    avg = int(sum(avg_speeds)/len(avg_speeds))
    avg_arr = [avg] * len(avg_speeds)

    # Create plots
    speed_plt, = plt.plot(processed_ticks, speeds, 'm-', label="Speed")
    torque_plt, = plt.plot(processed_ticks, torques, 'c-', label="Torque CMD")
    setpoint_plt, = plt.plot(processed_ticks, setpoints, 'b-', label="Setpoint (" + str(setpoint) + ")")
    cmds_plt, = plt.plot(processed_ticks, actual_cmds, 'y-', label="I cmd")
    cmds_ramped_plt, = plt.plot(processed_ticks, actual_ramped_cmds, 'g-', label="I cmd (ramp)")
    avg_plt, = plt.plot(processed_avg_ticks, avg_arr, 'r-', label="Average speed (" + str(avg) + ")")
    plt.legend(handles=[speed_plt, torque_plt, setpoint_plt, cmds_plt, cmds_ramped_plt, avg_plt], fontsize='xx-large')

    # Image config
    plt.xlabel("Ticks")

    fig = plt.gcf()
    fig.set_size_inches(20, 15)

    # Add extra info
    txt = r'$k_p$: ' + str(k_p) + '\nPeriod: ' + str(period) + '\n'
    axes = plt.gca()
    img_bot = axes.get_ylim()[0]
    plt.text(0, img_bot, txt, fontsize='xx-large')

    plt.savefig('imgs/' + filename.split('/')[-1][:-4] + '.png')

    plt.show()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("filename")
    parser.add_argument("-kz", "--keep_zeros", action="store_true")
    parser.add_argument("-sp", "--setpoint", type=int)
    parser.add_argument("-at", "--average_threshold", type=int)

    args = parser.parse_args()

    if args.setpoint:
        parse_csv_output(args.filename, args.average_threshold, args.keep_zeros, args.setpoint)
    else:
        parse_csv_output(args.filename, args.average_threshold, args.keep_zeros)
