/*******************************************************************************
 * @file    quad_module.cpp
 * @brief   Module to organize functions for quadstat  
 *
 * @cite    Adapted quad_module.cpp, XPOD_V3.2.3 by Ajay Kendagal
 * 
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    July 29, 2025
 ******************************************************************************/
#include "quad_module.h"

/**************************************************************************/
 /*!
 *    @brief  sets status to true 
 */
/**************************************************************************/
QUAD_Module::QUAD_Module()
{
  status = true;
}

/**************************************************************************/
 /*!
 *    @brief  "begins" the Quadstat sensors 
 *    @return returns true if called successfully
 */
/**************************************************************************/
bool QUAD_Module::begin()
{
  alpha_one = MCP342x(ALPHA_ONE_ADDR);
  alpha_two = MCP342x(ALPHA_TWO_ADDR);

  MCP342x::generalCallReset();
  delay(1);

  return status;
}

/**************************************************************************/
 /*!
 *    @brief   
 *    @return 
 */
/**************************************************************************/
QUAD_Data QUAD_Module::return_updated()
{
  MCP342x::Config status;
  String quad_data;
  long value = 0;
  int16_t value_smallguy;
  QUAD_Data dataset;

  // Initiate a conversion; convertAndRead() will wait until it can be read
  alpha_one.convertAndRead(MCP342x::channel1, MCP342x::oneShot, MCP342x::resolution16, 
                          MCP342x::gain1, 1000000, value, status);
  value_smallguy = value;
  dataset.QS1_C1 = value_smallguy;

  alpha_one.convertAndRead(MCP342x::channel2, MCP342x::oneShot, MCP342x::resolution16, 
                          MCP342x::gain1, 1000000, value, status);
  value_smallguy = value;
  dataset.QS1_C2 = value_smallguy;

  alpha_one.convertAndRead(MCP342x::channel3, MCP342x::oneShot, MCP342x::resolution16, 
                          MCP342x::gain1, 1000000, value, status);
  value_smallguy = value;
  dataset.QS2_C1 = value_smallguy;

  alpha_one.convertAndRead(MCP342x::channel4, MCP342x::oneShot, MCP342x::resolution16, 
                          MCP342x::gain1, 1000000, value, status);
  value_smallguy = value;
  dataset.QS2_C2 = value_smallguy;

  alpha_two.convertAndRead(MCP342x::channel1, MCP342x::oneShot, MCP342x::resolution16, 
                          MCP342x::gain1, 1000000, value, status);
  value_smallguy = value;
  dataset.QS3_C1 = value_smallguy;

  alpha_two.convertAndRead(MCP342x::channel2, MCP342x::oneShot, MCP342x::resolution16, 
                          MCP342x::gain1, 1000000, value, status);
  value_smallguy = value;
  dataset.QS3_C2 = value_smallguy;

  alpha_two.convertAndRead(MCP342x::channel3, MCP342x::oneShot, MCP342x::resolution16, 
                          MCP342x::gain1, 1000000, value, status);
  value_smallguy = value;
  dataset.QS4_C1 = value_smallguy;

  alpha_two.convertAndRead(MCP342x::channel4, MCP342x::oneShot, MCP342x::resolution16, 
                          MCP342x::gain1, 1000000, value, status);
  value_smallguy = value;
  dataset.QS4_C2 = value_smallguy;

  return dataset;
}