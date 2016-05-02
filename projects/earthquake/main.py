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

    ringlen = 1600
    rb_x = deque(np.zeros(ringlen, dtype='f'), ringlen)
    rb_y = deque(np.zeros(ringlen, dtype='f'), ringlen)
    rb_z = deque(np.zeros(ringlen, dtype='f'), ringlen)

    fig = plt.figure()

    ax1 = plt.subplot(3, 1, 1)
    ax2 = plt.subplot(3, 1, 2)
    ax3 = plt.subplot(3, 1, 3)

    ax1.grid(True)
    ax2.grid(True)
    ax3.grid(True)

    ax1.set_ylim([-1.5, 1.5])
    ax2.set_ylim([-1.5, 1.5])
    ax3.set_ylim([-1.5, 1.5])

    graph1, = ax1.plot(rb_x)
    graph2, = ax2.plot(rb_y)
    graph3, = ax3.plot(rb_z)

    file = "output.log"
    f = open(file,'w')

    plt.show(block=False)

    while (not stop):
        stop, packet, length = self.serial_port.receive()

        if (length > 0):
          # Unpack the values
          elements = length/2
          values = struct.unpack('<' + 'h' * elements, packet)
          # Iterate over values and append to buffer
          for x, y, z in zip(*[iter(values)]*3):
            # Conver to Gs
            x = x / 256.0
            y = y / 256.0
            z = z / 256.0
            
            # Append to ringbuffer
            rb_x.append(x)
            rb_y.append(y)
            rb_z.append(z)

            # Write to log file
            f.write(str(x) + "," + str(y) + "," + str(z) + "\n")
            f.flush()

        # Update the graph data
        graph1.set_ydata(rb_x)
        graph2.set_ydata(rb_y)
        graph3.set_ydata(rb_z)

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