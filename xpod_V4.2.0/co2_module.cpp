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

/**************************************************************************/
 /*!
 *    @brief  begins Wire object
 */
/**************************************************************************/
ELT_S300::begin()
{
  Wire.begin(); //just in case lol
}

/**************************************************************************/
 /*!
 *    @brief  Reads incoming I2C communication with the CO2 Sensor
 *    @return uint16_t of CO2 reading
 */
/**************************************************************************/
uint16_t ELT_S300::getS300CO2() {
  int i = 1;
  uint16_t reading;
  wire_setup(CO2_I2C_ADDR, CO2_ACKNOWLEDGE_ADDR, 7);

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