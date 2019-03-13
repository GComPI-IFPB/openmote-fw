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

def swap_title(title = None):
	if (title == "OFDM_1_MCS_1"):
		return "OFDM1-MCS1"
	if (title == "OFDM_2_MCS_2"):
		return "OFDM2-MCS2"
	if (title == "OFDM_3_MCS_3"):
		return "OFDM3-MCS3"
	if (title == "OFDM_4_MCS_5"):
		return "OFDM4-MCS5"
	if (title == "OQPSK_RATE_5"):
		return "OQPSK-DSSS"

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
        title = swap_title(title = title)
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
    #plt.legend(loc='lower left')
    plt.legend()
    plt.grid()
    #plt.show()
    
    # Save plot to file
    fig.savefig(filename, dpi = dpi, bbox_inches = 'tight')

    plt.close()

def plot_file_errors(filename = None, dpi = 600, title = None, data = None, maximum = None, minimum = None, std_dev = None, err_min = None, err_max = None, axis = None, packet_length = None):
    # Use grayscale
    #plt.style.use('grayscale')

    # Create figure
    fig, ax = plt.subplots(1)

    # Define markers
    markers = itertools.cycle(('x', 'o', 'v', 's', 'd')) 

    # Plot data
    for line_name, line_data, line_max, line_min, line_std, line_err_min, line_err_max in zip(data.keys(), data.values(), maximum.values(), minimum.values(), std_dev.values(), err_min.values(), err_max.values()):       
        ax.plot(axis, line_data, marker = next(markers), label = line_name)
        ax.fill_between(axis, line_err_min, line_err_max, alpha=0.25)
        
    # Configure plot
    # plt.title("PDR for {} interference \n (Interference bandwidth = {} kHz, Packet length = {} bytes)".format(title[0], title[1], packet_length))
    plt.xlabel("SINR (dB)")
    plt.xticks(axis, axis)
    plt.yticks(np.arange(0,101, 10))
    plt.ylabel("PDR (%)")
    plt.ylim(0,100)
    #plt.legend(loc='lower left')
    plt.legend()
    plt.grid()
    #plt.show()
    
    # Save plot to file
    fig.savefig(filename, dpi = dpi, bbox_inches = 'tight')

    plt.close()

def calculate_statistics(data = None, experiments = None):
    result_mean = {}
    result_max = {}
    result_min = {}
    result_std = {}
    result_err_max = {}
    result_err_min = {}

    for key, value in data.items():
        tests = []
        local_err_min = 100*np.ones(len(value[0]))
        local_err_max = np.zeros(len(value[0]))
        
        for exp in range(1, experiments + 1):
            scratch = np.asarray(value[exp-1])
            # Get the results from each test
            tests.append(scratch)
            local_err_min = np.minimum(local_err_min, scratch)
            local_err_max = np.maximum(local_err_max, scratch)

        # Convert the results into a matrix transposing columns and lines
        array = np.column_stack(tests)

        local_mean = []
        local_max = []
        local_min = []
        local_std = []
        for line in array:
            mean = np.mean(line)
            maximum = np.max(line)
            minimum = np.min(line)
            std_dev = np.std(line)
            local_mean.append(mean)
            local_max.append(maximum)
            local_min.append(minimum)
            local_std.append(std_dev)
        
        result_mean[key] = local_mean
        result_max[key]  = local_max
        result_min[key]  = local_min
        result_std[key]  = local_std
        result_err_min[key] = local_err_min
        result_err_max[key] = local_err_max

    return (result_mean, result_max, result_min, result_std, result_err_min, result_err_max)

def transform_data(data = None, axis = None):
    axis = axis[0]
    result = {}

    # Arrange data into similar experiments
    for d in data:
        for key, value in d.items():
            if not key in result:
                result[key] = []
            result[key].append(value)

    return (axis, result)

def main():
    data_dir = "data"
    data_extension = ".csv"

    figures_dir = "figures"
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

            exp_axis = []
            exp_data = []

            start = 1
            stop = 4
            experiments = stop - start + 1

            for i in range(1, experiments + 1):
                exp_number = str(i)

                # Generate filename for data and image
                data_file = os.path.join(data_dir, exp_number, file + data_extension)
                
                # Recover data from file
                data, axis = parse_file(filename = data_file)

                exp_data.append(data)
                exp_axis.append(axis)

            axis, data = transform_data(data = exp_data, axis = exp_axis)

            data, maximum, minimum, std_dev, err_min, err_max = calculate_statistics(data = data, experiments = experiments)

            # Generate image name
            image_file = os.path.join(figures_dir, file + figures_extension)

            # Plot the data
            plot_file_errors(filename = image_file, dpi = 600, title = title, data = data, maximum = maximum, minimum = minimum, std_dev = std_dev,
                            err_min = err_min, err_max = err_max, axis = axis, packet_length = packet_length)

if __name__ == '__main__':
    main()