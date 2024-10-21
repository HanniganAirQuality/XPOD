/*******************************************************************************
 * @file    ads_module.cpp
 * @brief   
 *
 * @author   Ajay Kandagal, ajka9053@colorado.edu
 * @date    Feb 18 2023
 ******************************************************************************/
#include "ads_module.h"

/**************************************************************************/
 /*!
 *    @brief  ADS_Module object; Assigns addresses & channels to ADS1115 modules
 */
/**************************************************************************/
ADS_Module::ADS_Module()
{
  ads_module[ADS_SENSOR_FIG2600].addr = 0x48;
  ads_module[ADS_SENSOR_FIG2600].channel = 3;

  ads_module[ADS_SENSOR_FIG2602].addr = 0x49;
  ads_module[ADS_SENSOR_FIG2602].channel = 2;

#if FIGARO3_ENABLED
  ads_module[ADS_SENSOR_FIG3].addr = 0x48;
  ads_module[ADS_SENSOR_FIG3].channel = 0;

  ads_module[ADS_HEATER_FIG3].addr = 0x48;
  ads_module[ADS_HEATER_FIG3].channel = 1;
#endif

#if FIGARO4_ENABLED
  ads_module[ADS_SENSOR_FIG4].addr = 0x49;
  ads_module[ADS_SENSOR_FIG4].channel = 0;

  ads_module[ADS_HEATER_FIG4].addr = 0x49;
  ads_module[ADS_HEATER_FIG4].channel = 1;
#endif

  ads_module[ADS_SENSOR_PID].addr = 0x48;
  ads_module[ADS_SENSOR_PID].channel = 2;

  ads_module[ADS_SENSOR_E2V].addr = 0x4B;
  ads_module[ADS_SENSOR_E2V].channel = 0;

  ads_module[ADS_SENSOR_CO].addr = 0x4A;
  ads_module[ADS_SENSOR_CO].channel = -1; //Not a value which is actually used, only a placeholder

  for (int i = 0; i < ADS_SENSOR_COUNT; i++)
    ads_module[i].status = false;
} //ADS_Module()

/**************************************************************************/
 /*!
 *   @brief  Start ADS_Module by init all ads sensors on ADS1115
 *   @return True if find ADS1115 channels (4 & 4), 
 *           False if status of channels is false
 */
/**************************************************************************/
bool ADS_Module::begin()
{
  for (int i = 0; i < ADS_SENSOR_COUNT; i++)
  {
    if (ads_module[i].module.begin(ads_module[i].addr))
      ads_module[i].status = true;
  }

  for (int i = 0; i < ADS_SENSOR_COUNT; i++)
  {
    if (ads_module[i].status == false)
      return false;
  }

  return true;
} //bool ADS_Module::begin()

/**************************************************************************/
 /*!
 *    @brief  Reads raw CO Aux sensor readings; no signal processing!!
 *    @return Raw CO Aux readings with reference to V_reference
 */
/**************************************************************************/
float ADS_Module::read_co_aux()
{
  ads_module_t *sensor = &ads_module[ADS_SENSOR_CO];

  float val;
  const float multiplier = 0.1875F;  // ADS1115  @ +/- 6.144V gain (16-bit results)

  if (!sensor->status)
    return -999;

  return (sensor->module.readADC_Differential_0_1());
} //float ADS_Module::read_co_worker()

/**************************************************************************/
 /*!
 *    @brief  Reads raw CO Main sensor readings; no signal processing!!
 *    @return Raw CO Main readings
 */
/**************************************************************************/
float ADS_Module::read_co_worker()
{
  ads_module_t *sensor = &ads_module[ADS_SENSOR_CO];

  float val;
  const float multiplier = 0.1875F;  // ADS1115  @ +/- 6.144V gain (16-bit results)

  if (!sensor->status)
    return -999;

  return (sensor->module.readADC_Differential_2_3());
} //float ADS_Module::read_co_worker()

/**************************************************************************/
 /*!
 *    @brief  Reads raw sensor readings; no signal processing!!
 *        @param  ads_sensor_id index of the sensor to be read (in id_e form)
 *    @return Raw ADS1115 reading for relevant channel (or -999 for error)
 */
/**************************************************************************/
uint16_t ADS_Module::read_raw(ads_sensor_id_e ads_sensor_id)
{
  ads_module_t *sensor = &ads_module[ads_sensor_id];

  if (!sensor->status)
    return -999;

  return sensor->module.readADC_SingleEnded(sensor->channel);
} //uint16_t ADS_Module::read_raw(ads_sensor_id_e ads_sensor_id)

/**************************************************************************/
 /*!
 *    @brief  String for SD from ADS
 *    @return String composed of all ADS1115 raw readings 
 */
/**************************************************************************/
String ADS_Module::read4sd_raw()
{
  String out_str = "";

  out_str += String(read_raw(ADS_SENSOR_FIG2600)) + ",";
  out_str += String(read_raw(ADS_SENSOR_FIG2602)) + ",";

  #if FIGARO3_ENABLED
    out_str += String(read_raw(ADS_SENSOR_FIG3)) + ",";
    out_str += String(read_raw(ADS_HEATER_FIG3)) + ",";
  #endif

  #if FIGARO4_ENABLED
    out_str += String(read_raw(ADS_SENSOR_FIG4)) + ",";
    out_str += String(read_raw(ADS_HEATER_FIG4)) + ",";
  #endif

  out_str += String(read_raw(ADS_SENSOR_PID)) + ",";
  out_str += String(read_raw(ADS_SENSOR_E2V)) + ",";

  out_str += String(read_co_aux()) + "," + String(read_co_worker());

  return out_str;
}

/**************************************************************************/
 /*!
 *    @brief  String for Serial Monitor from ADS
 *    @return String composed of all ADS1115 raw readings with labels
 */
/**************************************************************************/
String ADS_Module::read4print_raw()
{
  String out_str = "";
  
  out_str += "FIG1:" + String(read_raw(ADS_SENSOR_FIG2600)) + ",";
  out_str += "FIG2:" + String(read_raw(ADS_SENSOR_FIG2602)) + ",";

  #if FIGARO3_ENABLED
    out_str += "FIG3:" + String(read_raw(ADS_SENSOR_FIG3)) + ",";
    out_str += "FIG3_H:" + String(read_raw(ADS_HEATER_FIG3)) + ",";
  #endif
  
  #if FIGARO4_ENABLED
    out_str += "FIG4:" + String(read_raw(ADS_SENSOR_FIG4)) + ",";
    out_str += "FIG4_H:" + String(read_raw(ADS_HEATER_FIG4)) + ",";
  #endif

  out_str += "PID:" + String(read_raw(ADS_SENSOR_PID)) + ",";
  out_str += "E2V:" + String(read_raw(ADS_SENSOR_E2V)) + ",";

  out_str += "CO_AUX:" + String(read_co_aux()) + ",CO_MAIN:" + String(read_co_worker());

  return out_str;
}
