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
import numpy as np

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
    x_axis = [str(offset-int(b)) if b != "0" else "No" for b in x_axis[1:]]

    # Recover data
    for line in csv_lines:    
        title, data = line[0], line[1:]
        #title = swap_title(title = title)
        data = [float(b) for b in data]
        x_data[title] = data 

    return (x_data, x_axis)

def plot_file(filename = None, dpi = 600, data = None, axis = None):
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
    plt.title("Sensitivity")
    plt.xlabel("SINR (dB)")
    plt.xticks(axis, axis)
    plt.ylabel("PDR (%)")
    plt.ylim(0,100)
    #plt.legend(loc='lower left')
    plt.legend()
    plt.grid()
    #plt.show()
    
    # Save plot to file
    fig.savefig(filename, dpi = dpi, bbox_inches = 'tight')

def main():
    data_dir = "sensitivity"
    data_extension = ".csv"

    figures_dir = "figures"
    figures_extension = ".png"

    experiments = [1, 2]
    experiment_files = ["sensitivity_20bytes"]
    
    packet_lengths = ["20", "120"]

    plot_titles = []

    for experiment in experiments:
    	# Run through all tests depending on packet length
    	for experiment_file in experiment_files:
            exp_axis = []
            exp_data = []

            exp_number = str(experiment)

            # Generate filename for data and image
            data_file = os.path.join(data_dir, exp_number, experiment_file + data_extension)
            
            # Recover data from file
            data, axis = parse_file(filename = data_file)

            plot_file(filename = "test" + exp_number + figures_extension, dpi = 600, data = data, axis = axis)

            print(data, axis)


if __name__ == '__main__':
    main()