/*******************************************************************************
 * @file    ads_module.h
 * @brief   Splits ADS1115 code from .ino & updates from ADS1015.h --> ADS1115.h
 *
 * @cite    Adapted LPOD_Particle-V7 ads_module.h, XPOD_V3.3.0 by Ajay Kendagal
 * 
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    July 28, 2025
 * 
 * @log     Changed CO to read int16_t (not needing decimals??? why he do dat??)
 *          Decided to adapt ads_module.cpp from 
 ******************************************************************************/
#ifndef _ADS_MODULE_H
#define _ADS_MODULE_H

#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <stdint.h>

#include "xpod_node.h"


/****************** STRUCTS, OBJECTS ********************/
/*! Index: FIG1, FIG2, FIG3, FIG4, FIG3_HEATER, FIG4_HEATER, MISC2611, AS_WORKER, AS_AUXILIARY, COUNT */
enum ads_sensor_id_e
{
    FIG1 = 0,
    FIG2,
    FIG3,
    FIG3_HEATER,
    FIG4,
    FIG4_HEATER,
    #if MQ_ENABLED
      MQ,
    #endif //MQ_ENABLED
    #if PID_ENABLED
      PID,
    #endif //PID_ENABLED
    MISC2611,
    AS_WORKER,
    AS_AUXILIARY,
    ADS_SENSOR_COUNT
}; //enum ads_sensor_id_e

/*! (per each sensor) addr, channel, status, module (ADS1115) */
struct ads_module_t
{
    uint8_t addr;
    int8_t channel;
    bool status;
    Adafruit_ADS1115 module;
}; //struct ads_module_t

/*! ADS data structure (ALL DATA) as uint16_t */
struct ADS_Data
{
  uint16_t Fig1;
  uint16_t Fig2;
  uint16_t Fig3;
  uint16_t Fig3_heater;
  uint16_t Fig4;
  uint16_t Fig4_heater;
  #if MQ_ENABLED
    uint16_t Mq;
  #endif //MQ_ENABLED
  #if PID_ENABLED
    uint16_t Pid;
  #endif //PID_ENABLED
  uint16_t Misc2611;
  int16_t Auxiliary;
  int16_t Worker;
};  //struct ads_heaters

/****************** CLASSES ********************/
/*! ADS1115 to include 4 Figaros, MiSC-2611, and an Alphasense B4 Sensor */
class ADS_Module {
  public:
    ADS_Module();
    bool begin();
    
    uint16_t read_raw(ads_sensor_id_e ads_sensor_id);    //UNSIGNED (+ only)
    int16_t read_as_auxiliary();                         //allows return of - value
    int16_t read_as_worker();                            //allows return of - value

    ADS_Data return_updated();

  private:
    ads_module_t ads_module[ADS_SENSOR_COUNT];
};

#endif //_ADS_MODULE_H