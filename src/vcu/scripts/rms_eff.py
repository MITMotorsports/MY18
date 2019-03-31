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

# fin_spds = np.linspace(0, 6000, 600, endpoint=False)
# fin_trqs = np.linspace(0, 240, 48, endpoint=False)
eff_dict = {}

TRQ_DELTA = 20
SPD_DELTA = 100

for trq, spd, eff in zip(trqs, spds, effs):
    idx = (int(trq/TRQ_DELTA) * TRQ_DELTA, int(spd/SPD_DELTA) * SPD_DELTA)
    try:
        eff_dict[idx].append(eff)
    except KeyError:
        eff_dict[idx] = [eff]

# fin_trqs = np.linspace(0, 240, 48, endpoint=False)
fin_spds = []
fin_trqs = []
fin_effs = []
for (trq, spd), eff in eff_dict.items():
    if trq < 0:
        continue

    avg_eff = sum(eff)/len(eff)
    fin_trqs.append(trq)
    fin_spds.append(spd)
    fin_effs.append(avg_eff)

plt.subplot(211)
plt.scatter(spds, trqs, c=effs, cmap='plasma', s=20)
plt.colorbar()

plt.subplot(212)
plt.scatter(fin_spds, fin_trqs, c=fin_effs, cmap='plasma', s=20)
plt.colorbar()
plt.show()
