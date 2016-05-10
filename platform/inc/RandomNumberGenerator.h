/**
 * @file       RandomNumberGenerator.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef RANDOM_NUMBER_GENERATOR_H_
#define RANDOM_NUMBER_GENERATOR_H_

#include <stdint.h>

class RandomNumberGenerator
{
public:
    RandomNumberGenerator();
    void init(void);
    uint16_t get(void);
};

#endif /* RANDOM_NUMBER_GENERATOR_H_ */
