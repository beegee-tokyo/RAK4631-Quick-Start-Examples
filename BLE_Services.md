# BLE services

This is simpler than it sounds. My "standard" BLE initialization is always with 3 default service characteristics:
1. OTA DFU service
2. LoRa® parameter settings service
3. BLE UART service  

These 3 services are always the same and are in **`ble.cpp`**. There is nothing in this file that would need to be changed for different applications. The BLE services work all with callback functions, process the data and only inform the loop() task with a flag if additional work has to be done.    

## OTA DFU service
This service is running complete in the background and the user application does not need to take care of it.

## LoRa® parameter settings service
This service creates a BLE characteristic that is used to send/receive the **`s_lorawan_settings`** parameter structure between the device and an Android application. The parameter structure is explained in the [Parameter readme](./Parameter.md).    
Details and source code for the Android application can be found in the [MY-nRF52-Toolbox](https://github.com/beegee-tokyo/My-nRF52-Toolbox) repository.

## BLE UART service
If data was received, the flag **`BLE_DATA`** is set. This flag should be reset in the **`ble_data_handler`** function and the received data can be handled there as well.

```c++
/**
 * @brief Handle BLE UART data
 * 
 */
void ble_data_handler(void)
{
	if (enable_ble)
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
			String uart_rx_buff = ble_uart.readStringUntil('\n');

			uart_rx_buff.toUpperCase();

			MYLOG("BLE", "BLE Received %s", uart_rx_buff.c_str());
		}
	}
}
```