# Events loop

Different to usual Arduino programming style, there is no loop() function for the application. (Well, there is one, but it is hidden). Instead the application is complete event driven. In the application code there should never be a call for **`delay()`** or a loop that waits for something to happen. Instead timers and interrupt handlers are used to react on events.

There are two main elements that are used to notify about an event.
1) Event flag    
    The event flag is a 32bit variable. Each bit is representing a single event. By using single bits, several events can happen at nearly the same time and can be handled sequentially without missing an event
2) Semaphores
    The loop task is waiting in sleep mode for the release of the semaphore. If an event (timer or interrupt) releases the semaphore, the loop wakes up and starts processing the event. Most events, like LoRa events (packet sent, packet received, network joined) and BLE events (LoRaWAN parameter settings) are handled in the background. User defined events are handled in the **`app_event_handler()`**.

## Event flag
As said, the event flag can hold up to 32 different events. For the internal events, like LoRaWAN, BLE the lower bits of the event handler are reserved. The lower 6 bits of the event flag are occupied by these fixed events:

| Internal Event | Event mask |
| -- | :-: |
| STATUS event is a timer triggered event) | 0b0000000000000001 |
| BLE_CONFIG is triggered when new LoRaWAN parameters arrived over BLE | 0b0000000000000010 |
| BLE_DATA is triggered when data over BLE UART has arrived. It is an internal event, but handling is forwarded to **`ble_data_handler()`** | 0b0000000000000100 |
| LORA_DATA is triggered after a LoRaWAN packet was received. It is an internal event, but handling is forwarded to **`lora_data_handler()`** | 0b0000000000001000
| LORA_TX_FIN is triggered after a LoRaWAN TX cycle is finished. It is an internal event, but handling is forwarded to **`lora_data_handler()`** | 0b0000000000010000 |
| AT_CMD is triggered after an AT command was received over the USB interface. The event is completely handled in the background. | 0b0000000000100000 |

Additional events are defined in **`app.h`**. To avoid conflicts with the internal events, the user app events should start from the highest bit of the event flag. Here are a few examples for user app events:

| User App Event | Event mask |
| -- | :-: |
| ACC_TRIGGER is set when the accelerometer sets its interrupt line | 0b1000000000000000 |
| PIR_TRIGGER is set if a motion sensor reports a motion detection | 0b0100000000000000 |
| BUTTON is set when the user pushes a button | 0b0100000000000000 |

## Semaphore
The semaphore is used to wake up the **`loop()`** task which is stopped until the semaphore is given by an event. Once the event(s) are handled, the task goes back to sleep.    

## Examplxes
To make it easier to show the usage, I added two simple examples, one using a timer and the other one using hardware interrupts to perform actions on events.    

### Example 1 - Interrupts
_**RAK4631-LP-Acceleration**_ is using interrupts. In **`init_app()`** the setup function for the accelerometer (`init_acc()`) is called. In this function, beside of initializing the accelerometer chip, an interrupt handler is assigned to the GPIO that is connected to the hardware interrupt pin of the accelerometer. The chip is initialized to set an interrupt when a certain level of movement (in m G) has occured.

```c++
	// Set the interrupt callback function
	attachInterrupt(INT1_PIN, acc_int_handler, RISING);
```

The interrupt handler is setting the event flag (ACC_TRIGGER) and uses the task semaphore to wake up the MCU and handle the event.

```c++
void acc_int_handler(void)
{
	// Set the event flag
	g_task_event_type |= ACC_TRIGGER;
	// Wake up the task to handle it
	xSemaphoreGiveFromISR(g_task_sem, &xHigherPriorityTaskWoken);
}
```

The event flag is handled in the **`app_event_handler()`** function that is called after the MCU woke up. Here the event flag is cleared as well. This event triggers as well sending of the device status over LoRaWAN.

```c++
// Accelerometer triggered an interrupt
if ((g_task_event_type & ACC_TRIGGER) == ACC_TRIGGER)
{
	g_task_event_type &= N_ACC_TRIGGER;
	MYLOG("APP", "ACC triggered wakeup");

	// Get ACC status
	get_acc_int();
...
	lmh_error_status result = send_lora_packet(collected_data, data_size);
...
}
```

### Example 2 - Timer triggered
_**RAK4631-LP-Environment**_ is using the predefined timer to send environmental data frequently over LoRaWAN. The time between two timer events can be setup over the [BLE LoRaWAN settings](./BLE_Services.md) function or with an [AT command](./AT-Command.md/#at+sendfreq) (**`AT+SENDFREQ`**).

Every time the **`loop()`** task is woken up by the timer event, **`app_event_handler()`** is reading the sensor data from a Bosch BME680 sensor and the sensor values are sent over LoRaWAN.

```c++
void app_event_handler(void)
{
	// Timer triggered event
	if ((g_task_event_type & STATUS) == STATUS)
	{
		g_task_event_type &= N_STATUS;
		MYLOG("APP", "Timer wakeup");
...
	lmh_error_status result = send_lora_packet(collected_data, data_size);
...
}
```
