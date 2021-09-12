| ![RAKwireless](./assets/RAK.png) | ![RAKstar](./assets/rakstar.jpg) | ![WisBlock](./assets/WisBlock.png) |
| :-: | :-: | :-: |

_**What started here as a frame for an application has now been moved into an Arduino Library that can be used for both Arduino IDE and PlatformIO.**_
_**Check out the [WisBlock-API](https://github.com/beegee-tokyo/WisBlock-API) for the latest updated version**_
_**The WisBlock-API has been used in the following PlatformIO projects:**_
- _**[RAK4631-Kit-4-RAK1906](https://github.com/beegee-tokyo/RAK4631-Kit-4-RAK1906) Environment sensor application for the [WisBlock Kit 4](https://store.rakwireless.com/collections/kits-bundles/products/wisblock-kit-4-air-quality-monitor)**_
- _**[RAK4631-Kit-2-RAK1910-RAK1904-RAK1906](https://github.com/beegee-tokyo/RAK4631-Kit-2-RAK1910-RAK1904-RAK1906) LPWAN GNSS tracker application for the [WisBlock Kit 2](https://store.rakwireless.com/collections/kits-bundles/products/wisblock-kit-2-lora-based-gps-tracker-with-solar-panel)**_
- _**[RAK4631-Kit-2-RAK12500-RAK1906](https://github.com/beegee-tokyo/RAK4631-Kit-2-RAK12500-RAK1906) LPWAN GNSS tracker application using the [RAK12500](https://store.rakwireless.com/products/wisblock-gnss-location-module-rak12500)**_

~~_**WORK IN PROGRESS! ONLY LORAWAN VERSION FOR PLATFORMIO IS READY**_~~

# Container for your next LoRa® / LoRaWAN® application

Everytime I get a new prototype module from RAK or a new application idea pops up in my mind, I start to create a new project folder, then copy all the basic functionality that I usually want from other projects into it.      
Kind of wasted time and easily leads to mistakes. So I thought if I have a ready to use project folder ( == Container) with all the basic functionality ready, that makes it much easier. Just copy the folder, rename it to whatever your application is doing and concentrate on the implementation of your application specific code.  

## Why call it a container?      
Let's image the container as a box that has all the basic functions already in it:     
<center><img src="./assets/empty-container.jpg"></center>    
The only thing I need to do is to write the application specific stuff and put it into the container.    
<center><img src="./assets/container.png"></center>

## Why two different containers for LoRa® and LoRaWAN®?
With a little bit effort, I could write the code in a way that it easily covers both scenarios, but for code readability and easier maintenance I separate it into two different containers.

## What are basic functions?
I do not know about you, but for me the essential functions are:
1. Setup LoRa®/LoRaWAN® parameter with an AT command interface over USB
2. BLE OTA DFU for firmware updates
3. LoRa®/LoRaWAN® parameter setup over BLE
4. BLE UART (usually for wireless debug output)
5. Storing the parameters in a nonvolatile memory storage
6. Initialize LoRa®/LoRaWAN®   
7. Support for all LoRaWAN® regions
8. Timer for frequently performed actions
9. Event triggered loop to reduce the power consumption of the application

Most of above I usually have in my applications and I do not want to rewrite it every time I start a new application.

(1), (2) (3) (4) and (5) are going hand in hand together. I have already code blocks for the AT command interface, initialization of BLE with all services I need, and other code blocks that read or save parameters into the flash memory. 

(6) and (7) is always the same, initialize LoRa and then either start the join process (if LoRaWAN®), or just be ready to listen and send packets (LoRa® P2P)

And (8) and (9) are a standardized loop function that is sleeping until it gets woken up by an event.

# How to use the container

All the application code is in **`app.cpp`** and **`app.h`**.    
In **`app.h`** all global definitions and required include files (e.g. for libraries) are placed.    
**`app.cpp`** is replacing the Arduino loop() function. Only that it does not run as a loop, wasting battery power.    
In **`app.cpp`** you find `setup_app()`. This function is called at the startup of the device. You place here the initializations for your application, like setup of sensor libraries.  And there is a second function `init_app()`. This function is called after BLE and LoRa®/LoRaWAN® are completely intialized.    

Here is an example for `setup_app()` where BLE is disabled
```c++
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
	enable_ble = false;
}
```

Here is an example for `init_app()` where I initialize an interrupt input from a PIR sensor:
```c++
bool init_app(void)
{
	// Add your application specific initialization here
	pinMode(PIR_PIN, INPUT);
	attachInterrupt(PIR_PIN, pir_triggered, RISING);
	return true;
}
```
`pir_triggered` is the interrupt callback, I will explain in more details later.

Then there are three event handlers, `app_event_handler()`, `ble_data_handler()` and `lora_data_handler()`. These three functions are called on different events.

## `app_event_handler`
The [parameters](#parameters) have a setup for a periodic happening event. In the parameters it is defined how often this event, called **STATUS**, is happening. It can be used to either send periodic packets over LoRaWAN or read a sensor or a GPS module. You can as well define your own additional events, that can be e.g. triggered by interrupts from an external device or sensor.    
The ID's for these additional events are defined in `app.h`. There are a few examples listed there:
```c++
/** Examples for application events */
#define PIR_TRIGGER   0b0000000000100000
#define N_PIR_TRIGGER 0b1111111111011111
#define BUTTON        0b0000000001000000
#define N_BUTTON      0b1111111110111111
```
Make sure that the events are all a unique number and use only a single bit. You have the option to define up to 28 application events. 5 events are already occupied by the LoRa®/LoRaWAN® and BLE events.    
Each event flag should have a clear mask as well. You will see the usage below.    

To handle these events, you have to add an event handler inside of `app_event_handler()`. In the example below, I use the example event `PIR_TRIGGER`:    
```c++
	// Check if it is a PIR trigger event
	if ((g_task_event_type & PIR_TRIGGER) == PIR_TRIGGER)
	{
		/**************************************************************/
		/**************************************************************/
		/// \todo IMPORTANT, YOU MUST CLEAR THE EVENT FLAG HERE
		/**************************************************************/
		/**************************************************************/
		g_task_event_type &= N_PIR_TRIGGER;
		MYLOG("APP", "PIR triggered");

		/**************************************************************/
		/**************************************************************/
		/// \todo read sensor or whatever you need to do when
		/// \todo the PIR motion sensor was triggered
		/**************************************************************/
		/**************************************************************/

	}
```

Now to trigger the event _**PIR_TRIGGER**_ we need an interrupt callback, which is called when the PIR sensor signals a motion detection.
```c++
void pir_triggered(void)
{
	// Define the event type
	g_task_event_type |= PIR_TRIGGER;
	// Wake up the handler, it will check g_task_event_type and know that he has to handle an PIR alarm.
	xSemaphoreGiveFromISR(g_task_sem, pdFALSE);

}
```

## `ble_data_handler()`
This handler is called when data was received over the BLE UART service. You can remove it if you set `enable_ble` to false, or use the received data for your own application

## `lora_data_handler()`
This handler is called when data was received over over LoRa® or LoRaWAN®. Here you parse the received data for your own application

_**That is basically all you need to do to add your own application to a working LoRaWAN and BLE container.**_

## How to setup the LoRaWAN®/LoRa® parameters
The easiest way to setup the parameters are by using the USB interface and the AT command interface. After the RAK4631 is flashed, connect a serial terminal application and setup all required parameters using the [AT commands](./AT-Commands.md).

An example for a typical setup for LoRaWAN, region AS923-3, using OTAA, period time for frequent actions set to 60 seconds and automatic join network after reboot:
```c++
AT+APPEUI=70b3d57ed00201e1                    // Set the Application EUI from the LoRaWAN server application
	OK                                        // Response from device
AT+APPKEY=2b84e0b09b68e5cb42176fe753dcee79    // Set Application key from the LoRaWAN server application
	OK                                        // Response from device
AT+DEVEUI=ac1f09fffe03efdc                    // Set Device EUI from the LoRaWAN device setup
	OK                                        // Response from device
AT+NJM=0                                      // Set join mode to OTAA
	OK                                        // Response from device
AT+SENDFREQ=60                                // Set periodic sending to 60 seconds
	OK                                        // Response from device
AT+BAND=10                                    // Set LoRaWAN region to AS923-3
	OK                                        // Response from device
AT+JOIN=0,1,8,10                              // Enable automatic join after reboot, 10 join retries
	OK                                        // Response from device
ATZ                                           // Reboot the device
```
After these sequence, the node will reboot and automatically start the OTAA join sequence.

_**After each AT command you have to wait for the response from the device. Don't send the next AT command before the response was received**_

Another option is to connect to the RAK4631 with the Android app [My nRF52 Toolbox](https://play.google.com/store/apps/details?id=tk.giesecke.my_nrf52_tb) and setup all parameters from the Android app over BLE. This requires that you enable the BLE function.

The third option is to setup the LoRaWAN®/LoRa® parameters directly in the code. Please read [Parameters section](#parameters) for details.    

----

## Special functions and example code
**1) Application debug output.** I am using my own debug log output with the `MY_LOG(...)` function. Log output over USB is enabled and disabled either from `main.h` or in the `platformio.ini` file with the `build_flags` option:
```ini
build_flags = 
	-DMY_DEBUG=0 ; 0 to disable debug output, 1 to enable debug output
```
**2) Disable BLE functions.** Set `enable_ble = false;` in the `setup_app()` function. If BLE is disabled, you can delete the `ble_data_handler()` function from `app.cpp`.     
**3) SW versioning.** The SW version can be set in  4 level numbering w.x.y.z in the `platformio.ini` file with the `build_flags` option: 
```ini
build_flags = 
	-DSW_VERSION_1=1   ; Version major digit
	-DSW_VERSION_2=0   ; Version second digit
	-DSW_VERSION_3=0   ; Version third digit
	-DSW_VERSION_4=0   ; Version minor digit
```
**4) Enable/disable the BLE indicator LED.** The blue BLE indicator LED can be disabled in `platformio.ini` file with the `build_flags` option: 
```ini
build_flags = 
	-DNO_BLE_LED=1  ; if set, BLE LED is disabled. Remove define or comment it out to enable the BLE LED.
```

**5) Example application.** There are two examples how to use the container.    
_Example 1_ is based on timer events and reads a RAK1906 environment sensor frequently and sends a data packet ==> [RAK4631-LP-Environment](./PlatformIO/RAK4631-LP-Environment).     
_Example 2_ is interrupt driven and sends a packet when an acceleration in x, y or z axis was detected by the RAK1904 module ==> [RAK4631-LP-Acceleration](./PlatformIO/RAK4631-LP-Acceleration)

----

_Read on below if you want to know more about the container functions itself._

----
----

# Let's look a little bit deeper into these "standard" functionality.

## Parameters
Here I have two different scenarios, depending whether I am working on a LoRa® P2P solution or using LoRaWAN®. The principle for both is the same, predefined structures hold all the required information and standardized functions write them to or read them from the nonvolatile memory. The difference is only in the content of the data structures. 

## Parameters in nonvolatile memory
All parameters can be preset in the **`main.h`** include file. 
I explain this in the [Parameter storage](./Parameter.md) document.

## AT command interface
The manual for all AT commands is in the [AT Command Manual](./AT-Commands.md)
After setting the parameters over with the AT commands, they are stored in the nonvolitale memory.

Using the AT commands avoids to have to create different binary files for different devices. You can keep the default settings and just setup each indivual device over the USB port.

## BLE services

Details about the BLE services are in the [BLE-Services](./BLE_Services.md) document.  
  
## LoRa® / LoRaWAN®

LoRa P2P and LoRaWAN setup and handlers are quite different. The details can be found in [LoRaWAN Handler](./Lorawan_Handler.md) and [LoRa Handler](./Lora_Handler.md).

As long as no special protocol handling is required, it should not be necessary to change anything in these code blocks.

## Timed events and event triggered loop()  

When we write quick and dirty in Arduino, we just let the loop() run and run and run. Just checking if a sensor has some data or if a packet has arrived over LoRa.     
But if we want to write an application that is concerned about power consumption, this is the worst thing to do. As a personal standard, in my applications I have the loop waiting for an event to happen and if nothing happens, just halt its execution. This is done with the help of FreeRTOS and semaphores. Time triggered events or communication events use a semaphore to wake up the loop(), tell it with a flag what events have happened and let the loop() handle them. After that, the loop goes back to sleep.  
Yes, I use the word _**sleep**_, because in nRF Arduino, if there are no events (like BLE) and all tasks are halted waiting for a semaphore, the MCU will go into sleep mode. That is the best way to lower the battery consumption.   
More information about events and where to place application specific code is in the [Events & Loop](./Events_Loop.md) document.

----

_**LoRa® is a registered trademark or service mark of Semtech Corporation or its affiliates.**_    
_**LoRaWAN® is a licensed mark.**_

----
