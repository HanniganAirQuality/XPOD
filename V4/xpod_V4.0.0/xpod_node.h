/*******************************************************************************
 * @project Hannigan Lab's Next Gen. Air Quality Pods
 *
 * @file    xpod_node.h
 * @brief   XPOD Sensor Array Firmware
 *
 * @author 	Percy Smith
 * @date 	  October 09, 2024
 ******************************************************************************/
#ifndef _XPOD_NODE_H
#define _XPOD_NODE_H

/****************** POD ID & OTHERS ********************/
const char xpodID[] = "XPODA2";

/****************** CONFIG/SETTING ********************/
#define SERIAL_ENABLED        1
#define SD_ENABLED            1 //SPI
#define RTC_ENABLED           1 //I2C
  #define ADJUST_DATETIME     0
  #define USE_UTC             1

#define ADS_ENABLED           0
#define CO2_ENABLED           1
#define BME_ENABLED           0
#define QUAD_ENABLED          0
#define MET_ENABLED           0 //NOT WRITTEN !!!! KEEP 0 - trying to fix mr interruption
#define MQ_ENABLED            0
#define PMS_ENABLED           0

#define EXTERNAL_LED          1
#define IC_POTS               1
#define THE_DAWG              1 //say hi to mr watchdog. he is needed for CO2 - this is more a dev feature.

/****************** PIN DEFINITIONS ********************/
//Important Pins
#define IN_VOLT_PIN           A0
#define SD_CS                 53
//LED Definitions
#define BLUE_LED              11
#define GREEN_LED             12
#define RED_LED               13
#define EXT_RED               61
#define EXT_GREEN             60


#endif // _XPOD_NODE_H