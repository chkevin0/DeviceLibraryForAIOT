/*
============================================
	Human Machine Interface (HMI) device library code
	by 210312 KevinYW-Chen
	HMI Product Munmer : TJC8048x570_011c 

	Vendor  : http://www.tjc1688.com/Product/xuanxingzhinan/
	CMD Information : http://wiki.tjc1688.com/doku.php?id=start
	Github	: https://github.com/chkevin0/DeviceLibraryForAIOT

	Release	date : 2021-04-06
	ChangeLog:
		2021-04-21
		Optimize the function of "HMI_GetStatus", first save the message in char and then merge into "String".

		2021-05-06
		Add the functions "HMI_ChangePicture" for change the picture on the screen and 
		"HMI_ChangePco" for change the font color.

		2021-08-18
		Add the functions "HMI_SendStr_RecordErrorCode" and HMI_SendStr_RecordErrorCode_NoDataLength.
		Define the format of Error code serial data.
	
	Remark:
		SC16IS750 : I2C/SPI-to-UART modul.
============================================
*/

#include "HMICtrlFunc.h"

#define HMI      Serial2
#define RS485    Serial3 

//HMI device return Serial data format
#define Invalid_Instruction			0x00
#define Success						0x01
#define Invalid_ControlID			0x02
#define Invalid_PageID				0x03
#define Invalid_ImageID				0x04
#define Invalid_FontID				0x05
#define FileOperation_Failed		0x06
#define CRC_Failed					0x09
#define Invalid_BaudRate			0x11
#define Invalid_CurveID_ChannelNum	0x12
#define Invalid_Variable			0x1A
#define Invalid_VarOperation		0x1B
#define AsgmtOP_Failed				0x1C
#define EEPROM_OP_Failed			0x1D
#define Invalid_NumberOfParameters	0x1E
#define IO_OP_Failed				0x1F
#define Incorrect_EscapeChar		0x20
#define VarName_TooLong				0x23
#define SerialBuffer_Overflow		0x24

//-----------------------------------------------------------------------------------------------------
//-                         Send command to HMI                                                       -
//-----------------------------------------------------------------------------------------------------

void HMI_SendStr(String command) 								//for HMI (SC16IS750)
{
	for (int i = 0; i < command.length(); i++)
    HMI.write(command[i]);

	for (int i = 0; i < 3; i++) 								//OxFFFFFF:HMI CMD EndCode
    HMI.write(0xff);

	delay(250);

	return 0;
}

void HMI_ShowDataToTXT(String objname, String TXTContent)
{
	HMI_SendStr(objname + ".txt=\"" + TXTContent + "\"");
}

void HMI_ChangePage(String pagename)
{
	HMI_SendStr("page " + pagename);
}

void HMI_ChangePicture(String pagename, String objname, String PictureID)
{
  HMI_SendStr(pagename + "." + objname + ".pic=" + PictureID);
}

void HMI_ChangePco(String pagename, String objname, String PcoID)
{
  HMI_SendStr(pagename + "." + objname + ".pco=" + PcoID);
}



bool HMI_SendStr_RecordErrorCode(String command) 				//for HMI (SC16IS750)
{
	String 	RspMessage_str;
	byte	RspMessage_byte_ptr;
	uint8_t RspMessage_int8_ptr;
	uint8_t message_int8_array[4];
	byte    message_byte_array[4];
	uint8_t	counter = 0, TimeOut = 5;
	double  delay_time = 150;									//Minimum time : 45ms
																//If the setting is less than 45 ms, an abnormality will occur.

	while(1){
		while(HMI.available() > 0)    HMI.read();				//Clear HMI buff 

		//--------------Release CMD to HMI---------------------
		for (int i = 0; i < command.length(); i++)	HMI.write(command[i]);
		for (int i = 0; i < 3; i++) 				HMI.write(0xff);											//OxFFFFFF:HMI CMD EndCode
		delay(delay_time);			

		//--------------Get Response Message from HMI--------------
		if (HMI_GetStatus(&RspMessage_str, &RspMessage_int8_ptr, message_byte_array) ){

			RPI_Write_ErrorLog(command);																		//可移除
			RPI_Write_ErrorLog( "RspMessage_str : " + String(byte(RspMessage_str[0]))+"," + String(byte(RspMessage_str[1]))+"," 
								+ String(byte(RspMessage_str[2]))+","+ String(byte(RspMessage_str[3])) );		//可移除
			if (RspMessage_str[0] == Success){
				RPI_Write_ErrorLog("Instruction Success");						
				delay(delay_time);
				return 1;
			}
			else{
				switch(RspMessage_str[0]){
					case Invalid_Instruction:
						RPI_Write_ErrorLog("Invalid Instruction");
						break;
					case Invalid_ControlID:
						RPI_Write_ErrorLog("Invalid Control ID");
						break;
					case Invalid_PageID:
						RPI_Write_ErrorLog("Invalid Page ID");
						break;
					case Invalid_ImageID:
						RPI_Write_ErrorLog("Invalid Image ID");
						break;
					case Invalid_FontID:
						RPI_Write_ErrorLog("Invalid Font ID");
						break;
					case FileOperation_Failed:
						RPI_Write_ErrorLog("File Operation Failed");
						break;
					case CRC_Failed:
						RPI_Write_ErrorLog("CRC Failed");
						break;
					case Invalid_BaudRate:
						RPI_Write_ErrorLog("Invalid baud rate setting");
						break;
					case Invalid_CurveID_ChannelNum:
						RPI_Write_ErrorLog("Invalid curve control ID number or channel number");
						break;
					case Invalid_Variable:
						RPI_Write_ErrorLog("Invalid variable name");
						break;
					case Invalid_VarOperation:
						RPI_Write_ErrorLog("Invalid Variable operation");
						break;
					case AsgmtOP_Failed:
						RPI_Write_ErrorLog("Assignment Operation Failed");
						break;
					case EEPROM_OP_Failed:
						RPI_Write_ErrorLog("EEPROM Operation Failed");
						break;
					case Invalid_NumberOfParameters:
						RPI_Write_ErrorLog("Invalid Number Of Parameters");
						break;
					case IO_OP_Failed:
						RPI_Write_ErrorLog("IO Operation Failed");
						break;
					case Incorrect_EscapeChar:
						RPI_Write_ErrorLog("Incorrect Escape Characters");
						break;
					case VarName_TooLong:
						RPI_Write_ErrorLog("Variable Name Too Long");
						break;
					case SerialBuffer_Overflow:
						RPI_Write_ErrorLog("Serial Buffer Overflow");
						break;
					
					default:
		       			RPI_Write_ErrorLog( "Default RspMessage_str: " + String(byte(RspMessage_str[0])) ) ;
		      			break;

				}
				break;
			}
		}
		//Did not Get Response Message from HMI
		else{
			if(counter > TimeOut){
				RPI_Write_ErrorLog("TimeOut");
				break;
			}
			RPI_Write_ErrorLog("TimeOut_counter : "+String(counter));
			counter++;
		}
	}
	delay(delay_time);
	return 0;											//If get ERROR or time out ，then return 0

}

bool HMI_SendStr_RecordErrorCode_NoDataLength (String command)
{
	String 	RspMessage_str;
	byte	RspMessage_byte_ptr;
	uint8_t RspMessage_int8_ptr;
	uint8_t message_int8_array[4];
	byte    message_byte_array[4];
	uint8_t	counter = 0, TimeOut = 5;
	double  delay_time = 150;							//Minimum time : 45ms
														//If the setting is less than 45 ms, an abnormality will occur.


	while(1){
		while(HMI.available() > 0)    HMI.read();		//Clear HMI buff 

		//--------------Release CMD to HMI---------------------
		for (int i = 0; i < command.length(); i++)	HMI.write(command[i]);
		for (int i = 0; i < 3; i++) 				HMI.write(0xff);											//OxFFFFFF:HMI CMD EndCode
		delay(delay_time);			

		//--------------Get Response Message from HMI--------------
		if (HMI_GetStatus(&RspMessage_str, &RspMessage_int8_ptr, message_byte_array) ){

			RPI_Write_ErrorLog_NoDataLength(command);															//可移除
			RPI_Write_ErrorLog_NoDataLength( "RspMessage_str : " + String(byte(RspMessage_str[0]))+"," + String(byte(RspMessage_str[1]))+"," 
								+ String(byte(RspMessage_str[2]))+","+ String(byte(RspMessage_str[3])) );		//可移除
			if (RspMessage_str[0] == Success){
				RPI_Write_ErrorLog_NoDataLength("Instruction Success");						
				delay(delay_time);
				return 1;
			}
			else{
				switch(RspMessage_str[0]){
					case Invalid_Instruction:
						RPI_Write_ErrorLog_NoDataLength("Invalid Instruction");
						break;
					case Invalid_ControlID:
						RPI_Write_ErrorLog_NoDataLength("Invalid Control ID");
						break;
					case Invalid_PageID:
						RPI_Write_ErrorLog_NoDataLength("Invalid Page ID");
						break;
					case Invalid_ImageID:
						RPI_Write_ErrorLog_NoDataLength("Invalid Image ID");
						break;
					case Invalid_FontID:
						RPI_Write_ErrorLog_NoDataLength("Invalid Font ID");
						break;
					case FileOperation_Failed:
						RPI_Write_ErrorLog_NoDataLength("File Operation Failed");
						break;
					case CRC_Failed:
						RPI_Write_ErrorLog_NoDataLength("CRC Failed");
						break;
					case Invalid_BaudRate:
						RPI_Write_ErrorLog_NoDataLength("Invalid baud rate setting");
						break;
					case Invalid_CurveID_ChannelNum:
						RPI_Write_ErrorLog_NoDataLength("Invalid curve control ID number or channel number");
						break;
					case Invalid_Variable:
						RPI_Write_ErrorLog_NoDataLength("Invalid variable name");
						break;
					case Invalid_VarOperation:
						RPI_Write_ErrorLog_NoDataLength("Invalid Variable operation");
						break;
					case AsgmtOP_Failed:
						RPI_Write_ErrorLog_NoDataLength("Assignment Operation Failed");
						break;
					case EEPROM_OP_Failed:
						RPI_Write_ErrorLog_NoDataLength("EEPROM Operation Failed");
						break;
					case Invalid_NumberOfParameters:
						RPI_Write_ErrorLog_NoDataLength("Invalid Number Of Parameters");
						break;
					case IO_OP_Failed:
						RPI_Write_ErrorLog_NoDataLength("IO Operation Failed");
						break;
					case Incorrect_EscapeChar:
						RPI_Write_ErrorLog_NoDataLength("Incorrect Escape Characters");
						break;
					case VarName_TooLong:
						RPI_Write_ErrorLog_NoDataLength("Variable Name Too Long");
						break;
					case SerialBuffer_Overflow:
						RPI_Write_ErrorLog_NoDataLength("Serial Buffer Overflow");
						break;
					
					default:
		       			RPI_Write_ErrorLog_NoDataLength( "Default RspMessage_str: " + String(byte(RspMessage_str[0])) ) ;
		      			break;

				}
				break;
			}
		}
		//Did not Get Response Message from HMI
		else{
			if(counter > TimeOut){
				RPI_Write_ErrorLog_NoDataLength("TimeOut");
				break;
			}
			RPI_Write_ErrorLog_NoDataLength("TimeOut_counter : "+String(counter));
			counter++;
		}
	}
	delay(delay_time);
	return 0;											//If get ERROR or time out ，then return 0

}

//-----------------------------------------------------------------------------------------------------
//-                         Read messages from HMI                                                    -
//-----------------------------------------------------------------------------------------------------
bool HMI_ReadStr(String* message_ptr)
{
	char      RspTmp_char;
	String    RspData = "";
	String    RspMessage_str = "";
  	int       NumberOfRspMessage = 0;

 	if(HMI.available() > 0) delay(150);
  	while(HMI.available() > 0){
    	RspTmp_char = HMI.read();
    	RspMessage_str += RspTmp_char;
  	}
  	NumberOfRspMessage = RspMessage_str.length();

  	if(RspMessage_str[0] == 0x02 && RspMessage_str[NumberOfRspMessage-1] == 0x03){
    	*message_ptr = RspMessage_str;
    	return 1;
  	}

    return 0;
}

bool HMI_GetStatus(String* message_ptr, uint8_t* message_int8_ptr , byte* message_byte_array)
{
	char      RspTmp_char;
	byte 	  RspTmp_byte;
	uint8_t   RspTmp_int8;

	String    RspData = "";
	String    RspMessage_str;
  	int       NumberOfRspMessage = 0;
  	uint8_t   RspTmp_int8_array[3];
  	byte 	  RspTmp_byte_array[4]={0};

  	int i;

 	RspMessage_str = "";
 	//------------------------------------
 	
 	if(HMI.available() > 0){
	  	while(HMI.available() > 0){
	    	RspTmp_char = HMI.read();
	    	RspMessage_str += RspTmp_char;
	  	}
	  	*message_ptr = RspMessage_str;
	  	NumberOfRspMessage = RspMessage_str.length();
	  	return 1;
	}
	
	//------------方法1------------------------
 	/*
 	if(HMI.available() > 0){
 		RspMessage_str = HMI.read();
 		while(HMI.available() > 0)    HMI.read();			//Clear HMI buff 

 		*message_ptr = RspMessage_str;
	  	NumberOfRspMessage = RspMessage_str.length();
	  	return 1;
 	}
 	*/
 	//--------------方法2----------------------
 	/*
 	if(HMI.available() > 0){
	  	while(HMI.available() > 0){
	    	RspTmp_char = HMI.read();
	    	RspMessage_str += byte(RspTmp_char);
	  	}
	  	*message_ptr = RspMessage_str;
	  	NumberOfRspMessage = RspMessage_str.length();
	  	return 1;
	 }
	*/
	//--------------方法3----------------------
 	/*
 	if(HMI.available() > 0){
	  	while(HMI.available() > 0){
	    	RspTmp_byte = HMI.read();
	    	RspTmp_char = RspTmp_byte;
	    	RspMessage_str += RspTmp_char;
	  	}
	  	*message_ptr = RspMessage_str;
	  	NumberOfRspMessage = RspMessage_str.length();
	  	return 1;
	 }
	*/
 	//------------方法4------------------------
 	/*
 	if(HMI.available() > 0){
 		RspTmp_int8 = HMI.read();
 		while(HMI.available() > 0)    HMI.read();			//Clear HMI buff 

 		*message_int8_ptr = RspTmp_int8;
	  	NumberOfRspMessage = RspMessage_str.length();
	  	return 1;
 	}
	*/
 	//------------方法5------------------------
 	/*
	if(HMI.available() > 0){
 		HMI.readBytes(RspTmp_byte_array, 4);
 		while(HMI.available() > 0)    HMI.read();			//Clear HMI buff 

 		*message_int8_ptr = RspTmp_byte_array[0];
 		for(i=0 ; i<4 ; i++){
 			message_byte_array[i] = RspTmp_byte_array[i];
	  	}
	  	return 1;
 	}
 	*/
 	//----------------------------------------
    return 0;
}


