//HMICtrlFunc
#ifndef _HMICtrlFunc_H_
#define _HMICtrlFunc_H_

#include <Arduino.h>
#include <RPICtrlFunc.h>

void HMI_SendStr					(String command);
void HMI_ShowDataToTXT				(String objname, String TXTContent);
void HMI_ChangePage					(String pagename);
void HMI_ChangePicture				(String pagename, String objname, String PictureID);
void HMI_ChangePco					(String pagename, String objname, String PictureID);
bool HMI_SendStr_RecordErrorCode	(String command);
bool HMI_SendStr_RecordErrorCode_NoDataLength	(String command);

bool HMI_ReadStr					(String* message_ptr);
bool HMI_GetStatus					(String* message_ptr, uint8_t* message_int8_ptr, byte* message_byte_array);
#endif 