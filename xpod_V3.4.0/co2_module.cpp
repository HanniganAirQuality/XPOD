#include "Arduino.h"
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
#include "co2_module.h"
#include <Wire.h>             //P - last tested with "Wire@1.0"

ELT_S300::begin()
{
  // Wire.begin();
}

/**************************************************************************/
 /*!
 *    @brief  Reads incoming I2C communication with the CO2 Sensor
 *    @return float of CO2 reading
 */
/**************************************************************************/
uint16_t ELT_S300::getS300CO2() {
  int i = 1;
  uint16_t reading;
  //float CO2val;
  wire_setup(0x31, 0x52, 7);

  while (Wire.available()) {
    byte val = Wire.read();
    if (i == 2) {
      reading = val;
      reading = reading << 8;
      delay(10);
    }
    if (i == 3) {
      reading = reading | val;
      delay(10);
    }
    i = i + 1;
  }
  return reading;
}

/**************************************************************************/
 /*!
 *    @brief  sets up the I2C comms with ELT S300 CO2 Sensor
 */
/**************************************************************************/
void ELT_S300::wire_setup(int address, byte cmd, int from) {
  Wire.beginTransmission(address);
  Wire.write(cmd);
  Wire.endTransmission();
  Wire.requestFrom(address, from);
}