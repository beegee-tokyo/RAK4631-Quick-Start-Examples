# LoRaWAN node for environment sensing

This app uses the RAK1906 to measure temperature, humidity, barometric pressure and air quality (as gas resistance) and transmit the data over LoRaWAN.

This example has BLE enabled, so you can setup the LoRaWAN parameters over BLE. 
Debug output is enabled as well. If you want to measure current consumption, you should disable the debug output in platformio.ini by setting `MY_DEBUG` to 0:
```ini
build_flags = 
	-DMY_DEBUG=0
```

Payload decoder for Chirpstack:    
```js
function Decode(fPort, bytes, variables) {
	var decoded = {};
	// byte 0 is the data type marker
	switch (bytes[0])
	{
		// type 1 = environmental data
		case 1:
			decoded.temperature = (bytes[1] << 8 | bytes[2]) / 100;
			decoded.humidity = (bytes[3] << 8 | + bytes[4]) / 100;
			decoded.pressure = (bytes[8] | (bytes[7] << 8) | (bytes[6] << 16) | (bytes[5] << 24)) / 100;
			decoded.gas = bytes[12] | (bytes[11] << 8) | (bytes[10] << 16) | (bytes[9] << 24);
			break;
		default:
			decoded.unknown = "Unknown data format";
			break;
	}
	return decoded;
}
```