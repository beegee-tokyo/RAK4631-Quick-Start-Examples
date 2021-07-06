## LoRaWAN® handler

Initialization of the LoRaWAN stack is a standard procedure, all setup parameters are taken from a structure that is stored in the non-volatile memory of the RAK4631.
  
If written well, the event callbacks do not need to know anything about the application or what kind of data it is sending or receiving. This code part has a standard send packet function **`send_lora_packet(uint8_t *data, uint8_t size);`** and all it needs to know is a pointer to the data and the data size.   

In addition, there is a second event that reports that a LoRaWAN packet sending cycle is finished. This triggers the event flag **`LORA_TX_FIN`**. When this event occurs and the sending method was **CONFIRMED_MSG**, then the global variable **`g_rx_fin_result`** contains the result (ACK received or no ACK received).    
In most cases the information that a TX cycle is finished is not relevant. But if it is necessary to send larger data packets that do not fit into the allowed payload size, this event can be used to trigger a second or third TX cycle to be able to send the rest of the payload.