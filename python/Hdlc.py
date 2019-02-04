# -*- coding: utf-8 -*-

# Import Python libraries
import logging
import itertools

# Import OpenMote libraries
import Crc16 as Crc16

# Import logging configuration
logger = logging.getLogger(__name__)

class Hdlc(object):
    HDLC_FLAG           = 0x7e
    HDLC_FLAG_ESCAPED   = 0x5e
    HDLC_ESCAPE         = 0x7d
    HDLC_ESCAPE_ESCAPED = 0x5d
    HDLC_MASK           = 0x20
    
    def __init__(self):
        pass
    
    # Converts a buffer into an HDLC frame
    def hdlcify(self, input = None):
        # Make a copy of the original input
        output = ''.join(input[:])
        
        # Check the CRC checksum
        crc_engine = Crc16.Crc16()
        for o in output:
            crc_engine.push(o)
        crc_result = crc_engine.get()
        
        # Append the CRC checksum
        output += chr((crc_result >> 8) & 0xFF)
        output += chr((crc_result >> 0) & 0xFF)
        
        # Substitute the HDLC flags
        output = output.replace(self.HDLC_ESCAPE, self.HDLC_ESCAPE + self.HDLC_ESCAPE_ESCAPED)
        output = output.replace(self.HDLC_FLAG, self.HDLC_ESCAPE + self.HDLC_FLAG_ESCAPED)
        
        # Append the HDLC flags
        output = self.HDLC_FLAG + output + self.HDLC_FLAG
        
        return output
    
    # Parses an HDLC frame
    # Returns the extracted frame or -1 if wrong CRC checksum
    def dehdlcify(self, frame = None):
        # Check if the input contains HDLC_FLAG
        assert frame[0] == self.HDLC_FLAG
        assert frame[-1] == self.HDLC_FLAG

        # Remove HDLC header and footer
        frame = frame[1:-1]

        # Replace inline HDLC flags
        output = []
        try:
            escape = False
            for f in frame:
                if (escape == True):
                    output.append(f ^ self.HDLC_MASK)
                    escape = False
                else:
                    if (f == self.HDLC_FLAG or
                        f == self.HDLC_ESCAPE):
                        escape = True
                    else:
                        output.append(f)
        except:
            logger.error("dehldicfy: Error replacing HDLC flags.")
            raise

        # Check output input size
        if (len(output) < 2):
            logger.error("dehldicfy: Invalid frame length.")
            raise
        
        # Get the CRC checksum
        try:
            crc_value = output[-1] + output[-2] * 256
        except:
            logger.error('dehldicfy: CRC value error.')
            raise
        
        # Compute the CRC checksum
        try:
            crc_engine = Crc16.Crc16()
            for o in output[:-2]:
                crc_engine.push(o)
            crc_result = crc_engine.get()
            
        except:
            logger.error("dehldicfy: CRC checksum error! crc_value = {}, crc_result = {}".format(hex(crc_value), hex(crc_result)))
            raise

        # Check CRC checksum
        if (crc_value != crc_result):
            logger.error("dehldicfy: CRC values do not match! crc_value = {}, crc_result = {}.".format(hex(crc_value), hex(crc_result)))
            raise
        
        # Remove the CRC checksum
        output = output[:-2]
        
        return output
