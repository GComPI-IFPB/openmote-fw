'''
@file       at86rf215-ping-pong.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       February, 2019
@brief      

@copyright  Copyright 2019, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

from matplotlib import rcParams
rcParams['font.family'] = 'serif'

import matplotlib.pyplot as plt
import csv

def parse_file(file = None):
    with open(file,'r') as csvfile:
        csv_file = csv.reader(csvfile, delimiter=',')
        csv_lines = [line for line in csv_file]
    
    # Recover X axis titles
    x_axis = csv_lines.pop(0)
    x_axis_title = x_axis[0]
    x_axis = [12-int(b) for b in x_axis[1:]]

    titles = []
    data   = []

    ofdm1_title, ofdm1_data = csv_lines[0][0], csv_lines[0][1:]
    ofdm1_data = [float(b) for b in ofdm1_data]
    titles.append(ofdm1_title)
    data.append(ofdm1_data)

    ofdm2_title, ofdm2_data = csv_lines[1][0], csv_lines[1][1:]
    ofdm2_data = [float(b) for b in ofdm2_data]
    titles.append(ofdm2_title)
    data.append(ofdm2_data)

    ofdm3_title, ofdm3_data = csv_lines[2][0], csv_lines[2][1:]
    ofdm3_data = [float(b) for b in ofdm3_data]
    titles.append(ofdm3_title)
    data.append(ofdm3_data)

    ofdm4_title, ofdm4_data = csv_lines[3][0], csv_lines[3][1:]
    ofdm4_data = [float(b) for b in ofdm4_data]
    titles.append(ofdm4_title)
    data.append(ofdm4_data)

    oqpsk_title, oqpsk_data = csv_lines[4][0], csv_lines[4][1:]
    oqpsk_data = [float(b) for b in oqpsk_data]
    titles.append(oqpsk_title)
    data.append(oqpsk_data)

    return(titles, data, x_axis)

def plot_file(name = "default", extension = "pdf", title = None, legend = None, payload = None, data = None, axis = None):
    fig = plt.figure()

    for y, t in zip(data, legend):
        plt.plot(axis, y, marker='o', label=t)

    plt.gca().invert_xaxis()
    plt.xlabel("SINR (dB)")
    plt.ylabel("PDR (%)")
    #plt.title("PDR for {} interference \n (Interference bandwidth = {} kHz, Packet length = {} bytes)".format(title[0], title[1], payload))
    plt.grid()
    plt.legend(loc='lower left')
    plt.xticks(axis, axis)
    plt.ylim(0,100)

    fig.savefig(name + "." + extension, dpi=600, bbox_inches='tight')

def main():
    extension = ".csv"
    tests = [["OFDM_1_MCS_1_20bytes", "OFDM_2_MCS_2_20bytes",
              "OFDM_3_MCS_3_20bytes", "OFDM_4_MCS_5_20bytes",
              "OQPSK_RATE_5_20bytes"],
             ["OFDM_1_MCS_1_120bytes", "OFDM_2_MCS_2_120bytes",
             "OFDM_3_MCS_3_120bytes",  "OFDM_4_MCS_5_120bytes",
             "OQPSK_RATE_5_120bytes"]]
    tests = [["OQPSK_RATE_5_20bytes"],["OQPSK_RATE_5_120bytes"]]
    payloads = ["20", "120"]
    titles = [["OFDM_1_MCS_1", "1200"],
             ["OFDM_2_MCS_2", "800"],
             ["OFDM_3_MCS_3", "400"],
             ["OFDM_4_MCS_5", "200"],
             ["OQPSK_RATE_5", "2000"]
            ]

    for test, payload in zip(tests, payloads):
        for file, title in zip(test, titles):
            path = file + extension
            legend, data, axis = parse_file(path)
            plot_file(name = file, extension = "png", title = title, payload = payload, legend = legend, data = data, axis = axis)

if __name__ == '__main__':
    main()