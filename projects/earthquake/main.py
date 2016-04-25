'''
@file       main.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       April, 2016
@brief

@copyright  Copyright 2016, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

#!/usr/bin/python

# Import Python libraries
import os
import sys
import time
import getopt
import logging
import logging.config
import numpy as np
import matplotlib.pyplot as plt
import struct
import signal
import random
from collections import deque

# Define path of the OpenMote libraries
library_path = os.path.abspath('../../python/library')
sys.path.append(library_path)

# Import OpenMote libraries
import Serial as Serial

class Earthquake():
  serial_name      = None
  serial_baudrate  = None
  serial_interface = None

  def __init__(self, serial_name = None, baud_rate = None):
      assert serial_name != None, logger.error("Serial port not defined.")
      assert baud_rate   != None, logger.error("Serial baudrate not defined.")
      
      self.serial_name = serial_name
      self.baud_rate   = baud_rate

  def run(self):
    stop = False
    
    # Create the Serial port
    logging.info("run: Creating the Serial port.")
    try:
        self.serial_port = Serial.Serial(serial_name = self.serial_name,
                                         baud_rate = self.baud_rate)
    except:                                   
        return

    # Start the Serial port
    print("- Serial: Listening to port %s at %s bps." % (self.serial_name, self.baud_rate))
    self.serial_port.start()

    ringlen = 100
    x = deque(np.zeros(ringlen, dtype='f'), ringlen)
    y = deque(np.zeros(ringlen, dtype='f'), ringlen)
    z = deque(np.zeros(ringlen, dtype='f'), ringlen)

    fig = plt.figure()

    ax1 = plt.subplot(3, 1, 1)
    ax2 = plt.subplot(3, 1, 2)
    ax3 = plt.subplot(3, 1, 3)

    ax1.grid(True)
    ax2.grid(True)
    ax3.grid(True)

    ax1.set_ylim([-2, 2])
    ax2.set_ylim([-2, 2])
    ax3.set_ylim([-2, 2])

    graph1, = ax1.plot(x)
    graph2, = ax2.plot(y)
    graph3, = ax3.plot(z)

    file = "output.log"
    f = open(file,'w')

    plt.show(block=False)

    while (not stop):
        stop, packet, length = self.serial_port.receive()
        if (length == 6):
          # Unpack the values
          values = struct.unpack('>hhh', packet)
          
          # Convert the values to G
          x_ = values[0] / 256.0
          y_ = values[1] / 256.0
          z_ = values[2] / 256.0

          # Update the data
          x.append(x_)
          y.append(y_)
          z.append(z_)

          # Write to log file
          f.write(str(x_) + "," + str(y_) + "," + str(z_) + "\n")
          f.flush()

          # Update the graph data
          graph1.set_ydata(x)
          graph2.set_ydata(y)
          graph3.set_ydata(z)

          # Update the plot
          fig.canvas.draw()

    # Close the file
    f.close()

    # Stop the serial port    
    self.serial_port.stop()

  def stop(self):
    stop = True
    self.serial_port.stop()

def parse_config(config = None, arguments = None):
    assert config    != None, logger.error("Config not defined.")
    assert arguments != None, logger.error("Arguments not defined.")

    try:
        opts, args = getopt.getopt(arguments, "p:b:")
    except getopt.GetoptError as error:
        print(str(error))
        sys.exit(1)
    
    # Parse command line options
    for option, value in opts:
        if option == '-p':
            config['serial_name'] = value
        elif option == '-b':
            config['baud_rate'] = value
        else:
            assert False, logger.error("Unhandled options while parsing the command line arguments.")
       
    return config

def main():
  default_config = {
      'serial_name' : '/dev/ttyUSB0',
      'baud_rate'   : '576000',
  }
  
  # Parse the command line arguments
  arguments = sys.argv[1:]
  config    = parse_config(default_config, arguments)

  # Create the parser based on user configuration
  earthquake = Earthquake(serial_name = default_config['serial_name'],
                          baud_rate   = default_config['baud_rate'])
  
  try:
    # Execute the thread
    earthquake.run()
  
  except(KeyboardInterrupt, SystemExit):
    try:
        # Stop the thread
        earthquake.stop()
    except SystemExit:
        os._exit(0)
    
if __name__ == "__main__":
  main()