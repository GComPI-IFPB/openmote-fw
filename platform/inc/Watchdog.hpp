/**
 * @file       Watchdog.hpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef WATCHDOG_HPP_
#define WATCHDOG_HPP_

#include <stdint.h>

class Watchdog
{
public:
    Watchdog(uint32_t interval);
    void init(void);
    void walk(void);
private:
    uint32_t interval_;
};

#endif /* WATCHDOG_HPP_ */
