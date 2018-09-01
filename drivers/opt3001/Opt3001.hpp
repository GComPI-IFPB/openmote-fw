/**
 * @file       Opt3001.hpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef OPT3001_HPP_
#define OPT3001_HPP_

#include <stdint.h>
#include <stdbool.h>

#include "I2c.hpp"

#define OPT3001_I2C_ADDR_PRIM       ( 0x47 )
#define OPT3001_I2C_ADDR_SEC        ( 0x48 )

class Opt3001
{

public:
    Opt3001(I2c& i2c, uint8_t address);
    bool init(void);
    bool enable(void);
    bool disable(void);
    bool read(uint16_t* raw);
    void convert(uint16_t raw, float* lux);
    bool test(void);
private:
    I2c i2c_;
    uint8_t address_;
};

#endif /* OPT3001_HPP_ */
