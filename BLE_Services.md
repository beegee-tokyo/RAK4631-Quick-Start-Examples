# BLE services

This is simpler than it sounds. My "standard" BLE initialization is always with 3 default service characteristics:
1. OTA DFU service
2. LoRa® parameter settings service
3. BLE UART service  

These 3 services are always the same and are in **`ble.cpp`**. There is nothing in this file that would need to be changed for different applications. The BLE services work all with callback functions, process the data and only inform the loop() task with a flag if additional work has to be done.    

