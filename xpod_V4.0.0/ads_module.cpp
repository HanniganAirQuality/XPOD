/*******************************************************************************
 * @file    ads_module.cpp
 * @brief   Splits ADS1115 code from .ino & updates from ADS1015.h --> ADS1115.h
 *
 * @cite    Adapted LPOD_Particle-V7 ads_module.cpp, 
 * 
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    July 28, 2025
 * 
 * @log     Changed CO to read int16_t (not needing decimals??? why he do dat??)
 *          Decided to adapt ads_module.cpp from 
 ******************************************************************************/
#include "ads_module.h"

/**************************************************************************/
 /*!
 *    @brief  ADS_Module object; Assigns addresses & channels to ADS1115 modules
 *            Addresses include: GND 0x48, 5V 0x49, SDA 0x4A, SCL 0x4B
 */
/**************************************************************************/
ADS_Module::ADS_Module()
{
  ads_module[FIG1].addr = 0x48;
  ads_module[FIG1].channel = 3;

  ads_module[FIG2].addr = 0x49;
  ads_module[FIG2].channel = 2;

  ads_module[FIG3].addr = 0x48;
  ads_module[FIG3].channel = 0;

  ads_module[FIG3_HEATER].addr = 0x48;
  ads_module[FIG3_HEATER].channel = 1;

  ads_module[FIG4].addr = 0x49;
  ads_module[FIG4].channel = 0;

  ads_module[FIG4_HEATER].addr = 0x49;
  ads_module[FIG4_HEATER].channel = 1;

  #if MQ_ENABLED
    ads_module[MQ].addr = 0x48;
    ads_module[MQ].channel = 1;
  #endif //MQ_ENABLED

  #if PID_ENABLED
    ads_module[PID].addr = 0x48;
    ads_module[PID].channel = 2;
  #endif //PID_ENABLED

  ads_module[MISC2611].addr = 0x4B;
  ads_module[MISC2611].channel = 0;

  ads_module[AS_WORKER].addr = 0x4B;
  ads_module[AS_WORKER].channel = 1;
  
  ads_module[AS_AUXILIARY].addr = 0x4A;
  ads_module[AS_AUXILIARY].channel = 2;

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
 *    @brief  Reads raw sensor readings; no signal processing!!
 *        @param  ads_sensor_id index of the sensor to be read (in id_e form)
 *    @return Raw ADS1115 reading for relevant channel (or 65535 for error)
 */
/**************************************************************************/
uint16_t ADS_Module::read_raw(ads_sensor_id_e ads_sensor_id)
{
  ads_module_t *sensor = &ads_module[ads_sensor_id];

  if (!sensor->status)
    return 65535;

  return sensor->module.readADC_SingleEnded(sensor->channel);
} //uint16_t ADS_Module::read_raw(ads_sensor_id_e ads_sensor_id)

/**************************************************************************/
 /*!
 *    @brief  Reads raw AS Aux sensor readings; no signal processing!!
 *    @return Reads differential between ADC0 & ADC1 which are Auxiliary AS
 */
/**************************************************************************/
int16_t ADS_Module::read_as_auxiliary()
{
  ads_module_t *sensor = &ads_module[AS_AUXILIARY];

  if (!sensor->status)
    return -999;

  return (sensor->module.readADC_Differential_0_1());
} //int16_t ADS_Module::read_as_auxiliary()


/**************************************************************************/
 /*!
 *    @brief  Reads raw alphasense Worker sensor readings; no signal processing!!
 *    @return Reads differential between ADC2 & ADC3 which are Worker AS
 */
/**************************************************************************/
int16_t ADS_Module::read_as_worker()
{
  ads_module_t *sensor = &ads_module[AS_WORKER];

  if (!sensor->status)
    return -999;

  return (sensor->module.readADC_Differential_2_3());
} //int16_t ADS_Module::read_as_worker()


/**************************************************************************/
 /*!
 *    @brief  Updates values and returns structured dataset
 *    @return ADS_Data structured dataset (w/o heaters)
 */
/**************************************************************************/
ADS_Data ADS_Module::return_updated()
{
  ADS_Data dataset;
  dataset.Fig1 = read_raw(FIG1);
  delay(100);
  dataset.Fig2 = read_raw(FIG2);
  delay(100);
  dataset.Fig3 = read_raw(FIG3);
  delay(100);
  dataset.Fig3_heater = read_raw(FIG3_HEATER);
  delay(100);
  dataset.Fig4 = read_raw(FIG4);
  delay(100);
  dataset.Fig4_heater = read_raw(FIG4_HEATER);
  delay(100);
  #if MQ_ENABLED
    dataset.Mq = read_raw(MQ);
    delay(100);
  #endif //MQ_ENABLED
  #if PID_ENABLED
    dataset.Pid = read_raw(PID);
    delay(100);
  #endif //PID_ENABLED
  dataset.Misc2611 = read_raw(MISC2611);
  delay(100);
  dataset.Auxiliary = read_as_auxiliary();
  delay(100);
  dataset.Worker = read_as_worker();
  delay(100);

  return dataset;
} //ADS_Data ADS_Module::return_updated()