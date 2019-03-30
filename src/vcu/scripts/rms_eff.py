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
date = 20180615
time1 = 191103
time2 = 193838
time3 = 194829
full_path = base.joinpath(str(date), "numpy", str(time2) + ".npz")

rms_eff_percent = 97
num_pole_pairs = 10.0
flux = 355

motor_constant_10e4 = 3 * num_pole_pairs * flux / 2
plim = 5

d = np.load(full_path)

def shift(arr, n=1):
    return np.array([0] * n + list(arr[:-n]))

def plot(data, val_name, label=None, mult=1):
    if label is None:
        label = val_name.replace('_', ' ').capitalize()
    plt.plot(data['time'] - min_time, mult * data[val_name], label=label)

# Load data
power = d['CurrentSensor_Power']
mc_voltage = d['MCVoltage']
flux_info = d['MCFluxInfo']
flux_command = d['MCModulationIndxFluxWeakeningInfo']
trq_cmd = d['MCCommand']
spd = d['MCMotorPositionInfo']


min_time = min(
    min(power['time']),
    min(mc_voltage['time']),
    min(flux_info['time']),
    min(flux_command['time']),
    min(trq_cmd['time']),
    min(spd['time'])
)

# Do calcs
# interp_vq = interpolate.interp1d(mc_voltage['time'], mc_voltage['VBC_Vq'], fill_value="extrapolate")

# vq_at_iq_fb_times = np.array([interp_vq(i) for i in flux_info['time']])
# vq_at_iq_com_times = np.array([interp_vq(i) for i in flux_command['time']])

vq_at_iq_fb_times = np.interp(flux_info['time'], mc_voltage['time'], mc_voltage['VBC_Vq'])

vq_iq_fb = vq_at_iq_fb_times * flux_info['Iq_feedback'] / 100

flux_time = flux_info['time'] - min_time

# interp_trq = interpolate.interp1d(trq_cmd['time'], trq_cmd['torque'], fill_value="extrapolate")
# trq_at_iq_fb_times = np.array([interp_trq(i) for i in flux_info['time']])

trq_at_iq_fb_times = np.interp(flux_info['time'], trq_cmd['time'], trq_cmd['torque'])

# interp_spd = interpolate.interp1d(spd['time'], spd['motor_speed'], fill_value="extrapolate")
# spd_at_iq_fb_times = np.array([interp_spd(i) for i in flux_info['time']])

spd_at_iq_fb_times = np.interp(flux_info['time'], spd['time'], spd['motor_speed'])

# interp_pwr = interpolate.interp1d(power['time'], power['result'], fill_value="extrapolate")
# pwr_at_iq_fb_times = np.array([interp_pwr(i) for i in flux_info['time']])

pwr_at_iq_fb_times = np.interp(flux_info['time'], power['time'], power['result'])
# pwr_at_iq_fb_times[pwr_at_iq_fb_times == 0] = 0.001

eff = vq_iq_fb / pwr_at_iq_fb_times * 100

# Plot
# plot(power, "result", label="Power (W)", mult=0.01)
# plot(flux_info, "Iq_feedback", mult=-0.1)
# plot(mc_voltage, "VBC_Vq", label="$V_q$", mult=-0.1)
# # plt.plot(flux_time, vq_iq_fb, label="$V_qI_q$ power (feedback)")
# plt.plot(flux_time, vq_iq_fb / pwr_at_iq_fb_times * 100, label="Efficiency")

# plot(spd, "motor_speed")
# plt.plot(flux_time, spd_at_iq_fb_times, label="Interp spd", linestyle='--')

# plot(trq_cmd, "torque")
# plt.plot(flux_time, trq_at_iq_fb_times, label="Interp trq", linestyle='--')

np.nan_to_num(eff, copy=False)
np.clip(eff, -1, 1, out=eff)

print("Min efficiency:", np.nanmin(eff))
print("Max efficiency:", np.nanmax(eff))

plt.scatter(-spd_at_iq_fb_times, trq_at_iq_fb_times, c=eff, cmap='viridis')

plt.show()
