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

# iq_cmds = np.array([])
# iq_fbs = np.array([])

for date, time in filepaths:
    full_path = base.joinpath(str(date), "numpy", str(time) + ".npz")
    d = np.load(full_path)

    flux_info = d['MCFluxInfo']
    flux_cmd = d['MCModulationIndxFluxWeakeningInfo']

    min_time = min(
        min(flux_info['time']),
        min(flux_cmd['time']),
    )

    flux_time = flux_info['time'] - min_time
    cmd_at_iq_fb_times = np.interp(flux_info['time'], flux_cmd['time'], flux_cmd['Iq_command'])

    fb = -flux_info["Iq_feedback"] / 10
    cmd = -cmd_at_iq_fb_times / 10

    # iq_cmds = np.append(iq_cmds, cmd)
    # iq_fbs = np.append(iq_fbs, fb)

    plt.scatter(cmd, fb)

plt.xlabel(r"$I_q$ command (A)")
plt.ylabel(r"$I_q$ feedback (A)")
plt.show()
