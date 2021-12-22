//HMICtrlFunc
#ifndef _PDFunc_H_
#define _PDFunc_H_
#include "Arduino.h"
#include <ADS1115.h>
#include <Wire.h>
#include <EEPROMex.h>

void 	PD_ADS1115_Initialize(uint8_t PD_Resolution, uint8_t PD_SamplingRate);
bool 	PD_ADS1115_GetLightIntensity(double* Light_Intensity);
int 	PD_ADS1115_GetSampleIntensityANDAbsorbance(double Standard_Intensity, double* Sample_Intensity, double* Sample_Absorbance);
#endif 