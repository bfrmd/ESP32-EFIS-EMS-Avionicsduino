#ifndef RDAMSENSORS_H
#define RDAMSENSORS_H

#include "RDAMglobals.h"

void sensorsInit();
void sensorsRead();
void airDataComputation();
void displayDebuggingData();
extern float iirFilter(float previousFilteredValue, float currentValue, float iirFilterCoefficient);

#endif