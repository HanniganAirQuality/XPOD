/*******************************************************************************
 * @project Hannigan Lab's Next Gen. Air Quality Pods
 *
 * @file    xpod_node.h
 * @brief   XPOD Sensor Array Firmware
 *
 * @author 	Percy Smith
 * @date 	  October 01, 2024
 * @log     Adds in UTC Functionality
 * @TBD     Working on making LEDs work 
 *          Improve navigatability
 *          Stem-off issues with SD naming
 ******************************************************************************/
#ifndef _XPOD_NODE_H
#define _XPOD_NODE_H

/****************** POD ID & OTHERS ********************/
// const char xpodID[] = "XPODA1";
const int UTC_CORRECTION = +6;

/****************** SENSOR CONFIGS ********************/
#define SERIAL_ENABLED        1
#define SD_ENABLED            1 //SPI
#define RTC_ENABLED           1 //I2C
  #define ADJUST_DATETIME     1
  #define WRITE_UTC_DT        1
#define ADS_ENABLED           1 //I2C
#define BME_ENABLED           1 //I2C
#define PMS_ENABLED           1
#define QUAD_ENABLED          1
#define MQ_ENABLED            0
#define MET_ENABLED           0

/****************** PIN DEFINITIONS ********************/
#define IN_VOLT_PIN           A0
#define SD_CS                 53
//LED Definitions
#define STATUS_RUNNING        12
#define STATUS_ERROR          11
#define STATUS_HALTED         13
#define RED_EXT               61
#define GREEN_EXT             60

/****************** SET ADDR & CONST ********************/
#define CO2_I2C_ADDR          0x31

#endif // _XPOD_NODE_H