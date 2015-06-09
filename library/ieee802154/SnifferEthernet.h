/**
 * @file       SnifferEthernet.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef SNIFFER_ETHERNET_H_
#define SNIFFER_ETHERNET_H_

#include "SnifferCommon.h"
#include "Ethernet.h"

class SnifferEthernet : public SnifferCommon
{
public:
    SnifferEthernet(Board& board, Radio& radio, Ethernet& ethernet);
    void init(void);
    void processRadioFrame(void);
private:
    void initEthernetFrame(uint8_t* buffer, uint8_t length, int8_t rssi, uint8_t lqi, uint8_t crc);
private:
    Ethernet ethernet_;

    uint8_t macAddress[6];
    static const uint8_t broadcastAddress[6];
    static const uint8_t ethernetType[2];

    uint8_t  ethernetBuffer[255];
    uint8_t* ethernetBuffer_ptr;
    uint32_t ethernetBuffer_len;
};

#endif /* SNIFFER_ETHERNET_H_ */
