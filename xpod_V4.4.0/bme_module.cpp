/*******************************************************************************
 * @file    bme_module.cpp
 * @brief   Module to organize functions for BME680 
 *
 * @cite    Adapted bme_module.cpp, XPOD_V3.2.3 by Ajay Kendagal
 * 
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    July 29, 2025
 ******************************************************************************/
#include "bme_module.h"

/**************************************************************************/
 /*!
 *    @brief  sets status to false by default which allows begin to work properly
 */
/**************************************************************************/
BME_Module::BME_Module()
{
  status = false;
}

/**************************************************************************/
 /*!
 *    @brief  "begins" the BME680 sensor 
 *    @return true/false - did sensor begin correctly?
 */
/**************************************************************************/
bool BME_Module::begin()
{
  if (bme_sensor.begin(BME_SENSOR_ADDR))  {
    status = true;
    
    // Set up oversampling and filter initialization
    bme_sensor.setTemperatureOversampling(BME680_OS_8X);
    bme_sensor.setHumidityOversampling(BME680_OS_2X);
    bme_sensor.setPressureOversampling(BME680_OS_4X);
    bme_sensor.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme_sensor.setGasHeater(320, 150);
  } //if (bme_sensor.begin(BME_SENSOR_ADDR))

  return status;
}

/**************************************************************************/
 /*!
 *    @brief  Updates T, P, RH, GR readings 
 *    @return BME_Data updated readings of T, P, RH, GR
 */
/**************************************************************************/
BME_Data BME_Module::return_updated() 
{
  BME_Data data_buffer; 
  if (!bme_sensor.performReading())  {
    data_buffer.T = -99;
    data_buffer.P = 0;
    data_buffer.RH = -99;
    data_buffer.GR = 0;
  }  else  {
    data_buffer.T = bme_sensor.temperature;
    data_buffer.P = bme_sensor.pressure;
    data_buffer.RH = bme_sensor.humidity;
    data_buffer.GR = bme_sensor.gas_resistance;
  } //if (!bme_sensor.performReading()) 

  return data_buffer;
}; //BME_Data BME_Module::return_updated()
