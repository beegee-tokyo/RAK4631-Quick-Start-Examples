/**
 * @file app.h
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief For application specific includes and definitions
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
#include <SparkFunLIS3DH.h>

#include <Wire.h>
/** Include the SX126x-API */
#include <WisBlock-API.h>
/** Application function definitions */
void setup_app(void);
bool init_app(void);
void app_event_handler(void);
void ble_data_handler(void) __attribute__((weak));
void lora_data_handler(void);

/** Examples for application events */
#define ACC_TRIGGER   0b1000000000000000
#define N_ACC_TRIGGER 0b0111111111111111
#define SEND_STAT     0b0100000000000000
#define N_SEND_STAT   0b1011111111111111

/** Sensor specific functions */
#define INT1_PIN WB_IO1
bool init_acc(void);
void get_acc_int(void);
extern bool has_x_move;
extern bool has_y_move;
extern bool has_z_move;

#endif