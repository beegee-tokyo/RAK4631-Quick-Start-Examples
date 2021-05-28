/**
 * @file at_cmd.h
 * @author Taylor Lee (taylor.lee@rakwireless.com)
 * @brief AT command parsing includes & defines
 * @version 0.1
 * @date 2021-04-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "main.h"

#ifndef __AT_H__
#define __AT_H__

#define AT_PRINTF(...)                \
	Serial.printf(__VA_ARGS__);       \
	if (ble_uart_is_connected)        \
	{                                 \
		ble_uart.printf(__VA_ARGS__); \
	}

#endif
