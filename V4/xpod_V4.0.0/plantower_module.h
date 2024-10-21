/*******************************************************************************
 * @file    plantower_module.h
 * @brief   Adapts the YPOD PM Firmware to XPOD 
 *
 * @cite    PM_Firmware.ino (YPOD) by ?? 
 *
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    October 09, 2024
 * @log     Adapts to XPOD
******************************************************************************/
#ifndef _PLANTOWER_MODULE_H
#define _PLANTOWER_MODULE_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "xpod_node.h"

// Constants defined in the Plantower PMS5003 datasheet
// #define DEFAULT_BAUD_RATE 9600
#define START_BYTE1       0x42
#define START_BYTE2       0x4d

/*! PMS5003 data structure (ALL DATA) as uint16_t */
struct pms5003data 
{
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t reserved;
  uint16_t checksum;
};

/*! PMS5003 class to include functionality from YPOD's PM_Firmware.ino */
class PMS5003 {
  public:
    PMS5003();
    bool readPMSdata(Stream *s); 
    pms5003data returnPMdataset();

  private:
    pms5003data PMS_data;
};

#endif  //PLANTOWER_MODULE_H