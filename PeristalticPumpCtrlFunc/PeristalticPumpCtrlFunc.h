//PeristalticPumpCtrlFunc
#ifndef _PeristalticPumpCtrlFunc_H_
#define _PeristalticPumpCtrlFunc_H_
#include "Arduino.h"

void Peristaltic_PumpInit				(void);
//Peristaltic_SetAddr : Pump device address can only be set individually
void Peristaltic_SetAddr				(uint8_t SetAddress);
void Peristaltic_Launch					(uint8_t address);
void Peristaltic_Launch_Duration		(uint8_t address, float duration);
void Peristaltic_Stop					(uint8_t address);
void Peristaltic_SetSpeed				(uint8_t address, uint16_t PumpSpeed);
void Peristaltic_SetForward				(uint8_t address);
void Peristaltic_SetReverse				(uint8_t address);

int Peristaltic_ReadSpeed				(uint8_t address);
int Peristaltic_ReadMaxSpeed			(uint8_t address);
uint16_t Peristaltic_CaculateCRC		(uint8_t* Data,	uint16_t len);

#endif 