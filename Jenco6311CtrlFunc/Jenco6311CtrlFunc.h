#ifndef _Jenco6311CtrlFunc_H_
#define _Jenco6311CtrlFunc_H_
#ifdef ARDUINO
    #if ARDUINO < 100
        #include "WProgram.h"
    #else
        #include "Arduino.h"
	#endif
#endif
#include "PeristalticPumpCtrlFunc.h"

//---------------Jenco6311 Status Infomation-------------------------------------------------
void Jenco6311_GetData			(uint8_t address, String* RspData_ptr, int* NumberOfRsp_ptr);
void Jenco6311_GetData			(uint8_t address, String* RspData_ptr);
double Jenco6311_GetpH			(uint8_t address, String* pH_String_ptr);
bool Jenco6311_GetAbsolute_mV	(uint8_t address, String* Absolute_mV_String_ptr, double* Absolute_mV_double_ptr);
bool Jenco6311_GetRelative_mV 	(uint8_t address, String* Relative_mV_String_ptr, double* Relative_mV_double_ptr);

//---------------Jenco6311 LCD Display Infomation--------------------------------------------
void Jenco6311_GetLCDInfo		(uint8_t address, String* RspData_ptr, int* NumberOfRsp_ptr);
// void Jenco6311_GetLCDInfo	(uint8_t address, String* RspData_ptr);
void debug_display				(String* RspData_ptr, int NumberOfLine);
void debug_display_init			(void);
void Jenco6311_CheckTest		(uint8_t address);
bool Jenco6311_CheckMinor_Buff	(uint8_t address, char number_char);
bool Jenco6311_CheckMinor_SAVE	(uint8_t address);
bool Jenco6311_CheckMinor_PASS	(uint8_t address);

bool Jenco6311_CheckMajor_Cal	(uint8_t address);
bool Jenco6311_CheckMajor_PH	(uint8_t address);
bool Jenco6311_CheckMajor_Pt	(uint8_t address);
bool Jenco6311_CheckMajor_rES	(uint8_t address);
bool Jenco6311_CheckMajor_tHEr	(uint8_t address);
bool Jenco6311_CheckMajor_700	(uint8_t address);
bool Jenco6311_CheckMajor_686	(uint8_t address);
bool Jenco6311_CheckMajor_1246	(uint8_t address);
bool Jenco6311_CheckMajor_1001	(uint8_t address);
bool Jenco6311_CheckMajor_918	(uint8_t address);
bool Jenco6311_CheckMajor_401	(uint8_t address);
bool Jenco6311_CheckMajor_400	(uint8_t address);
bool Jenco6311_CheckMajor_168	(uint8_t address);
bool Jenco6311_CheckMajor_EFF	(uint8_t address,double* EFF_Value);
//---------------Jenco6311 Button Control Function--------------------------------------------
int Jenco6311_ButtonCtrl		(uint8_t address, int command);					
int Jenco6311_PressMode			(uint8_t address);	//COMMAND = 11
int Jenco6311_PressCal			(uint8_t address);		//COMMAND = 12
int Jenco6311_PressUp			(uint8_t address);	//COMMAND = 13
int Jenco6311_PressDown			(uint8_t address);		//COMMAND = 14
int Jenco6311_PressEnter		(uint8_t address);	//COMMAND = 15
int Jenco6311_PressMode2s		(uint8_t address);		//COMMAND = 16
int Jenco6311_PressWash			(uint8_t address);	//COMMAND = 17
int Jenco6311_PressWash2S		(uint8_t address);		//COMMAND = 18


#endif 