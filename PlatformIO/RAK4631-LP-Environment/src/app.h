/**
 * @file app.h
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief For application specific includes and definitions
 *        Will be included from main.h
 * @version 0.1
 * @date 2021-04-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef APP_H
#define APP_H

#include <Arduino.h>
/** Add you required includes after Arduino.h */
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

/** Application function definitions */
void setup_app(void);
bool init_app(void);
void app_event_handler(void);
void ble_data_handler(void) __attribute__((weak));
void lora_data_handler(void);
void lora_tx_finished(bool success);
void lora_rx_failed(void);

/** Examples for application events */
#define AT_CMD 0b0000000000010000
#define N_AT_CMD 0b1111111111101111
#define PIR_TRIGGER 0b0000000000100000
#define N_PIR_TRIGGER 0b1111111111011111
#define BUTTON 0b0000000001000000
#define N_BUTTON 0b1111111110111111

/** Sensor specific functions */
bool init_bme680(void);
uint8_t bme680_get();

#endif