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
filepaths = [
    (20180615, 191103),
    (20180615, 193838),
    (20180615, 194829),
    # (20190330, "040403")
]

def shift(arr, n=1):
    return np.array([0] * n + list(arr[:-n]))

def plot(data, val_name, label=None, mult=1):
    if label is None:
        label = val_name.replace('_', ' ').capitalize()
    plt.plot(data['time'] - min_time, mult * data[val_name], label=label)


spds = np.array([])
trqs = np.array([])
effs = np.array([])

for date, time in filepaths:
    full_path = base.joinpath(str(date), "numpy", str(time) + ".npz")
    d = np.load(full_path)
    
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
    vq_at_iq_fb_times = np.interp(flux_info['time'], mc_voltage['time'], mc_voltage['VBC_Vq'])

    vq_iq_fb = vq_at_iq_fb_times * flux_info['Iq_feedback'] / 100

    flux_time = flux_info['time'] - min_time

    # Interpolate
    trq_at_iq_fb_times = np.interp(flux_info['time'], trq_cmd['time'], trq_cmd['torque'])
    spd_at_iq_fb_times = np.interp(flux_info['time'], spd['time'], spd['motor_speed'])
    pwr_at_iq_fb_times = np.interp(flux_info['time'], power['time'], power['result'])

    # Calc eff
    eff = vq_iq_fb / pwr_at_iq_fb_times
    np.nan_to_num(eff, copy=False)
    np.clip(eff, -1, 1, out=eff)
    eff = np.abs(eff)

    # Append to arrays
    spds = np.append(spds, -spd_at_iq_fb_times)
    trqs = np.append(trqs, trq_at_iq_fb_times/10)
    effs = np.append(effs, eff)

# print("Max speed:", np.max(spds))

# plt.subplot(211)
plt.scatter(spds, trqs, c=effs, cmap='plasma', s=20)
plt.colorbar()
plt.show()

# eff_interp_fin = interpolate.interp2d(spds, trqs, effs)
# _new_spds = np.linspace(0, 6000, 10)
# _new_trqs = np.linspace(0, 240, 10)
# new_spds, new_trqs = np.meshgrid(_new_spds, _new_trqs)
# new_eff = eff_interp_fin(new_spds, new_trqs)
# plt.scatter(new_spds, new_trqs, c=new_eff[0,:], cmap='plasma', s=200)
# grid = interpolate.griddata((spds, trqs), effs, (_new_spds, _new_trqs), method='nearest')

# grid_x, grid_y = np.mgrid[0:6000:60j, 0:240:20j]
# grid = interpolate.griddata((spds, trqs), effs, (grid_x, grid_y), method='nearest')

# plt.subplot(212)
# plt.scatter(grid_x, grid_y, c=grid, cmap='plasma', s=20)
# plt.colorbar()
# plt.show()

# numpy.savez("grid.npz", grid_x=grid_x, grid_y=grid_y, grid=grid)

# for row in grid:
#     print "  {",
#     for val in enumerate(row):
#         print "{},".format(int(val * 1000))
#     print "},"
# print "}"
