import numpy as np
from matplotlib import pyplot as plt
from pathlib import Path
from math import pi
from scipy import interpolate
plt.rcParams.update({'font.size': 22})

# rad / s * (60 s / min) * (1 rotation / 2pi rad)
RADS_PER_SEC_TO_RPM = 60 / (2*pi)
RPM_TO_RADS_PER_SEC = 1 / RADS_PER_SEC_TO_RPM

base = Path("/home/dani/Documents/racecar/data")
date = 20190330
time1 = "220143"
full_path = base.joinpath(str(date), "numpy", str(time1) + ".npz")

d = np.load(full_path)

power = d['CurrentSensor_Power']
tCMD = d['MCCommand']
settings = d['VCU_PowerLimSettings']
motorSpeed = d['MCMotorPositionInfo']
d = np.load(full_path)

power = d['CurrentSensor_Power']
mc_voltage = d['MCVoltage']

min_time = min(
    min(power['time']),
    min(tCMD['time']),
    min(settings['time'])
)

plt.plot(tCMD['time'] - min_time, tCMD['torque'], label="Torque")
plt.plot(power['time'] - min_time, power['result'], label="Power")
plt.plot(motorSpeed['time'] - min_time, -motorSpeed['motor_speed'], label="Speed")

plt.plot(settings['time'] - min_time, settings['using_vq_lim'] * 600, label="$Vq$ based limit on")
plt.plot(settings['time'] - min_time, settings['using_pl'] * 650, label="Power limiting on")
plt.plot(settings['time'] - min_time, settings['power_lim'] * 100, label="Limit in W")

plt.legend()
plt.show()
