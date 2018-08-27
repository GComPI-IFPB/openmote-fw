# -*- coding: utf-8 -*-

# Import Python libraries
import logging

# Import OpenMote libraries
import Crc16 as Crc16

# Import logging configuration
logger = logging.getLogger(__name__)

class Hdlc(object):
    HDLC_FLAG           = b'\x7e'
    HDLC_FLAG_ESCAPED   = b'\x5e'
    HDLC_ESCAPE         = b'\x7d'
    HDLC_ESCAPE_ESCAPED = b'\x5d'
    
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
    def dehdlcify(self, input = None):
        # Check if the input contains HDLC_FLAG
        assert input[0] == self.HDLC_FLAG
        assert input[-1] == self.HDLC_FLAG

        # Remove HDLC header and footer
        input = input[1:-1]

        # Replace inline HDLC flags
        try:
            output = []
            for (x, y) in zip(input[0::2], input[1::2]):
                if ((x == self.HDLC_ESCAPE and y == self.HDLC_FLAG_ESCAPED) or 
                    (x == self.HDLC_ESCAPE and y == self.HDLC_ESCAPE_ESCAPED)):
                    output.append(self.HDLC_FLAG)
                else:
                    output.append(x), output.append(y)
        except:
            logging.error("dehldicfy: Error replacing HDLC flags.")
        
        # Check output input size
        if (len(output) < 2):
            logging.error("dehldicfy: Invalid frame length.")
        
        # Get the CRC checksum
        try:
            crc_value = int.from_bytes(b''.join(output[-2:]), 'big')
        except Exception as e:
            logger.error('dehldicfy: CRC value error.')
        
        # Compute the CRC checksum
        try:
            crc_engine = Crc16.Crc16()
            for o in output[:-2]:
                crc_engine.push(o)
            crc_result = crc_engine.get()
        except:
            logger.error('dehldicfy: CRC checksum error.')    

        # Check CRC checksum
        if (crc_value != crc_result):
            logging.error("dehldicfy: CRC values do not match.")
            return None
        
        # Remove the CRC checksum
        output = output[:-2]
        
        return output
