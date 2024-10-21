/*******************************************************************************
 * @file    co2_module.cpp
 * @brief   Splits CO2 firmware from ino 
 *
 * @cite    YPOD Original .ino (by ???)
 *
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    October 7, 2024
******************************************************************************/
#ifndef _CO2_MODULE_H
#define _CO2_MODULE_H

#include <Arduino.h>
#include "xpod_node.h"


/****************** SET ADDR & CONST ********************/
#define CO2_I2C_ADDR          0x31
#define CO2_WRITE_ADDR        0x62
#define CO2_ACKNOWLEDGE_ADDR  0x52
#define CO2_READ_ADDR         0x63
#define MAX_I2C_SPD           400000
#define REC_I2C_SPD           100000


/*! ELT_S300 class to include functionality from YPOD's .ino */
class ELT_S300 {
  public:
    begin();
    float getS300CO2();

  private:
    void wire_setup(int address, byte cmd, int from);
};

#endif  //_CO2_MODULE_H