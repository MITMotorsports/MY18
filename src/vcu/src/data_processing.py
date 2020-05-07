import numpy as np
import matplotlib.pyplot as plt
import os

#DAY = "20180916"
DAY = "20180909"
DATA_DIR = "/home/dani/Documents/racecar/testing_data/" + DAY + "/numpy/"
#DATA_FILE = "183444.npz"
#DATA_FILE = "184836.npz"
#data = np.load(DATA_DIR + DATA_FILE)
# DATA_DIR_INTERP = "/home/dani/Documents/racecar/testing_data/20180909/numpy_interp/"
# data_interp = np.load(DATA_DIR_INTERP + DATA_FILE)


def get_column(arr, index):
    return list(i[index] for i in arr)


def plot(data_time_columns):
    plt.plot(data_time_columns[0],data_time_columns[1],label=data_time_columns[2])


# front_wheel_speeds = get_column(data_interp['FrontCanNodeLeftWheelSpeed'],1)
# resolver_speeds = get_column(data_interp['MCMotorPositionInfo'],2)

# plt.plot(resolver_speeds/front_wheel_speeds, label="Slip ratio")

#print(data['MCTorqueTimerInfo'])
#print(get_column(data['MCTorqueTimerInfo'],3)[0])

# 9/9
#DATA_FILES = ["192451.npz"]# #["184836.npz","185112.npz","185654.npz","192046.npz","192323.npz","192451.npz","192553.npz","192738.npz","192844.npz","193041.npz","193221.npz", ]
DATA_FILES = ["193344.npz"] # toruqe
# #,"193505.npz",]

# 9/16
# DATA_FILES = ["123451.npz"]
# DATA_FILES = ["123019.npz"]
#DATA_FILES = ["172219.npz"] # dip
# DATA_FILES = ["112533.npz"] # backwards

# DATA_FILES = os.listdir(DATA_DIR)
for DATA_FILE in DATA_FILES:#192451.npz
    print(DATA_FILE)
    #print(get_column(data['DashRequestLC'],3))
    # print()

    try:
        data = np.load(DATA_DIR + DATA_FILE)

        messages = []
        messages.append([get_column(data['MCTorqueTimerInfo'],0), [i * -1 for i in get_column(data['MCTorqueTimerInfo'],3)], "Feedback torque", ""])
        messages.append([get_column(data['MCCommand'],0), get_column(data['MCCommand'],1), "Commanded speed", " "])
        messages.append([get_column(data['MCCommand'],0), get_column(data['MCCommand'],6), "Commanded torque", " "])
        messages.append([get_column(data['FrontCanNodeLeftWheelSpeed'],0), [i/1000 for i in get_column(data['FrontCanNodeLeftWheelSpeed'],2)], "Front wheel speed",  ""])
        messages.append([get_column(data['MCFluxInfo'],0), get_column(data['MCFluxInfo'],2), "Iq feedback",  ""])
        messages.append([get_column(data['MCFluxInfo'],0), get_column(data['MCFluxInfo'],3), "Flux command",  ""])
        messages.append([get_column(data['MCFluxInfo'],0), get_column(data['MCFluxInfo'],4), "Flux feedback",  ""])
    except:
        continue
    # Find min time
    min_time = -1
    for message in messages:
        if min_time == -1:
            min_time = min(message[0])
        else:
            min_time = min(min_time, min(message[0]))

    # Subtract out min time
    for message in messages:    
        for i, time in enumerate(message[0]):
            message[0][i] = time - min_time

    #print(messages)

    for message in messages:
        plt.plot(message[0], message[1], label=message[2], marker=message[3])


#        plt.plot(, , label="Feedback torque")
#       plt.plot(get_column(data['MCTorqueTimerInfo'],0), [100 for i in get_column(data['MCTorqueTimerInfo'],3)], label="Feedback torque")
#        plt.plot(get_column(data['MCCommand'],0), get_column(data['MCCommand'],1), label="Commanded speed")
#        plt.plot(get_column(data['MCTorqueTimerInfo'],0), get_column(data['MCTorqueTimerInfo'],1), label="Commanded torque")
#        plt.plot(get_column(data['FrontCanNodeLeftWheelSpeed'],0), [i/1000 for i in get_column(data['FrontCanNodeLeftWheelSpeed'],2)], label="Front wheel speed")#, marker='o')
#       plt.plot(get_column(data['MCTorqueTimerInfo'],0), [2400 for i in get_column(data['MCTorqueTimerInfo'],3)], label="Feedback toqrue")
    #plt.plot(get_column(data['DashRequestLC'],0),get_column(data['DashRequestLC'],3), label="Speeding up torque")

    plt.legend()
    plt.ticklabel_format(scientific=False)
    plt.show()
