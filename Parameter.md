# LoRaWAN / LoRa manual setup

In case the setup of the LoRaWAN parameters should be fixed in the application sources and not setup over BLE or with AT commands, all parameters are accessible in the `main.h` include file.

----

## Content
[Setup for LoRaWAN](#setup-for-lorawan)    
- [LoRaWAN parameters](#lorawan-parameters)    

[Setup for LoRa](#setup-for-lora)    
- [LoRa parameters](#lora-parameters)

----

## Setup for LoRaWAN
```c++  
struct s_lorawan_settings
{
	uint8_t valid_mark_1 = 0xAA;				// Just a marker for the Flash
	uint8_t valid_mark_2 = LORAWAN_DATA_MARKER; // Just a marker for the Flash

	// Flag if node joins automatically after reboot
	bool auto_join = false;
	// Flag for OTAA or ABP
	bool otaa_enabled = true;
	// OTAA Device EUI MSB
	uint8_t node_device_eui[8] = {0x00, 0x0D, 0x75, 0xE6, 0x56, 0x4D, 0xC1, 0xF3};
	// OTAA Application EUI MSB
	uint8_t node_app_eui[8] = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x02, 0x01, 0xE1};
	// OTAA Application Key MSB
	uint8_t node_app_key[16] = {0x2B, 0x84, 0xE0, 0xB0, 0x9B, 0x68, 0xE5, 0xCB, 0x42, 0x17, 0x6F, 0xE7, 0x53, 0xDC, 0xEE, 0x79};
	// ABP Network Session Key MSB
	uint8_t node_nws_key[16] = {0x32, 0x3D, 0x15, 0x5A, 0x00, 0x0D, 0xF3, 0x35, 0x30, 0x7A, 0x16, 0xDA, 0x0C, 0x9D, 0xF5, 0x3F};
	// ABP Application Session key MSB
	uint8_t node_apps_key[16] = {0x3F, 0x6A, 0x66, 0x45, 0x9D, 0x5E, 0xDC, 0xA6, 0x3C, 0xBC, 0x46, 0x19, 0xCD, 0x61, 0xA1, 0x1E};
	// ABP Device Address MSB
	uint32_t node_dev_addr = 0x26021FB4;
	// Send repeat time in milliseconds: 2 * 60 * 1000 => 2 minutes
	uint32_t send_repeat_time = 120000;
	// Flag for ADR on or off
	bool adr_enabled = false;
	// Flag for public or private network
	bool public_network = true;
	// Flag to enable duty cycle
	bool duty_cycle_enabled = false;
	// Number of join retries
	uint8_t join_trials = 5;
	// TX power 0 .. 15
	uint8_t tx_power = 0;
	// Data rate 0 .. 15 (validity depnends on Region)
	uint8_t data_rate = 3;
	// LoRaWAN class 0: A, 2: C, 1: B is not supported
	uint8_t lora_class = 0;
	// Subband channel selection 1 .. 9
	uint8_t subband_channels = 1;
	// Data port to send data
	uint8_t app_port = 2;
	// Flag to enable confirmed messages
	lmh_confirm confirmed_msg_enabled = LMH_UNCONFIRMED_MSG;
	// Command from BLE to reset device
	bool resetRequest = true;
	// LoRa region
	uint8_t lora_region = 0;
};
```    

----

### LoRaWAN Parameters
`auto_join`         
- if true, the node will automatically start to connect to the LoRaWAN network    

`otaa_enabled`    
- if true, OTAA join method is used    

`node_device_eui`     
- (OTAA) the DevEUI, MSB format     

`node_app_eui`
- (OTAA) the AppEUI, MSB format    

`node_app_key`    
- (OTAA) the AppKey, MSB format    

`node_nws_key`     
- (ABP) the network session key, MSB format     

`node_apps_key`
- (ABP) the application session key, MSB format    

`node_dev_addr`    
- (ABP) the device address, 32bit format

`send_repeat_time`    
- automatic sending of packets frequency in milli seconds. If set to 0, automatic sending is disabled

`adr_enabled`
- if true, ADR (automatic datarate adaption) is enabled

`public_network`
- if true, public network is selected 

`duty_cycle_enabled`
- if true, duty cycle is enabled

`join_trials`
- how often OTAA join should be retried before reporting an error

`tx_power`
- TX power 0 .. 15 (validity depends on Region)

`data_rate`
- Data rate 0 .. 15 (validity depends on Region)

`lora_class`
- LoRaWAN class 0: A, 2: C, _1: B is not supported_

`subband_channels`
- Subband channel selection 1 .. 9, depends on Region, e.g. for US915 subband 0 uses LoRaWAN channels 0 to 7, subband 1 uses channels 8 to 15

`app_port`
- Default data port to be used

`confirmed_msg_enabled`
- if true, packets are sent are requesting ACK from LoRaWAN server

`resetRequest`
- do not change, used by BLE application to reset the node

`lora_region`
- defines LoRaWAN region
   - 0 = AS923-1 Asia 923 MHz
   - 1 = AU915 Australia 915 MHz
   - 2 = CN470 China 470 MHz
   - 3 = CN779 China 779 MHz
   - 4 = EU433 Europe 433 MHz
   - 5 = EU868 Europe 868 MHz
   - 6 = IN865 India 865 MHz
   - 7 = KR920 Korea 920 MHz
   - 8 = US915 US 915 MHz
   - 9 = AS923-2 Asia 923 MHz with frequency shift of -1.8MHz (not tested)
   - 10 = AS923-3 Asia 923 MHz with frequency shift of -6.6MHz (e.g. Philippines) (in use)
   - 11 = AS923-4 Asia 923 MHz with frequency shift of -5.9MHz (Israel) (not tested)
   - 12 = RU864 Russia 864 MHz (not tested)

----

## Setup for LoRa
still to be done

----

### LoRa parameters
still to be done

----

_**LoRa® is a registered trademark or service mark of Semtech Corporation or its affiliates.**_    
_**LoRaWAN® is a licensed mark.**_

----
