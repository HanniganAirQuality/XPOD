/*******************************************************************************
 * @file    co2_module.cpp
 * @brief   Splits CO2 firmware from ino 
 *
 * @cite    YPOD Original .ino (by ???)
 *
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    July 25, 2025 
 * @log     for some reason it wrote to "long" data type and then "float" (weird) -- changed to uint16_t
******************************************************************************/
#ifndef _CO2_MODULE_H
#define _CO2_MODULE_H

#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>             //P - last tested with "Wire@1.0"

#include "xpod_node.h"

/****************** SET ADDR & CONST ********************/
// Available Commands
#define CO2_WRITE_ADDR        0x62
#define CO2_ACKNOWLEDGE_ADDR  0x52
#define CO2_READ_ADDR         0x63
// Limitations 
#define MAX_I2C_SPD           400000
#define REC_I2C_SPD           100000


/****************** CLASSES ********************/
/*! ELT_S300 class to include functionality from YPOD's .ino */
class ELT_S300 {
  public:
    begin();
    uint16_t getS300CO2();

  private:
    void wire_setup(int address, byte cmd, int from);
};

#endif  //_CO2_MODULE_H