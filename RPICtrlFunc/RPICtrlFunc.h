#ifndef _RPICtrlFunc_H_
#define _RPICtrlFunc_H_
	#ifdef ARDUINO
	    #if ARDUINO < 100
	        #include "WProgram.h"
	    #else
	        #include "Arduino.h"
		#endif
	#endif
	
	//--------------------------Send--------------------------------
	void RPI_SendFuncDataToRPI		(byte function,String data);
	void RPI_SendFuncDataToRPI		(byte function);
	void RPI_WriteLog				(String log);
	void RPI_Write_ErrorLog			(String log);
	void RPI_Write_ErrorLog_NoDataLength(String data);
	void RPI_SendResultToRPI_Platform(double Result_double, int select);

	//--------------------------Get--------------------------------
	int  RPI_GetCMDMessage			(String* CMDMessage);
	void RPI_GetIP					(String* IP);
	void RPI_GetTime				(String* time);
	void RPI_CreatTXTNameByTime		(String* time);
	void RPI_CreatpHCalTXTNameByTime(String* time);
	//--------------------------UU--------------------------------
	void UU_RPI_GetIP				(String* IP);

#endif 