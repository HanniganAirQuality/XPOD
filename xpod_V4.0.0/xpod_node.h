#include <stdint.h>
/*******************************************************************************
 * @project Hannigan Lab's Next Gen. Air Quality Pods
 *
 * @file    xpod_node.h
 * @brief   XPOD Sensor Array Firmware
 *
 * @author 	Percy Smith
 * @date 	  July 28, 2025
 ******************************************************************************/
#ifndef _XPOD_NODE_H
#define _XPOD_NODE_H

/****************** POD ID & CONST ********************/
const char XPODID[] = "MPODB1";
const uint8_t UTC_CONV = 6; //for MST choose 6, MDT choose 7 

/****************** CONFIG/SETTING ********************/
#define SERIAL_ENABLED        1
#define SD_ENABLED            1 //SPI (CS: D53)
#define RTC_ENABLED           1 //I2C (ADR: 0x68)
  #define ADJUST_DATETIME     0
  #define USE_UTC             0

#define ADS_ENABLED           0 //I2C (ADR: 0x48, 0x49, 0x4A, 0x4B)
  #define PID_ENABLED         0 
  #define MQ_ENABLED          0
#define CO2_ENABLED           0 //I2C (ADR: 0x31)
#define BME_ENABLED           0 //I2C (ADR: 0x76)
#define QUAD_ENABLED          0 //I2C (ADR: 0x6E, 0x69)
#define PMS_ENABLED           1 //UART (TX/RX: Serial1)
  #define INCLUDE_STANDARD    1
  #define INCLUDE_PARTICLES   1

#define THE_DAWG              1 //say hi to mr watchdog - he is needed for CO2 - this is a dev feature.

/****************** SET ADDR & CONST ********************/
#define BME_SENSOR_ADDR       0x76
#define CO2_I2C_ADDR          0x31
#define ALPHA_ONE_ADDR        0x69
#define ALPHA_TWO_ADDR        0x6E

/****************** PIN DEFINITIONS ********************/
//Important Pins
#define SD_CS                 53

//LED Definitions
#define BLUE_LED              11
#define GREEN_LED             12
#define RED_LED               13

#endif // _XPOD_NODE_H