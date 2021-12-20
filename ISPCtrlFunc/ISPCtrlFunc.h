#ifndef _ISPCtrlFunc_H_
#define _ISPCtrlFunc_H_
#ifdef ARDUINO
    #if ARDUINO < 100
        #include "WProgram.h"
    #else
        #include "Arduino.h"
	#endif
#endif
#include "HMICtrlFunc.h"
// #include "I2Cdev.h"
// #include <ADS1115.h>
// #include <Wire.h>
// #include <EEPROMex.h>

//-------------------------------------------------------------------------------------------------	
	void ISP_Initialization				(uint8_t address_number);
	void ISP_SpeedSet					(uint8_t address_number, int SpeedRange_int);
	void ISP_Init_SpeedSet				(uint8_t address_number, int SpeedRange_int);

	void ISP_Terminate					(uint8_t address_number);
	void ISP_SendRawCMD					(uint8_t address_number, String RawCMD);		//Example : ISP_SendRawCMD(2, "IA1000OA900R")
	void ISP_ValveCtrl_Input			(uint8_t address_number);
	void ISP_ValveCtrl_Output			(uint8_t address_number);
	void ISP_MovePlunger_AbsPos 		(uint8_t address_number, int position_int);
	void ISP_MovePlunger_AbsPosRecPos	(uint8_t address_number, int position_int, int* RecordPosition_ptr);
	void ISP_MovePlunger_RelDisplUp		(uint8_t address_number, int RelDisplacement_int, int threshold, int *RecordPosition_ptr);
	void ISP_MovePlunger_RelDisplDown	(uint8_t address_number, int RelDisplacement_int, int threshold, int *RecordPosition_ptr);
	void ISP_FlushPump					(uint8_t address_number, int frequency_int);

//-------------------------------------------------------------------------------------------------	
	int  ConfCorOfRspMsg				(String RspMessage);							// Confirm Correctness Of Responses Message
	bool ISP_CheckStatus				(uint8_t address_number, uint8_t* ErrorCode);	// 0:Busy	/ 1:Idle
	bool ISP_CheckIdle					(uint8_t address_number);						// 0:Busy	/ 1:Idle
	bool ISP_CheckIdle_TimeOut			(uint8_t address_number, uint16_t TimeOut_Sec);	// 0:Busy	/ 1:Idle

//----------------測試區----------------------------------------------------------------------------	
	void MovePlungerAbsPos_test			(uint8_t address_number, String position);


#endif 