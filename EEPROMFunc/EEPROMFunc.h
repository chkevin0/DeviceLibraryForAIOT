//HMICtrlFunc
#ifndef _EEPROMFunc_H_
#define _EEPROMFunc_H_
#include "Arduino.h"
#include <EEPROMex.h>

//---------------------------Write--------------------------
void EEPROM_WriteDouble			(int TypesOffset, int column, int Row, double value_double);
void EEPROM_WriteInt 			(int TypesOffset, int column, int Row, int value_int);
void EEPROM_WriteByte 			(int TypesOffset, int column, int Row, uint8_t value_byte);

//---------------------------Read--------------------------
double 	EEPROM_ReadDouble		(int TypesOffset, int column, int Row);
int 	EEPROM_ReadInt			(int TypesOffset, int column, int Row);
uint8_t EEPROM_ReadByte			(int TypesOffset, int column, int Row);

#endif 