import numpy as np
from matplotlib import pyplot as plt
from pathlib import Path
from math import pi
from scipy import interpolate
from mpltools import color
plt.rcParams.update({'font.size': 22})

# rad / s * (60 s / min) * (1 rotation / 2pi rad)
RADS_PER_SEC_TO_RPM = 60 / (2*pi)
RPM_TO_RADS_PER_SEC = 1 / RADS_PER_SEC_TO_RPM

color.cycle_cmap(10, cmap='Dark2')

base = Path("/home/dani/Documents/racecar/data")
filepaths = [
    # ("20190327", "112624"),
    # # ("20190327", "032221"),
    # ("20190327", "111226"),
    # # ("20190327", "015447")
    # ("20190327", "163546"),
    # ("20190327", "161913"),
    # ("20190327", "164959"),
    ("20190407", "204714"),
    ("20190407", "205405"),
    ("20190407", "210445"),
    ("20190407", "215219"),
    ("20190407", "222512"),
    ("20190407", "223931"),
]

spds = np.array([])
ids = np.array([])
iq_cmds = np.array([])
iq_fbs = np.array([])
imags = np.array([])

for date, time in filepaths:
    full_path = base.joinpath(str(date), "numpy", str(time) + ".npz")
    d = np.load(full_path)
    
    # Load data
    fluxes = {
        "command": d['MCModulationIndxFluxWeakeningInfo'],
        "feedback": d['MCFluxInfo']
    }
    spd = d['MCMotorPositionInfo']

    for idx, (flux_name, flux_info) in enumerate(fluxes.items()):
        # print(flux_name)

        min_time = min(
            min(flux_info['time']),
            min(spd['time']),
        )

        flux_time = flux_info['time'] - min_time

        # Interpolate
        spd_at_iq_fb_times = np.interp(flux_info['time'], spd['time'], spd['motor_speed'])

        Id = flux_info['_'.join(['Id', flux_name])] / 10
        Iq = -flux_info['_'.join(['Iq', flux_name])] / 10
        # imag = (Id**2 + Iq**2)**0.5

        # # Append to arrays
        # spds = np.append(spds, -spd_at_iq_fb_times)
        # ids = np.append(ids, Id)
        # iqs = np.append(iqs, Iq)
        # imags = np.append(imags, imag)

        label = "{}/{}/{} {}:{}:{}".format(
            date[4:6], date[6:], date[:4], time[:2], time[2:4], time[4:])

        # print("IDX:", idx)

        plt.subplot(2, 2, 2 * idx + 1)
        # print("Plotting on {}{}{}".format(2, idx + 1, 1))
        plt.scatter(-spd_at_iq_fb_times, Id, label=label)
        plt.title(r"$I_d$ " + flux_name)
        plt.ylim(-50, 50)
        plt.xlabel("Speed (RPM)")
        plt.ylabel("Current (A)")

        plt.subplot(2, 2, 2 * idx + 2)
        # print("Plotting on {}{}{}".format(2, idx + 1, 2))
        plt.scatter(-spd_at_iq_fb_times, Iq, label=label)
        plt.title(r"$I_q$ " + flux_name)
        plt.ylim(-50, 250)
        plt.xlabel("Speed (RPM)")
        plt.ylabel("Current (A)")
plt.show()

# plt.subplot(211)
# plt.title(r"$I_d$ feedback")
# plt.xlabel("Speed (RPM)")
# plt.ylabel("Current (A)")
# plt.scatter(spds, ids, c=imags, cmap='plasma', s=20)
# clb = plt.colorbar()
# clb.ax.set_title(r"$\sqrt{I_q^2+I_d^2}$")

# plt.subplot(212)
# plt.title(r"$I_q$ feedback")
# plt.xlabel("Speed (RPM)")
# plt.ylabel("Current (A)")
# plt.scatter(spds, iqs, c=imags, cmap='plasma', s=20)
# clb = plt.colorbar()
# clb.ax.set_title(r"$\sqrt{I_q^2+I_d^2}$")
# plt.show()
