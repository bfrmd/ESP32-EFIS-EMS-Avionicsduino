#ifndef RDAMCANBUS_H
#define RDAMCANBUS_H

#include "RDAMglobals.h"

void canInit();
void TaskCANcode(void* parameter);
float iirFilter(float previousFilteredValue, float currentValue, float iirFilterCoefficient);  // this function is declared extern in RDAMsensors.h

#endif