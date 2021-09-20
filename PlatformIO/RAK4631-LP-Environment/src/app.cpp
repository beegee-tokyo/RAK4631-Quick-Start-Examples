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

#include "app.h"

/** Set the device name, max length is 10 characters */
char g_ble_dev_name[10] = "RAK-ENV";

/** Just for the example we add the number of packets to each LoRaWAN packet */
uint32_t packet_counter = 0;

/** Packet buffer for sending */
uint8_t collected_data[64] = {0};

/** Send Fail counter **/
uint8_t send_fail = 0;

/**
 * @brief Application specific setup functions
 * 
 */
void setup_app(void)
{
	// Called in the very beginning of setup
	/**************************************************************/
	/**************************************************************/
	/// \todo set enable_ble to true if you want to enable BLE
	/// \todo if true, BLE is enabled and advertise 60 seconds
	/// \todo after reset or power-up
	/**************************************************************/
	/**************************************************************/
	g_enable_ble = true;
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
	if (!init_bme680())
	{
		return false;
	}

	return true;
}

/**
 * @brief Application specific event handler
 *        Requires as minimum the handling of STATUS event
 *        Here you handle as well your application specific events
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
		/// \todo Just as example, if BLE is enabled and you want
		/// \todo to restart advertising on an event you can call
		/// \todo restart_advertising(uint16_t timeout); to advertise
		/// \todo for another <timeout> seconds
		/**************************************************************/
		/**************************************************************/
		if (g_enable_ble)
		{
			restart_advertising(15);
		}

		/**************************************************************/
		/**************************************************************/
		/// \todo read sensor or whatever you need to do frequently
		/// \todo write your data into a char array
		/// \todo call LoRa P2P send_lora_packet()
		/**************************************************************/
		/**************************************************************/

		uint8_t data_size = bme680_get();
		lmh_error_status result = send_lora_packet(collected_data, data_size);
		switch (result)
		{
		case LMH_SUCCESS:
			MYLOG("APP", "Packet enqueued");
		packet_counter++;
			break;
		case LMH_BUSY:
			MYLOG("APP", "LoRa transceiver is busy");
			break;
		case LMH_ERROR:
			MYLOG("APP", "Packet error, too big to send with current DR");
			break;
		}

		MYLOG("APP", "LoRa package sent");
	}
}

/**
 * @brief Handle BLE UART data
 * 
 */
void ble_data_handler(void)
{
	if (g_enable_ble)
	{
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
			String uart_rx_buff = g_ble_uart.readStringUntil('\n');

			uart_rx_buff.toUpperCase();

			MYLOG("BLE", "BLE Received %s", uart_rx_buff.c_str());
		}
	}
}

/**
 * @brief Handle received LoRa Data
 * 
 */
void lora_data_handler(void)
{
	if ((g_task_event_type & LORA_JOIN_FIN) == LORA_JOIN_FIN)
	{
		/**************************************************************/
		/**************************************************************/
		/// \todo LoRa Join finished
		/// \todo If Join failed, Join request can be restarted here
		/**************************************************************/
		/**************************************************************/
		g_task_event_type &= N_LORA_JOIN_FIN;
		if (g_join_result)
		{
			MYLOG("APP", "Successfully joined network");
		}
		else
		{
			MYLOG("APP", "Join network failed");
			/// \todo here join could be restarted.
			lmh_join();

			// If BLE is enabled, restart Advertising
			if (g_enable_ble)
			{
				restart_advertising(15);
			}
		}
	}

	if ((g_task_event_type & LORA_TX_FIN) == LORA_TX_FIN)
	{
		/**************************************************************/
		/**************************************************************/
		/// \todo LoRaWAN TX cycle (including RX1 and RX2 window) finished
		/// \todo can be used to enable next sending
		/// \todo if confirmed packet sending, g_rx_fin_result holds the result of the transmission
		/**************************************************************/
		/**************************************************************/
		g_task_event_type &= N_LORA_TX_FIN;

		MYLOG("APP", "LPWAN TX cycle %s", g_rx_fin_result ? "finished ACK" : "failed NAK");
		if (g_ble_uart_is_connected)
		{
			g_ble_uart.printf("LPWAN TX cycle %s", g_rx_fin_result ? "finished ACK" : "failed NAK");
		}

		if (!g_rx_fin_result)
		{
			// Increase fail send counter
			send_fail++;

			if (send_fail == 10)
			{
				// Too many failed sendings, reset node and try to rejoin
				delay(100);
				sd_nvic_SystemReset();
			}
		}
	}

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

		/**************************************************************/
		/**************************************************************/
		/// \todo Just an example, if BLE is enabled and BLE UART
		/// \todo is connected you can send the received data
		/// \todo for debugging
		/**************************************************************/
		/**************************************************************/
		if (g_ble_uart_is_connected && g_enable_ble)
		{
			for (int idx = 0; idx < g_rx_data_len; idx++)
			{
				g_ble_uart.printf("%02X ", g_rx_lora_data[idx]);
			}
			g_ble_uart.println("");
		}
	}
}
