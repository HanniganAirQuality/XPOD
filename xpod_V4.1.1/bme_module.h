/*******************************************************************************
 * @file    bme_module.h
 * @brief   Module to organize functions for BME680 
 *
 * @cite    Adapted bme_module.h, XPOD_V3.2.3 by Ajay Kendagal
 * 
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    July 29, 2025
 ******************************************************************************/
#ifndef _BME_MODULE_H
#define _BME_MODULE_H

#include <Arduino.h>
#include <stdint.h>
#include <Adafruit_BME680.h>

#include "xpod_node.h"

/****************** STRUCTS, OBJECTS ********************/
/*! BME680 data structure (ALL DATA) as respective datatypes */
struct BME_Data {
  float T;
  uint32_t P;
  float RH;
  uint32_t GR;
};

/****************** CLASSES ********************/
class BME_Module {
  public:
    BME_Module();
    bool begin();

    BME_Data return_updated();

  private:
    Adafruit_BME680 bme_sensor;
    bool status;
};

#endif //_BME_MODULE_H