/**
 * @file       Aes.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include <Aes.hpp>
#include <string.h>

#include "Semaphore.hpp"

#include "cc2538_include.h"
#include "platform_types.h"



/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Aes::Aes(void)
{
}

void Aes::enable(void)
{
	SysCtrlPeripheralReset(SYS_CTRL_PERIPH_AES);
    SysCtrlPeripheralEnable(SYS_CTRL_PERIPH_AES);
}

void Aes::reset(void)
{
	enable();
}

bool Aes::sleep(void)
{
	return true;
}

bool Aes::wakeup(void)
{
	loadKey(key_);

	return true;
}

bool Aes::loadKey(uint8_t key[16])
{
	uint8_t status;

	// Store the key in non-volatile RAM
	memcpy(key_, key, 16);

	// Load the key at a given location
	status = AESLoadKey((uint8_t *) key_, KEY_AREA_0);
	
	// Check for AES status
	if (status != AES_SUCCESS)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Aes::encrypt(uint8_t* input, uint8_t* output, uint32_t length)
{
	// Payload has to be multiple of 16
	if (length % 16 != 0)
	{
		return false;
	}
	else
	{
		// Perform the encryption
		return Aes::processBuffer(input, output, length, true);
	}
}

bool Aes::decrypt(uint8_t* input, uint8_t* output, uint32_t length)
{
	// Payload has to be multiple of 16
	if (length % 16 != 0)
	{
		return false;
	}
	else
	{
		// Perform the decrỳption
		return Aes::processBuffer(input, output, length, false);
	}
}

/*=============================== protected =================================*/

void Aes::interruptHandler(void)
{
}

/*================================ private ==================================*/

bool Aes::processBuffer(uint8_t* input, uint8_t* output, uint8_t length, bool encrypt)
{
    uint8_t blocks;
    bool status = false;

    // Number blocks has to be multiple of 16
    blocks = length >> 4;

    // For each block encrypt or decrypt
    for (uint8_t n = 0; n < blocks; n++) {
        status = Aes::processBlock(&input[16 * n], &output[16 * n], KEY_AREA_0, encrypt);
        if (!status) break;
    }

    return status;
}

bool Aes::processBlock(uint8_t* input, uint8_t* output, uint8_t key, bool encrypt)
{
	// Encrypt a data block
	if (AESECBStart(input, output, key, encrypt, false) == AES_SUCCESS)
	{
		do {
			ASM_NOP;
		} while (!(AESECBCheckResult()));

		if (AESECBGetResult() == AES_SUCCESS)
		{
			return true;
		}
	}

	return false;
}
