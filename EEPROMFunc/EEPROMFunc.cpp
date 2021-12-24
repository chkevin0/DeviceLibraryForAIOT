/*
============================================
	Arduino Atmega-1290 device library code，
	by 210312 KevinYW-Chen.
	
	EEPROM is used to store the setting parameters of the instrument, 
	setting parameters will be imported into the system when instrument is turned on.

	Vendor  : https://www.arduino.cc/
	Github	: https://github.com/chkevin0/DeviceLibraryForAIOT

	Release	date : 2021-07-14
	
	Remark:
		The memory location of the parameter needs to refer to the file "BootParameters.xlsx".
============================================
*/

#include "EEPROMFunc.h"

#define HMI      Serial2
#define RS485    Serial3 

//---------------------------Write--------------------------
void EEPROM_WriteDouble	(int TypesOffset, int column, int Row, double value_double)
{
	int EEPROM_address;

	EEPROM.setMemPool(0,0);
	EEPROM_address = (TypesOffset<<8) + (column<<4) + Row;
	EEPROM.updateDouble(EEPROM_address, value_double);

	return 0;
}

void EEPROM_WriteInt (int TypesOffset, int column, int Row, int value_int)
{
	int EEPROM_address;

	EEPROM.setMemPool(0,0);
	EEPROM_address = (TypesOffset<<8) + (column<<4) + Row;
	EEPROM.updateInt(EEPROM_address, value_int);

	return 0;
}

void EEPROM_WriteByte (int TypesOffset, int column, int Row, uint8_t value_byte)
{
	int EEPROM_address;

	EEPROM.setMemPool(0,0);
	EEPROM_address = (TypesOffset<<8) + (column<<4) + Row;
	EEPROM.updateByte(EEPROM_address, value_byte);

	return 0;
}


//---------------------------Read--------------------------
double EEPROM_ReadDouble(int TypesOffset, int column, int Row)
{
	int EEPROM_address;
	EEPROM_address = (TypesOffset<<8) + (column<<4) + Row;

	return EEPROM.readDouble(EEPROM_address);
}

int EEPROM_ReadInt(int TypesOffset, int column, int Row)
{
	int EEPROM_address;
	EEPROM_address = (TypesOffset<<8) + (column<<4) + Row;

	return EEPROM.readInt(EEPROM_address);
}

uint8_t EEPROM_ReadByte(int TypesOffset, int column, int Row)
{
	int EEPROM_address;
	EEPROM_address = (TypesOffset<<8) + (column<<4) + Row;

	return EEPROM.readByte(EEPROM_address);
}