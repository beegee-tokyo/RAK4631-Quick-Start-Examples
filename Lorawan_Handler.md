## LoRaWAN® handler

Initialization of the LoRaWAN stack is a standard procedure, all setup parameters are taken from a structure that is stored in the non-volatile memory of the RAK4631.
  
If written well, the event callbacks do not need to know anything about the application or what kind of data it is sending or receiving. This code part has a standard send packet function **`send_lora_packet(uint8_t *data, uint8_t size);`** and all it needs to know is a pointer to the data and the data size. And if a data packet was received, it copies the packet into a safe location and inform the loop() task about the event. The data handling is done in the loop() task.   
