import numpy as np
from matplotlib import pyplot as plt
from pathlib import Path
from math import pi
from scipy import interpolate
plt.rcParams.update({'font.size': 22})

# rad / s * (60 s / min) * (1 rotation / 2pi rad)
RADS_PER_SEC_TO_RPM = 60 / (2*pi)
RPM_TO_RADS_PER_SEC = 1 / RADS_PER_SEC_TO_RPM

base = Path("/home/dani/Documents/racecar/data/Parsed")
filepaths = [
    # ("20180615", "191103"),
    # ("20180615", "193838"),
    # ("20180615", "194829"),
    # ("20190330", "220706"),
    # ("20190330", "220143"),
    # ("20190325", "053330"),
    # ("20190325", "153521"),
    # ("20190327", "112624"),
    # ("20190327", "032221"),
    # ("20190327", "111226"),
    # ("20190327", "015447"),
    ("20190327", "163546"),
    ("20190327", "161913"),
    ("20190327", "164959"),
    ("20190327", "170159"),
    # ("20190328", "174251")
]

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
    trq_fb = d['MCTorqueTimerInfo']

    min_time = min(
        min(power['time']),
        min(mc_voltage['time']),
        min(flux_info['time']),
        min(flux_command['time']),
        min(trq_cmd['time']),
        min(spd['time']),
        min(trq_fb['time'])
    )

    # Do calcs
    vq_at_iq_fb_times = np.interp(flux_info['time'], mc_voltage['time'], mc_voltage['VBC_Vq'])
    vd_at_id_fb_times = np.interp(flux_info['time'], mc_voltage['time'], mc_voltage['VAB_Vd'])

    vq_iq_fb = vq_at_iq_fb_times * flux_info['Iq_feedback'] / 100
    vd_id_fb = vq_at_iq_fb_times * flux_info['Id_feedback'] / 100


    flux_time = flux_info['time'] - min_time

    # Interpolate
    trq_at_iq_fb_times = np.interp(flux_info['time'], trq_cmd['time'], trq_cmd['torque'])
    # trq_at_iq_fb_times = np.interp(flux_info['time'], trq_fb['time'], trq_fb['torque_feedback'])
    spd_at_iq_fb_times = np.interp(flux_info['time'], spd['time'], spd['motor_speed'])
    pwr_at_iq_fb_times = np.interp(flux_info['time'], power['time'], power['result'])

    # Calc eff
    eff = (vq_iq_fb) / pwr_at_iq_fb_times
    np.nan_to_num(eff, copy=False)
    np.clip(eff, -1, 1, out=eff)
    eff = np.abs(eff)

    # Append to arrays
    spds = np.append(spds, -spd_at_iq_fb_times)
    trqs = np.append(trqs, trq_at_iq_fb_times/10)
    effs = np.append(effs, eff)

eff_dict = {}

TRQ_DELTA = 20
SPD_DELTA = 200

for trq, spd, eff in zip(trqs, spds, effs):
    idx = (int(trq/TRQ_DELTA) * TRQ_DELTA, int(spd/SPD_DELTA) * SPD_DELTA)
    try:
        eff_dict[idx].append(eff)
    except KeyError:
        eff_dict[idx] = [eff]

fin_spds = []
fin_trqs = []
fin_effs = []
for (trq, spd), eff in eff_dict.items():
    if trq < 0 or spd < 0:
        continue

    avg_eff = sum(eff)/len(eff)
    fin_trqs.append(trq)
    fin_spds.append(spd)
    fin_effs.append(avg_eff)

fin_spds = np.array(fin_spds)
fin_trqs = np.array(fin_trqs)
fin_effs = np.array(fin_effs)

plt.subplot(311)
plt.xlim(0,6100)
plt.ylim(0,250)
plt.title(r"Raw efficiency data $\left(\frac{V_qI_q}{P_{DC}}\right)$")
plt.xlabel("Speed (RPM)")
plt.ylabel("Torque (Nm)")
plt.scatter(spds, trqs, c=effs, cmap='plasma', s=20)
clb = plt.colorbar()
clb.ax.set_title('Efficiency')

plt.subplot(312)
plt.xlim(0,6100)
plt.ylim(0,250)
plt.title("Averaged efficiecy over 20 Nm by 100 RPM chunks")
plt.xlabel("Speed (RPM)")
plt.ylabel("Torque (Nm)")
plt.scatter(fin_spds, fin_trqs, c=fin_effs, cmap='plasma', s=600)
clb = plt.colorbar()
clb.ax.set_title('Efficiency')

plt.subplot(313)
plt.xlim(0,6100)
plt.ylim(0,250)
plt.title("Complete interpolated efficiency map")
plt.xlabel("Speed (RPM)")
plt.ylabel("Torque (Nm)")

TRQ_DELTA = 1
SPD_DELTA = 5

grid_x, grid_y = np.mgrid[0:6000:SPD_DELTA, 0:240:TRQ_DELTA]
grid = interpolate.griddata((fin_spds, fin_trqs), fin_effs, (grid_x, grid_y), method='linear')
plt.scatter(grid_x, grid_y, c=grid, cmap='plasma', s=100)
clb = plt.colorbar()
clb.ax.set_title('Efficiency')

np.nan_to_num(grid, copy=False)
np.clip(grid, 0, 1, out=grid)

np.savez("grid", grid=grid)

# print "{"
# for row in grid:
#     print "  {",
#     for val in row:
#         print "{},".format(int(round(val * 100))),
#     print "},"
# print "}"

# print(grid.shape)

plt.show()
