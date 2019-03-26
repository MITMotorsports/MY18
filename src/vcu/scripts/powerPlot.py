import numpy as np
from matplotlib import pyplot as plt
from pathlib import Path
from math import pi

# rad / s * (60 s / min) * (1 rotation / 2pi rad)
RADS_PER_SEC_TO_RPM = 60 / (2*pi)
RPM_TO_RADS_PER_SEC = 1 / RADS_PER_SEC_TO_RPM

base = Path("/home/dani/Documents/racecar/data")
date = 20190324
time = 174505
full_path = base.joinpath(str(date), "numpy", str(time) + ".npz")

d = np.load(full_path)

def shift(arr, n=1):
    return np.array([0] * n + list(arr[:-n]))

power = d['CurrentSensor_Power']
current = d['CurrentSensor_Current']
tCMD = d['MCCommand']
tMAX = d['PowerLimMonitoring']

min_time = min(min(power['time']), min(current['time']), min(tCMD['time']), min(tMAX['time']))

motorSpeed = d['MCMotorPositionInfo']
plt.plot(tCMD['time'] - min_time, tCMD['torque'], label="Torque")
plt.plot(power['time'] - min_time, power['result'], label="Power")
plt.plot(motorSpeed['time'] - min_time, -motorSpeed['motor_speed'], label="Speed")
plt.plot(motorSpeed['time'] - min_time, -motorSpeed['motor_speed'] - shift(-motorSpeed['motor_speed']), label="Delta motor speed")
plt.plot(motorSpeed['time'] - min_time, 5000 / (-motorSpeed['motor_speed']*RPM_TO_RADS_PER_SEC), label="Real torque limit")
plt.legend()
plt.show()

maxPower = max(power['result'])
print('Max power [W]: '+str(maxPower))
index = np.where(power['result']==maxPower)
print(index[0][0])  # get index only
maxPowerIndex = index[0][0]

indexSpeed = np.where(motorSpeed['motor_speed']==323)
print('Index speed: '+ str(indexSpeed[0]))
print('tCMD length: '+str(len(tCMD['torque'])))
print('motorSpeed length: '+str(len(motorSpeed['motor_speed'])))

endtimePower = power['time'][len(power['time'])-1]-power['time'][0]
endTimeMotorSpeed = motorSpeed['time'][len(motorSpeed['time'])-1]-motorSpeed['time'][0]
print('End time [s]: ' + str(endtimePower))
print('Power time step [s]: ' + str(endtimePower/len(power['result'])))
print('Length of motor speed: ' +str(len(motorSpeed['motor_speed'])))
print('Motor speed time step [s]: ' + str(endTimeMotorSpeed/len(motorSpeed['motor_speed'])))
