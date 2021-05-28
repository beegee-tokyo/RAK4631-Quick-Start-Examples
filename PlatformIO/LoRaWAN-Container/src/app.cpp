/**
 * @file app.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Application specific functions. Mandatory to have init_app(), 
 *        app_event_handler(), ble_data_handler(), lora_data_handler()
 *        and lora_tx_finished()
 * @version 0.1
 * @date 2021-04-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "main.h"

char ble_dev_name[10] = "RAK";

uint32_t packet_counter = 0;

/**
 * @brief Application specific setup functions
 * 
 */
void setup_app(void)
{
	// Called in the very beginning of setup
}

/**
 * @brief Application specific initializations
 * 
 * @return true Initialization success
 * @return false Initialization failure
 */
bool init_app(void)
{
	// Add your application specific initialization here
	return true;
}

/**
 * @brief Application specific event handler
 *        Requires as minimum the handling of STATUS event
 * 
 */
void app_event_handler(void)
{
	// Timer triggered event
	if ((g_task_event_type & STATUS) == STATUS)
	{
		g_task_event_type &= N_STATUS;
		MYLOG("APP", "Timer wakeup");

		/**************************************************************/
		/**************************************************************/
		/// \todo read sensor or whatever you need to do frequently
		/// \todo write your data into a char array
		/// \todo call LoRa P2P send_lora_packet()
		/**************************************************************/
		/**************************************************************/

		uint8_t collected_data[8] = {0};
		uint8_t data_size = 0;
		collected_data[data_size++] = 'C';
		collected_data[data_size++] = 'N';
		collected_data[data_size++] = 'T';
		collected_data[data_size++] = ':';
		collected_data[data_size++] = ' ';
		char pck_cnt[6] = {0};
		int len = sprintf(pck_cnt, "%ld", packet_counter);
		for (int i = 0; i < len; i++)
		{
			collected_data[data_size++] = pck_cnt[i];
		}
		packet_counter++;
		send_lora_packet(collected_data, data_size);

		MYLOG("APP", "LoRa package sent");
	}
}

/**
 * @brief Handle BLE UART data
 * 
 */
void ble_data_handler(void)
{
#ifndef BLE_DIS
	// BLE UART data handling
	if ((g_task_event_type & BLE_DATA) == BLE_DATA)
	{
		/**************************************************************/
		/**************************************************************/
		/// \todo BLE UART data arrived
		/// \todo parse them here
		/**************************************************************/
		/**************************************************************/
		g_task_event_type &= N_BLE_DATA;
		String uart_rx_buff = ble_uart.readStringUntil('\n');

		uart_rx_buff.toUpperCase();

		MYLOG("BLE", "BLE Received %s", uart_rx_buff.c_str());
	}
#endif
}

/**
 * @brief Handle received LoRa Data
 * 
 */
void lora_data_handler(void)
{
	// LoRa data handling
	if ((g_task_event_type & LORA_DATA) == LORA_DATA)
	{
		/**************************************************************/
		/**************************************************************/
		/// \todo LoRa data arrived
		/// \todo parse them here
		/**************************************************************/
		/**************************************************************/
		g_task_event_type &= N_LORA_DATA;
		MYLOG("APP", "Received package over LoRa");
		char log_buff[g_rx_data_len * 3] = {0};
		uint8_t log_idx = 0;
		for (int idx = 0; idx < g_rx_data_len; idx++)
		{
			sprintf(&log_buff[log_idx], "%02X ", g_rx_lora_data[idx]);
			log_idx += 3;
		}
		MYLOG("APP", "%s", log_buff);

		if (ble_uart_is_connected)
		{
			for (int idx = 0; idx < g_rx_data_len; idx++)
			{
				ble_uart.printf("%02X ", g_rx_lora_data[idx]);
			}
			ble_uart.println("");
		}
	}
}

/**
 * @brief Callback from LoRa events after TX finished
 * 
 * @param success true if TX success, else false
 */
void lora_tx_finished(bool success)
{
	MYLOG("APP", "TX %s", success ? "success" : "failed");
}

/**
 * @brief Callback when RX failed either by timeout or by CRC error
 * 
 */
void lora_rx_failed(void)
{
	MYLOG("APP", "RX failure");
}
