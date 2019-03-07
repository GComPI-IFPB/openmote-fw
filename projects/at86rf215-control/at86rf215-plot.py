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
import os
import itertools

offset = 15

def parse_file(filename = None):
    # Open CSV file and read all lines
    with open(filename,'r') as csvfile:
        csv_file = csv.reader(csvfile, delimiter=',')
        csv_lines = [line for line in csv_file]

    # Create empty dictionary
    x_data = {}
    
    # Recover X axis titles
    x_axis = csv_lines.pop(0)
    x_axis_title = x_axis[0]
    x_axis = [str(offset-int(b)) if b != "0" else "None" for b in x_axis[1:]]

    # Recover data
    for line in csv_lines:    
        title, data = line[0], line[1:]
        data = [float(b) for b in data]
        x_data[title] = data 

    return (x_data, x_axis)

def plot_file(filename = None, dpi = 600, title = None, data = None, axis = None, packet_length = None):
    # Use grayscale
    #plt.style.use('grayscale')

    # Create figure
    fig = plt.figure()

    # Define markers
    markers = itertools.cycle(('x', 'o', 'v', 's', 'd')) 

    # Plot data
    for line_name, line_data in data.items():
        plt.plot(axis, line_data, marker = next(markers), label = line_name)

    # Configure plot
    plt.title("PDR for {} interference \n (Interference bandwidth = {} kHz, Packet length = {} bytes)".format(title[0], title[1], packet_length))
    plt.xlabel("SINR (dB)")
    plt.xticks(axis, axis)
    plt.ylabel("PDR (%)")
    plt.ylim(0,100)
    plt.legend(loc='lower left')
    plt.grid()
    #plt.show()
    
    # Save plot to file
    fig.savefig(filename, dpi = dpi, bbox_inches = 'tight')

def main():
    data_path = "data"
    data_extension = ".csv"

    figures_path = "figures"
    figures_extension = ".png"
    
    experiment_files = [["OFDM_1_MCS_1_20bytes", "OFDM_2_MCS_2_20bytes", "OFDM_3_MCS_3_20bytes", "OFDM_4_MCS_5_20bytes", "OQPSK_RATE_5_20bytes"],
                        ["OFDM_1_MCS_1_120bytes", "OFDM_2_MCS_2_120bytes", "OFDM_3_MCS_3_120bytes", "OFDM_4_MCS_5_120bytes", "OQPSK_RATE_5_120bytes"]
                       ]
    
    packet_lengths = ["20",
                     "120"
                     ]

    plot_titles = [["OFDM1-MCS1", "1200"],
                   ["OFDM2-MCS2", "800"],
                   ["OFDM3-MCS3", "400"],
                   ["OFDM4-MCS5", "200"],
                   ["OQPSK-DSSS", "2000"]
                  ]

    # Run through all tests depending on packet length
    for experiment_file, packet_length in zip(experiment_files, packet_lengths):
        # For each file in the test
        for file, title in zip(experiment_file, plot_titles):
            # Generate filename for data and image
            data_file = os.path.join(data_path, file + data_extension)
            image_file = os.path.join(figures_path, file + figures_extension)
            
            # Recover data from file
            data, axis = parse_file(filename = data_file)

            # Plot the data
            plot_file(filename = image_file, dpi = 600, title = title, data = data, axis = axis, packet_length = packet_length)

if __name__ == '__main__':
    main()