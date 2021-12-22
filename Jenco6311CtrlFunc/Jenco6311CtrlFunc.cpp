/*
============================================
	Jenco 6311 pH/ORP Controller (pH Controller) device library code
	by 210312 KevinYW-Chen.
	
	The controller needs to be used with METTLER's electrodes.

	Vendor(Jenco)  : http://www.jenco.com.tw/product/6311/
	Vendor(METTLER)  : https://www.mt.com/tw/zt/home.html
	Github	: https://github.com/chkevin0/DeviceLibraryForAIOT

	Release	date : 2021-04-09
	ChangeLog:
		2021-04-22
		Optimize the function of "Jenco6311_GetData", confirm whether Rsp.date length is 32 bytes.

		2021-07-20
		Add the functions "Jenco6311_CheckMajor_EFF"，electrode efficiency(EFF).
	
	Remark:
		The information of small icon(Minor information) does not match the Spec.
============================================
*/

#include "Jenco6311CtrlFunc.h"
#define HMI	Serial2 
#define RS485 Serial3 

//---------------Jenco6311 Status Infomation-------------------------------------------------
void Jenco6311_GetData(uint8_t address, String* RspData_ptr, int* NumberOfRsp_ptr)
{
	uint8_t CMD[2]			  =	{(address + 0x80) , 0x00};		//0x00:Get display data during normal operation
	char RspTmp_char;
	String FullStatusInfo_str = "";
	int x , NumberOfRsp;
	
	while(RS485.available()>0) RS485.read();					//Clear buffer of RS485 Read-port

	//-------Releass Device Address & Check Acknowledge------------
	while(1){
		RS485.write(CMD[0]);									//Releass CMD
		delay(10);

		if (RS485.available() > 0){
			RspTmp_char = RS485.read();
			if(RspTmp_char == 0x06)	break;						//Acknowledge must be equal to 6
		}
		else continue;
	}

	//-------Releass Command & Check Responses Data-----------------
	
	RS485.write(CMD[1]);										//Releass CMD
	delay(100);

	FullStatusInfo_str = "";
	while (RS485.available() > 0){
		RspTmp_char 	   	= RS485.read();
		FullStatusInfo_str += RspTmp_char;	
	}
	NumberOfRsp		   	= FullStatusInfo_str.length();
	
	if(NumberOfRsp == 32){
		*RspData_ptr 		   = FullStatusInfo_str;
		*NumberOfRsp_ptr	   = NumberOfRsp;
	}
	else{
		*RspData_ptr 		   = "Error";
		*NumberOfRsp_ptr	   = 5;
	}
	return 0;
}


void Jenco6311_GetData(uint8_t address, String* RspData_ptr)
{
	int NumberOfRsp;

	Jenco6311_GetData(address, RspData_ptr, &NumberOfRsp);
}

double Jenco6311_GetpH(uint8_t address, String* pH_String_ptr)
{
	String 	RspData;
	String  pH_temp_String = "";
	char 	temp_char;
	int 	NumberOfRsp, i;

	Jenco6311_GetData(address, &RspData, &NumberOfRsp);			//Get display data during normal operation.

	for( i=0 ; i<6 ; i++){										//GET pH Value(String)
		temp_char = RspData[i];
		pH_temp_String 	 += temp_char;
	}
	*pH_String_ptr = pH_temp_String;
	//*pH_String_ptr = RspData;

	if(pH_temp_String[0] == '-' || pH_temp_String[0] == ' ' || pH_temp_String[0] == '+'){
		if(pH_temp_String[0] == '-' ){
			return (double((
				(double(pH_temp_String[1]-0x30)) * 1000 +
				(double(pH_temp_String[2]-0x30)) * 100 +
				(double(pH_temp_String[4]-0x30)) * 10 +
				(double(pH_temp_String[5]-0x30))
				)))/-100;
		}
		else{
			return (double((
				(double(pH_temp_String[1]-0x30)) * 1000 +
				(double(pH_temp_String[2]-0x30)) * 100 +
				(double(pH_temp_String[4]-0x30)) * 10 +
				(double(pH_temp_String[5]-0x30))
				)))/100;
		}
	}
	else if(pH_temp_String[0] == 'O'){
		return 999;
	}
	else{
		return -999;
	}
}

bool Jenco6311_GetAbsolute_mV (uint8_t address, String* Absolute_mV_String_ptr, double* Absolute_mV_double_ptr)
{
	String 	RspData;
	String  Absolute_mV_temp_String = "";
	char 	temp_char;
	int 	NumberOfRsp, i;

	*Absolute_mV_double_ptr = 0;
	Jenco6311_GetData(address, &RspData, &NumberOfRsp);			//Get display data during normal operation.

	for( i=0 ; i<6 ; i++){
		temp_char = RspData[i+18];								//GET ORP Absolute mV
		Absolute_mV_temp_String += temp_char;
	}

	if( (Absolute_mV_temp_String[1] > 0x29 && Absolute_mV_temp_String[1] < 0x40 ) || Absolute_mV_temp_String[1] == 'V' || Absolute_mV_temp_String[1] == 'N'){
		*Absolute_mV_String_ptr = Absolute_mV_temp_String;
		
		if(Absolute_mV_temp_String[1] > 0x29 && Absolute_mV_temp_String[1] < 0x40 ){
			if(Absolute_mV_temp_String[0]=='+'){
				for(i=0 ; i<4 ; i++){
					(*Absolute_mV_double_ptr) = (*Absolute_mV_double_ptr)*10 + double(Absolute_mV_temp_String[i+2]-0x30);
				}
			}
			else{
				for(i=0 ; i<4 ; i++){
					(*Absolute_mV_double_ptr) = (*Absolute_mV_double_ptr)*10 + double(Absolute_mV_temp_String[i+2]-0x30);
				}
				(*Absolute_mV_double_ptr) = (*Absolute_mV_double_ptr)*-1;
			}
		}
		else if(Absolute_mV_temp_String[1] == 'V'){
			*Absolute_mV_double_ptr = 9999;
		}
		else{
			*Absolute_mV_double_ptr = -9999;
		}
		return 1;
	}
	else return 0;
}
bool Jenco6311_GetRelative_mV (uint8_t address, String* Relative_mV_String_ptr, double* Relative_mV_double_ptr)
{
	String 	RspData;
	String  Relative_mV_temp_String = "";
	char 	temp_char;
	int 	NumberOfRsp, i;

	Jenco6311_GetData(address, &RspData, &NumberOfRsp);			//Get display data during normal operation.

	for( i=0 ; i<6 ; i++){
		temp_char = RspData[i+24];								//GET ORP Relative mV
		Relative_mV_temp_String += temp_char;
	}

	if( (Relative_mV_temp_String[1] > 0x29 && Relative_mV_temp_String[1] < 0x40 ) || Relative_mV_temp_String[1] == 'V' || Relative_mV_temp_String[1] == 'N'){
		*Relative_mV_String_ptr = Relative_mV_temp_String;

		if(Relative_mV_temp_String[1] > 0x29 && Relative_mV_temp_String[1] < 0x40 ){
			if(Relative_mV_temp_String[0]=='+'){
				for(i=0 ; i<4 ; i++){
					(*Relative_mV_double_ptr) = (*Relative_mV_double_ptr)*10 + double(Relative_mV_temp_String[i+2]-0x30);
				}
			}
			else{
				for(i=0 ; i<4 ; i++){
					(*Relative_mV_double_ptr) = (*Relative_mV_double_ptr)*10 + double(Relative_mV_temp_String[i+2]-0x30);
				}
				(*Relative_mV_double_ptr) = (*Relative_mV_double_ptr)*-1;
			}
		}
		else if(Relative_mV_temp_String[1] == 'V'){
			*Relative_mV_double_ptr = 9999;
		}
		else{
			*Relative_mV_double_ptr = -9999;
		}
		return 1;
	}
	else return 0;
}

//---------------Jenco6311 LCD Display Infomation--------------------------------------------

void Jenco6311_GetLCDInfo(uint8_t address, String* RspData_ptr, int* NumberOfLCDInfo_ptr)
{
	uint8_t CMD[2]			  =	{(address + 0x80) , 0x01};		//0x01:To get the current LCD Display
	char RspTmp_char;
	String LCDInfo_str = "";
	int x , NumberOfLCDInfo;
	
	while(RS485.available()>0) RS485.read();					//Clear buffer of RS485 Read-port

  	//-------Releass Device Address & Check Acknowledge--------- 
	while(1){
		RS485.write(CMD[0]);									//Releass CMD
		delay(50);

		if (RS485.available() > 0){
			RspTmp_char = RS485.read();
			if(RspTmp_char == 0x06)	break;						//Acknowledge must be equal to 6
		}
		else continue;
	}

  	//-------Releass Command & Check LCD Infomation Data----------
 
	while(1){
		RS485.write(CMD[1]);									//Releass CMD
		delay(100);

		LCDInfo_str = "";
		while (RS485.available() > 0){
			RspTmp_char 		= RS485.read();
			LCDInfo_str 	   += RspTmp_char;
		}
		NumberOfLCDInfo	  	= LCDInfo_str.length();
		
		if(NumberOfLCDInfo == 16) break;
		else{
			*RspData_ptr		= "Error";
			*NumberOfLCDInfo_ptr= 5;
		}
	}
	*RspData_ptr		 = LCDInfo_str;
	*NumberOfLCDInfo_ptr = NumberOfLCDInfo;

	return 0;
}

void debug_display(String* RspData_ptr, int NumberOfLine)
{
	String LCDInfo_str = *RspData_ptr;

	if(NumberOfLine>0)
		SendStrToHmi("debug.t5.txt=\"LCDInfo:" + String(LCDInfo_str[0]) + String(LCDInfo_str[1]) + 
												 String(LCDInfo_str[2]) + String(LCDInfo_str[3]) + "\"");
	if(NumberOfLine>1)
		SendStrToHmi("debug.t6.txt=\"LCDInfo:" + String(LCDInfo_str[4]) + String(LCDInfo_str[5]) + 
												 String(LCDInfo_str[6]) + String(LCDInfo_str[7]) + "\"");
	if(NumberOfLine>2)
		SendStrToHmi("debug.t7.txt=\"LCDInfo:" + String(byte(LCDInfo_str[8]),HEX) + "," + String(byte(LCDInfo_str[9]),HEX) + "," +
												 String(byte(LCDInfo_str[10]),HEX) + "," + String(byte(LCDInfo_str[11]),HEX) + "\"");
	if(NumberOfLine>3)
		SendStrToHmi("debug.t8.txt=\"LCDInfo:" + String(byte(LCDInfo_str[12]),HEX) + "," + String(byte(LCDInfo_str[13]),HEX) + "," +
												 String(byte(LCDInfo_str[14]),HEX) + "," + String(byte(LCDInfo_str[15]),HEX) + "\"");
}

void debug_display_init(void)
{
	SendStrToHmi("debug.t5.txt=\"\"");
	SendStrToHmi("debug.t6.txt=\"\"");
	SendStrToHmi("debug.t7.txt=\"\"");
	SendStrToHmi("debug.t8.txt=\"\"");
}


bool Jenco6311_CheckMinor_Buff(uint8_t address, char number_char)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, x;

	Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);

	debug_display(&LCDInfo_str, 2);

	if(LCDInfo_str[4] == 'b' && LCDInfo_str[5] == 'U' &&
	   LCDInfo_str[6] == 'F' && LCDInfo_str[7] == number_char){
		return 1;
	}
	else return 0;
}

bool Jenco6311_CheckMinor_SAVE(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, x;

	Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);

	debug_display(&LCDInfo_str, 2);

	if(LCDInfo_str[4] == 'S' && LCDInfo_str[5] == 'A' &&
	   LCDInfo_str[6] == 'V' && LCDInfo_str[7] == 'E'){
		return 1;
	}
	else return 0;
}

bool Jenco6311_CheckMinor_PASS(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, x;

	Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);

	debug_display(&LCDInfo_str, 2);

	if(LCDInfo_str[4] == 'P' && LCDInfo_str[5] == 'A' &&
	   LCDInfo_str[6] == 'S' && LCDInfo_str[7] == 'S'){
		return 1;
	}
	else return 0;

}

bool Jenco6311_CheckMajor_Cal(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, x;

	Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);

	debug_display(&LCDInfo_str, 2);

	if(LCDInfo_str[0] == 'C' && LCDInfo_str[1] == 'A' &&
	   LCDInfo_str[2] == 'L' ){
		return 1;
	}
	else return 0;
}

bool Jenco6311_CheckMajor_PH(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(LCDInfo_str[1] == 'P' && LCDInfo_str[2] == 'H')	flag = 1;
	}

	if(flag == 1)	return 1;
	else 			return 0;
}

bool Jenco6311_CheckMajor_Pt(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(LCDInfo_str[0] == 'P' && LCDInfo_str[1] == 't')	flag = 1;
	}

	if(flag == 1)	return 1;
	else 			return 0;
}

bool Jenco6311_CheckMajor_rES(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(LCDInfo_str[0] == 'r' && LCDInfo_str[1] == 'E' && LCDInfo_str[2] == 'S')	flag = 1;
	}

	if(flag == 1)	return 1;
	else 			return 0;
}

bool Jenco6311_CheckMajor_tHEr(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(LCDInfo_str[0] == 't' && LCDInfo_str[1] == 'H' && LCDInfo_str[2] == 'E'  && LCDInfo_str[3] == 'r')	flag = 1;
	}

	if(flag == 1)	return 1;
	else 			return 0;
}

bool Jenco6311_CheckMajor_700(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(/*LCDInfo_str[0] == ' ' && */LCDInfo_str[1] == '7' && LCDInfo_str[2] == '0'  && LCDInfo_str[3] == '0')	flag = 1;
	}

	if(flag == 1)	return 1;
	else 			return 0;
}

bool Jenco6311_CheckMajor_686(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(/*LCDInfo_str[0] == ' ' && */LCDInfo_str[1] == '6' && LCDInfo_str[2] == '8'  && LCDInfo_str[3] == '6')	flag = 1;
	}

	if(flag == 1)	return 1;
	else 			return 0;
}

bool Jenco6311_CheckMajor_1246(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(LCDInfo_str[0] == '1' && LCDInfo_str[1] == '2' && LCDInfo_str[2] == '4'  && LCDInfo_str[3] == '6')	flag = 1;
	}

	if(flag == 1)	return 1;
	else 			return 0;
}

bool Jenco6311_CheckMajor_1001(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(LCDInfo_str[0] == '1' && LCDInfo_str[1] == '0' && LCDInfo_str[2] == '0'  && LCDInfo_str[3] == '1')	flag = 1;
	}

	if(flag == 1)	return 1;
	else 			return 0;
}

bool Jenco6311_CheckMajor_918(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(/*LCDInfo_str[0] == '1' &&*/ LCDInfo_str[1] == '9' && LCDInfo_str[2] == '1'  && LCDInfo_str[3] == '8')	flag = 1;
	}

	if(flag == 1)	return 1;
	else 			return 0;
}

bool Jenco6311_CheckMajor_401(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(/*LCDInfo_str[0] == '1' &&*/ LCDInfo_str[1] == '4' && LCDInfo_str[2] == '0'  && LCDInfo_str[3] == '1')	flag = 1;
	}

	if(flag == 1)	return 1;
	else 			return 0;
}

bool Jenco6311_CheckMajor_400(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(/*LCDInfo_str[0] == '1' &&*/ LCDInfo_str[1] == '4' && LCDInfo_str[2] == '0'  && LCDInfo_str[3] == '0')	flag = 1;
	}

	if(flag == 1)	return 1;
	else 			return 0;
}

bool Jenco6311_CheckMajor_168(uint8_t address)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(/*LCDInfo_str[0] == '1' &&*/ LCDInfo_str[1] == '1' && LCDInfo_str[2] == '6'  && LCDInfo_str[3] == '8')	flag = 1;
	}

	if(flag == 1)	return 1;
	else 			return 0;
}

bool Jenco6311_CheckMajor_EFF(uint8_t address,double* EFF_Value)
{
	String 		LCDInfo_str 	= "";
	int 		NumberOfRsp, DelayTime ,x;
	bool		flag			= 0;

	DelayTime = 75;
	flag = 0;
	for(x=0 ; x<4 ; x++){
		Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);
		debug_display(&LCDInfo_str, 2);
		delay(DelayTime);
		if(LCDInfo_str[0] == 'E' && LCDInfo_str[1] == 'F' && LCDInfo_str[2] == 'F'  /*&& LCDInfo_str[3] == '8'*/)	flag = 1;
	}

	if(flag == 1 && LCDInfo_str[6]>0x29 && LCDInfo_str[6]<0x40){
		if(LCDInfo_str[4]>0x29 && LCDInfo_str[4]<0x40){	
			*EFF_Value = (double((
						((double(LCDInfo_str[4]-0x30)) * 1000.0)+
						((double(LCDInfo_str[5]-0x30)) * 100.0)+
						((double(LCDInfo_str[6]-0x30)) * 10.0)+
						(double(LCDInfo_str[7]-0x30))
						)))/10.0;
		}
		else{
			*EFF_Value = (double((
						//((double(LCDInfo_str[4]-0x30)) * 1000.0)+
						((double(LCDInfo_str[5]-0x30)) * 100.0)+
						((double(LCDInfo_str[6]-0x30)) * 10.0)+
						(double(LCDInfo_str[7]-0x30))
						)))/10.0;
		}

		return 1;
	}
	else return 0;
}

int Jenco6311_ButtonCtrl(uint8_t address, int command)
{
	uint8_t CMD[2]			  =	{(address + 0x80) , command};
	char  	RspTmp_char;	
	String 	RspMessage_str = "";
	int 	x=0, NumberOfRsp;

	uint8_t ack_record[40]={0};

	while(RS485.read() > 0)	RS485.read();						//Clear buffer of RS485 Read-port

	//-------Releass Device Address & Check Acknowledge-----------------
	while(1){
		RS485.write(CMD[0]);									//Releass CMD
		delay(10);
		if (RS485.available() > 0){
			RspTmp_char 	= RS485.read();
			RspMessage_str 	= RspTmp_char;
			if(RspMessage_str[0] == 0x06) break;				//Acknowledge must be equal to 6
		}
		else continue;
	}
	//-------check numbers of Acknowledge-----------------
	// while(1){
	// 	RS485.write(CMD[0]);									//Releass CMD
	// 	delay(10);
	// 	if (RS485.available() > 0){
	// 		RspTmp_char = RS485.read();
	// 		RspMessage_str = RspTmp_char;
	// 		//if(RspMessage_str[0] == 0x06)	break;				//Acknowledge must be equal to 6
	// 		if(RspMessage_str[0] == 0x06){
	// 			SendStrToHmi("debug.t7.txt=\"Number of Acks:" + String(x) + "\"");
	// 			break;
	// 		}
	// 		else /*if (RspMessage_str[0] != 0x06)*/{
	// 			x++;
	// 		}
	// 	}
	// 	else continue;
	// }
	
	//-------Releass Command & Check Responses Data----------------------
	while(RS485.read() > 0)	RS485.read();						//Clear buffer of RS485 Read-port
	RspMessage_str = "";										//Clear string of Responses Message

	RS485.write(CMD[1]);										//Releass CMD
	delay(10);

	while(1){
		if (RS485.available() > 0){
			RspTmp_char = RS485.read();
			RspMessage_str = RspTmp_char;
			break;
		}
	}
	if 		(RspMessage_str[0] == 0x01) return 1;				//"1" then sent key is executed		
	else	return 2;											//"Unit is busy ，sent key is not executed" or "Error"

}

int Jenco6311_PressMode(uint8_t address)								//COMMAND = 11
{
	return Jenco6311_ButtonCtrl(address, 11);
}

int Jenco6311_PressCal(uint8_t address)									//COMMAND = 12
{
	return Jenco6311_ButtonCtrl(address, 12);
}

int Jenco6311_PressUp(uint8_t address)									//COMMAND = 13
{
	return Jenco6311_ButtonCtrl(address, 13);
}

int Jenco6311_PressDown(uint8_t address)								//COMMAND = 14
{
	return Jenco6311_ButtonCtrl(address, 14);
}

int Jenco6311_PressEnter(uint8_t address)								//COMMAND = 15
{
	return Jenco6311_ButtonCtrl(address, 15);
}

int Jenco6311_PressMode2s(uint8_t address)								//COMMAND = 16
{
	return Jenco6311_ButtonCtrl(address, 16);
}

int Jenco6311_PressWash(uint8_t address)								//COMMAND = 17
{
	return Jenco6311_ButtonCtrl(address, 17);
}

int Jenco6311_PressWash2S(uint8_t address)								//COMMAND = 18
{
	return Jenco6311_ButtonCtrl(address, 18);
}


void Jenco6311_CheckTest(uint8_t address)
{
	String LCDInfo_str 	= "";
	String temp_str1 	= "", temp_str2 	= "", temp_str3 	= "", temp_str4 	= "";
	String temp_str[8]	= {"","","","","","","",""};
	uint16_t Number_int16[8];
	int number_int[8];
	int x=13-2, y;
	int NumberOfRsp;

	Jenco6311_GetLCDInfo(address, &LCDInfo_str, &NumberOfRsp);

	temp_str1 	= LCDInfo_str[0];
	temp_str2 	= LCDInfo_str[1];
	temp_str3 	= LCDInfo_str[2]; 
	temp_str4 	= LCDInfo_str[3];

	for(y=0 ; y<8 ; y++){
		temp_str[y]	=	LCDInfo_str[y];
	 }

	for(y=0 ; y<8 ; y++){
		Number_int16[y]	=	LCDInfo_str[y+8];
	}
	for(y=0 ; y<8 ; y++){
		number_int[y]	=	Number_int16[y];
	}
	
	SendStrToHmi("main.value.txt=\"R:" + temp_str1 + temp_str2 + temp_str3 + temp_str4 + "\"");

	SendStrToHmi("debug.t5.txt=\"LCDInfo:" + temp_str[0] + temp_str[1] + temp_str[2] + temp_str[3] + "\"");
	delay(100);
	SendStrToHmi("debug.t6.txt=\"LCDInfo:" + temp_str[4] + temp_str[5] + temp_str[6] + temp_str[7] + "\"");
	delay(100);

	return number_int;
}