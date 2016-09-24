/**
 * @file       SnifferSerial.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include <string.h>

#include "SnifferSerial.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

/*=============================== variables =================================*/

/*================================= public ==================================*/

SnifferSerial::SnifferSerial(Board& board, Radio& radio, Aes& aes, Serial& serial):
    SnifferCommon(board, radio, aes), serial_(serial)
{
}

void SnifferSerial::processRadioFrame(void)
{
    RadioResult result;
    bool status;

    // This call blocks until a radio frame is received
    if (semaphore.take())
    {
        // Get packet from the radio
        radioBuffer_ptr = radioBuffer;
        radioBuffer_len = sizeof(radioBuffer);
        result = radio_.getPacket(radioBuffer_ptr, &radioBuffer_len, &rssi, &lqi, &crc);

        if (result == RadioResult_Success)
        {
            // Turn off the radio
            radio_.off();

            // If the packet needs to be decrypted
            if (decrypt)
            {
                // Decrypt the incoming frame
                status = aes_.decrypt(radioBuffer_ptr, aesBuffer_ptr, radioBuffer_len);
                if (status) {
                    // Make the radio pointer point to the frame
                    radioBuffer_ptr = aesBuffer_ptr;
                }
            }

            // Initialize Serial frame
            outputBuffer_ptr = outputBuffer;
            outputBuffer_len = sizeof(outputBuffer);
            initFrame(radioBuffer_ptr, radioBuffer_len, rssi, lqi, crc);

            // Transmit the radio frame over Serial
            serial_.write(outputBuffer_ptr, outputBuffer_len);
        }
    }
}

/*================================ private ==================================*/
