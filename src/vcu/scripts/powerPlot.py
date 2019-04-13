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
date = 20190331
time1 = "033829" # 0323
full_path = base.joinpath(str(date), "numpy", str(time1) + ".npz")

d = np.load(full_path)
grid = np.load("grid.npz")['grid']

NUM_TRQ_INDXS = 120
NUM_SPD_INDXS = 1200

def get_eff_percent(torque, speed):
    torq_idx = int(torque * NUM_TRQ_INDXS / 240)
    spd_idx = int(speed * NUM_SPD_INDXS / 6000)
    return grid[spd_idx][torq_idx] * 100


power = d['CurrentSensor_Power']
tCMD = d['MCCommand']
settings = d['VCU_PowerLimSettings']
spd = d['MCMotorPositionInfo']
monitoring = d['PowerLimMonitoring']

power = d['CurrentSensor_Power']
mc_voltage = d['MCVoltage']

min_time = min(
    min(power['time']),
    min(tCMD['time']),
    min(settings['time']),
    min(monitoring['time'])
)

plt.plot(tCMD['time'] - min_time, tCMD['torque'], label="Torque (dNm)")
plt.plot(power['time'] - min_time, power['result'], label="Power (W)")
plt.plot(spd['time'] - min_time, -spd['motor_speed'], label="Speed (RPM)")

plt.plot(settings['time'] - min_time, settings['using_vq_lim'] * -100, label="$Vq$ based limit on", linestyle=' ')
plt.plot(settings['time'] - min_time, settings['pl_enable'] * -200, label="Power limiting on", linestyle=' ', marker='o')
plt.plot(settings['time'] - min_time, settings['power_lim'] * 100, label="Limit (W)")

plt.plot(monitoring['time'] - min_time, monitoring['calc_eff'], label="Efficiency")


plt.xlabel("Time (s)")

plt.axhline(y=0)

plt.legend()
plt.show()
