import numpy as np
from matplotlib import pyplot as plt

d = np.load(r'C:\Users\alvar\Dropbox (MIT)\MIT\Fall 2018\FSAE\Data\Parsed\20190111\numpy\175913.npz')
#171114.npz
#for key, value in d.items()
#    print (key, value)

power = d['CurrentSensor_Power']
current = d['CurrentSensor_Current']
tCMD = d['MCCommand']
tMAX = d['PowerLimMonitoring']

motorSpeed = d['MCMotorPositionInfo']
plt.plot(tCMD['time'],tCMD['torque'])
plt.plot(power['time'], power['result'])
plt.plot(motorSpeed['time'], -motorSpeed['motor_speed'])
#plt.plot(tMAX['time']-tMAX['time'][0] ,tMAX['tMAX'])
#plt.plot(tMAX['time']-tMAX['time'][0] ,tMAX['tCAP'])
plt.legend( ('tCMD', 'Power','Motor Speed', 'tMAX', 'tCAP'))
#power['time'] = power['time']-power['time'][0]

#print(power['time'][idx])
#print(power['result'][idx]/25)
plt.show()

maxPower = max(power['result'])
print('Max power [W]: '+str(maxPower))
index = np.where(power['result']==maxPower)
print(index[0][0]) #get index only
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
#for i in range(maxPowerIndex-8,maxPowerIndex+8):
#    print('Index: '+str(i) + ' Power [W]: '+str(power['result'][i]))
for i in range(6600,6620):
    print('Index: '+str(i) + ' Motor Speed [RPM]: '+str(motorSpeed['motor_speed'][i]))