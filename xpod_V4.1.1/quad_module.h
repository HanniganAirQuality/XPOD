/*******************************************************************************
 * @file    quad_module.h
 * @brief   Module to organize functions for quadstat  
 *
 * @cite    Adapted quad_module.h, XPOD_V3.2.3 by Ajay Kendagal
 * 
 * @author  Percy Smith, percy.smith@colorado.edu
 * @date    July 29, 2025
 ******************************************************************************/
#ifndef _QUAD_Module_H
#define _QUAD_Module_H

#include <Arduino.h>
#include <stdint.h>
#include <MCP342x.h>

#include "xpod_node.h"

/****************** STRUCTS, OBJECTS ********************/
/*! Quad data structure (ALL DATA) as respective datatypes */
struct QUAD_Data {
  int16_t QS1_C1;
  int16_t QS1_C2;
  int16_t QS2_C1;
  int16_t QS2_C2;
  int16_t QS3_C1;
  int16_t QS3_C2;
  int16_t QS4_C1;
  int16_t QS4_C2;
};

/****************** CLASSES ********************/
class QUAD_Module
{
  public:
    QUAD_Module();
    bool begin();

    QUAD_Data return_updated();

  private:
    MCP342x alpha_one;
    MCP342x alpha_two;
    bool status;
};

#endif //_QUAD_Module_H