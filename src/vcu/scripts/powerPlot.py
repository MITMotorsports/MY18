import numpy as np
from matplotlib import pyplot as plt
from pathlib import Path
from math import pi
from scipy import interpolate

# rad / s * (60 s / min) * (1 rotation / 2pi rad)
RADS_PER_SEC_TO_RPM = 60 / (2*pi)
RPM_TO_RADS_PER_SEC = 1 / RADS_PER_SEC_TO_RPM

base = Path("/home/dani/Documents/racecar/data")
date = 20190327
time1 = 111226
time2 = 112624
full_path = base.joinpath(str(date), "numpy", str(time1) + ".npz")


rms_eff_percent = 97
num_pole_pairs = 10.0
flux = 355

motor_constant_10e4 = int(3 * num_pole_pairs * flux / 2)
plim = 5

d = np.load(full_path)

def shift(arr, n=1):
    return np.array([0] * n + list(arr[:-n]))

power = d['CurrentSensor_Power']
current = d['CurrentSensor_Current']
tCMD = d['MCCommand']
tMAX = d['PowerLimMonitoring']
mc_voltage = d['MCVoltage']

min_time = min(min(power['time']), min(current['time']), min(tCMD['time']), min(tMAX['time']))

motorSpeed = d['MCMotorPositionInfo']
plt.plot(tCMD['time'] - min_time, tCMD['torque'], label="Torque")
plt.plot(power['time'] - min_time, power['result'], label="Power")
plt.plot(motorSpeed['time'] - min_time, -motorSpeed['motor_speed'], label="Speed")

plt.plot(mc_voltage['time'] - min_time, -(5000.0 * 0.97 * 10 / (mc_voltage['VBC_Vq'])) * motor_constant, label='Vq based toeque lim')

allowed_iq = plim * 100 * rms_eff_percent / (mc_voltage['VBC_Vq'])

plt.plot(mc_voltage['time'] - min_time, -(motor_constant_10e4 * allowed_iq) / 10000, label='Vq based torque lim', linestyle="--")

plt.legend()
plt.show()
