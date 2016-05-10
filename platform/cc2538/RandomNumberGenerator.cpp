/**
 * @file       RandomNumberGenerator.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "RandomNumberGenerator.h"

#include "cc2538_include.h"
#include "cc2538_defines.h"

/*================================ define ===================================*/

#define SOC_ADC_ADCCON1_RCTRL0                  ( 0x00000004 )
#define SOC_ADC_ADCCON1_RCTRL1                  ( 0x00000008 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

RandomNumberGenerator::RandomNumberGenerator()
{
}

void RandomNumberGenerator::init(void)
{
    uint32_t i;
    uint16_t s = 0;

    /* Make sure the RNG is on */
    HWREG(SOC_ADC_ADCCON1) &= ~(SOC_ADC_ADCCON1_RCTRL1 | SOC_ADC_ADCCON1_RCTRL0);

    /* Enable clock for the RF Core */
    HWREG(SYS_CTRL_RCGCRFC) = 1;

    /* Wait for the clock ungating to take effect */
    while(HWREG(SYS_CTRL_RCGCRFC) != 1);

    /* Infinite RX - FRMCTRL0[3:2] = 10
    * This will mess with radio operation - see note above */
    HWREG(RFCORE_XREG_FRMCTRL0) = 0x00000008;

    /* Turn RF on */
    CC2538_RF_CSP_ISRXON();

    /*
    * Wait until "the chip has been in RX long enough for the transients to
    * have died out. A convenient way to do this is to wait for the RSSI-valid
    * signal to go high."
    */
    while(!(HWREG(RFCORE_XREG_RSSISTAT) & RFCORE_XREG_RSSISTAT_RSSI_VALID));

    /*
    * Form the seed by concatenating bits from IF_ADC in the RF receive path.
    * Keep sampling until we have read at least 16 bits AND the seed is valid
    *
    * Invalid seeds are 0x0000 and 0x8003 and should not be used.
    */
    while(s == 0x0000 || s == 0x8003) {
        for(i = 0; i < 16; i++) {
            s |= (HWREG(RFCORE_XREG_RFRND) & RFCORE_XREG_RFRND_IRND);
            s <<= 1;
        }
    }

    /* High byte first */
    HWREG(SOC_ADC_RNDL) = (s >> 8) & 0x00FF;
    HWREG(SOC_ADC_RNDL) = s & 0xFF;

    /* Turn RF off */
    CC2538_RF_CSP_ISRFOFF();
}

uint16_t RandomNumberGenerator::get(void)
{
    uint32_t random;

    /* Clock the RNG LSFR once */
    HWREG(SOC_ADC_ADCCON1) |= SOC_ADC_ADCCON1_RCTRL0;

    random = HWREG(SOC_ADC_RNDL) | (HWREG(SOC_ADC_RNDH) << 8);

    return random;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
