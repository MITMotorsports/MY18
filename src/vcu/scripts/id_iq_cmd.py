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
    # ("20190327", "112624"),
    # # ("20190327", "032221"),
    # ("20190327", "111226"),
    # # ("20190327", "015447")
    ("20190327", "163546"),
    ("20190327", "161913"),
    ("20190327", "164959"),
]

spds = np.array([])
ids = np.array([])
iqs = np.array([])
imags = np.array([])

cmaps = ["cool", "hot"]
count = 0

for date, time in filepaths:
    full_path = base.joinpath(str(date), "numpy", str(time) + ".npz")
    d = np.load(full_path)
    
    # Load data
    flux_info = d['MCModulationIndxFluxWeakeningInfo']
    spd = d['MCMotorPositionInfo']

    min_time = min(
        min(flux_info['time']),
        min(spd['time']),
    )

    flux_time = flux_info['time'] - min_time

    # Interpolate
    spd_at_iq_fb_times = np.interp(flux_info['time'], spd['time'], spd['motor_speed'])

    Id = flux_info['Id_command'] / 10
    Iq = -flux_info['Iq_command'] / 10
    imag = (Id**2 + Iq**2)**0.5

    # Append to arrays
    spds = np.append(spds, -spd_at_iq_fb_times)
    ids = np.append(ids, Id)
    iqs = np.append(iqs, Iq)
    imags = np.append(imags, imag)

    label = "{}/{}/{} {}:{}:{}".format(
        date[4:6], date[6:], date[:4], time[:2], time[2:4], time[4:])

    plt.subplot(2, 1, 1)
    plt.scatter(-spd_at_iq_fb_times, Id, label=label)
    plt.title(r"$I_d$ command")
    plt.xlabel("Speed (RPM)")
    plt.ylabel("Current (A)")

    plt.subplot(2, 1, 2)
    plt.scatter(-spd_at_iq_fb_times, Iq, label=label)
    plt.title(r"$I_q$ command")
    plt.xlabel("Speed (RPM)")

    count += 1
plt.legend()
plt.subplot(211)
plt.legend()
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
