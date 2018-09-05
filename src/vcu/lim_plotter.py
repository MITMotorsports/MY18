import numpy as np
import matplotlib.pyplot as plt
from os import listdir


# path = "/home/dani/Desktop/Git/Practice/20180622/numpy/122131.npy"
# path = "/home/dani/Desktop/Git/Practice/20180622/numpy/120059.npy"
# path = "/home/dani/Desktop/Git/Practice/20180622/numpy/170543.npy"
basepath = "/home/dani/Desktop/Git/Practice/20180623/numpy/"
paths = listdir(basepath)

for path in paths:
    print("\n")
    try:
        data = np.load(basepath + path)
        print(path)

        times = data[0]
        messages = data[1]

        print('Max torque command: ', max(messages['MCCommand']['torque']))
        print('Using voltage limiting:', any(messages['VCUControlsParams']['using_voltage_limiting']))
        print('Using temp limiting:', any(messages['VCUControlsParams']['using_temp_limiting']))
        print('Cutoff voltage:', max(messages['VCUControlsParams']['volt_lim_min_voltage']))
        print('Cutoff temp:', max(messages['VCUControlsParams']['temp_lim_thresh_temp']))
        print('Max power: ', max(messages['CurrentSensor_Power']['result']))

        # Load data
        bms_temps = messages['CellTemperatureRange']['max0'] / 10
        filtered_temps = messages['VCUControlsMonitoring']['filtered_temp'] / 100
        tl_gain = messages['VCUControlsMonitoring']['tl_gain'] / 100

        bms_voltages = messages['CellVoltageRange']['min'] / 1000
        voltage_used = messages['VCUControlsMonitoring']['voltage_used'] / 100
        vl_gain = messages['VCUControlsMonitoring']['vl_gain'] / 100

        actual_limps = np.divide(messages['MCCommand']['torque'], messages['VCUControlsMonitoring']['raw_torque'])

        # Plot data
        plt.subplot(221)
        bms_temps_plt, = plt.plot(bms_temps)
        filtered_temps_plt, = plt.plot(filtered_temps)
        plt.legend(handles=[bms_temps_plt, filtered_temps_plt], labels=['BMS temps', 'Filtered temps'])
        plt.xlabel('Time (seconds)')
        plt.ylabel('Temperature (degrees C)')

        plt.subplot(222)
        bms_voltages_plt, = plt.plot(bms_voltages)
        voltage_used_plt, = plt.plot(voltage_used)
        plt.legend(handles=[bms_voltages_plt, voltage_used_plt], labels=['BMS voltages', 'Voltages used'])
        plt.xlabel('Time (seconds)')
        plt.ylabel('Voltage (V)')

        plt.subplot(223)
        tl_gain, = plt.plot(filtered_temps, tl_gain, linestyle='None', marker='o')
        tl_limp, = plt.plot(filtered_temps, actual_limps, linestyle='None', marker='o')
        plt.legend(handles=[tl_gain, tl_limp], labels=['TL limp', 'Actual limp'])
        plt.xlabel('Filtered temps (degrees C)')
        plt.ylabel('TL Gain')

        plt.subplot(224)
        vl_gain, = plt.plot(voltage_used, vl_gain, linestyle='None', marker='o')
        vl_limp, = plt.plot(voltage_used, actual_limps, linestyle='None', marker='o')
        plt.legend(handles=[vl_gain, vl_limp], labels=['VL limp', 'Actual limp'])
        plt.xlabel('Voltage used (V)')
        plt.ylabel('VL Gain')

        plt.show()
    except KeyError:
        print("Unknown msg")
