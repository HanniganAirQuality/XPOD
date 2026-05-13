/*******************************************************************************
 * @file    ads_module.h
 * @brief   
 *
 * @author  Ajay Kandagal, ajka9053@colorado.edu
 * @editor  Percy Smith, percy.smith@colorado.edu
 * @date    July 25, 2025
 * 
 * @log     Changed CO to read int16_t (not needing decimals??? why he do dat??)
 ******************************************************************************/
#ifndef _ADS_MODULE_H
#define _ADS_MODULE_H

#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

#define FIGARO3_ENABLED       1
#define FIGARO4_ENABLED       1

enum ads_sensor_id_e
{
    ADS_SENSOR_FIG2600 = 0,
    ADS_SENSOR_FIG2602,
    ADS_SENSOR_FIG3,
    ADS_SENSOR_FIG4,
    ADS_HEATER_FIG3,
    ADS_HEATER_FIG4,
    ADS_SENSOR_PID,
    ADS_SENSOR_E2V,
    ADS_SENSOR_CO,
    ADS_SENSOR_COUNT
};

struct ads_module_t
{
    uint8_t addr;
    int8_t channel;
    bool status;
    Adafruit_ADS1115 module;
};

class ADS_Module {
  public:
    ADS_Module();
    bool begin();
    
    int16_t read_co_aux();
    int16_t read_co_main();
    uint16_t read_raw(ads_sensor_id_e ads_sensor_id);

    String read4sd();
    String read4print();
    String read4sd_raw();
    String read4print_raw();

  private:
    ads_module_t ads_module[ADS_SENSOR_COUNT];
};

#endif  //_ADS_MODULE_H
