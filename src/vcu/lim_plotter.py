import numpy as np
import matplotlib.pyplot as plt

data = np.load("/home/dani/Desktop/Git/Practice/numpy/142757.npy")

times = data[0]
messages = data[1]

print('Using voltage limiting:', any(messages['VCUControlsParams']['using_voltage_limiting']))
print('Using temp limiting:', any(messages['VCUControlsParams']['using_temp_limiting']))
print('Cutoff voltage:', max(messages['VCUControlsParams']['volt_lim_min_voltage']))
print('Cutoff temp:', max(messages['VCUControlsParams']['temp_lim_thresh_temp']))

# Load data
bms_temps = messages['CellTemperatureRange']['max0'] / 10
filtered_temps = messages['VCUControlsMonitoring']['filtered_temp'] / 100
tl_gain = messages['VCUControlsMonitoring']['tl_gain'] / 100

bms_voltages = messages['CellVoltageRange']['min'] / 1000
voltage_used = messages['VCUControlsMonitoring']['voltage_used'] / 100
vl_gain = messages['VCUControlsMonitoring']['vl_gain'] / 100

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
plt.plot(filtered_temps, tl_gain, linestyle='None', marker='o')
plt.xlabel('Filtered temps (degrees C)')
plt.ylabel('TL Gain')

plt.subplot(224)
plt.plot(voltage_used, vl_gain, linestyle='None', marker='o')
plt.xlabel('Voltage used (V)')
plt.ylabel('VL Gain')

plt.show()
