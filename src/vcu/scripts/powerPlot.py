import numpy as np
from matplotlib import pyplot as plt
from pathlib import Path


base = Path("/home/dani/Documents/racecar/data")
date = 20190324
time = 174505
full_path = base.joinpath(str(date), "numpy", str(time) + ".npz")

d = np.load(full_path)

power = d['CurrentSensor_Power']
current = d['CurrentSensor_Current']
tCMD = d['MCCommand']
tMAX = d['PowerLimMonitoring']

min_time = min(min(power['time']), min(current['time']), min(tCMD['time']), min(tMAX['time']))

motorSpeed = d['MCMotorPositionInfo']
plt.plot(tCMD['time'] - min_time, tCMD['torque'])
plt.plot(power['time'] - min_time, power['result'])
plt.legend( ('tCMD', 'Power','Motor Speed', 'tMAX', 'tCAP'))
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
