/*
============================================
	Raspberry Pi Zero W (RPI) device library code，
	by 210312 KevinYW-Chen.
	
	Raspberry Pi needs to install an external antenna，because the metal case will cause signal shielding.

	Vendor  : https://www.raspberrypi.com/products/raspberry-pi-zero-w/
	Github	: https://github.com/chkevin0/DeviceLibraryForAIOT

	Release	date : 2021-04-14
	ChangeLog:
		2021-04-29
		Add the functions "RPI_GetIP"，when the RPI is booted, it will automatically connect and get an IP.

		2021-07-23
		Add the functions "RPI_GetTime" and "RPI_CreatTXTNameByTime", creat a LOG TXT based on the timestamp.

		2021-08-03
		Add the functions "RPI_CreatpHCalTXTNameByTime", creat a LOG TXT for pH electrode calibration based on the time stamp.
	
	Remark:
		Communication format : STX + Data_length + Data + ETX
============================================
*/

#include "RPICtrlFunc.h"
#define RPI Serial1


//-------------------------------------------------------------
//                          Send
//-------------------------------------------------------------
void RPI_SendFuncDataToRPI(byte function,String data)
{
	int i;
	int length;
	length = (data.length()) + 1;

	RPI.write(0x02);
	RPI.write(length);
	if(data=="")RPI.write(function);
	else{
		RPI.write(function);
	    for (i = 0; i < data.length(); i++) RPI.write(data[i]);
	}
	RPI.write(0x03);

	delay(200);

	return 0;
}

void RPI_SendFuncDataToRPI(byte function)
{
	String data = "";
	RPI_SendFuncDataToRPI(function,data);

	delay(150);

	return 0;
}

void RPI_WriteLog(String log)
{
	byte		function = 0x32;

	RPI_SendFuncDataToRPI(function, (log) );			//Release function	

	return 0;
}

void RPI_Write_ErrorLog(String log)
{
	byte		function = 0xE0;

	RPI_SendFuncDataToRPI(function, (log) );			//Release function	

	return 0;
}

void RPI_Write_ErrorLog_NoDataLength(String data)
{
	byte		function = 0xE0;
	int 		i;
	
	RPI.write(0x02);
	RPI.write(function);
	for (i = 0; i < data.length(); i++) RPI.write(data[i]);
	RPI.write(0x03);

	return 0;
}

void RPI_SendResultToRPI_Platform(double Result_double, int select)		//select : 1~9
{
	byte	function = 0x33;
	String 	data = "";

	data = String( char(select+0x30) + String(Result_double) );	

	RPI_SendFuncDataToRPI(function, data);
}

//-------------------------------------------------------------
//                          Get
//-------------------------------------------------------------
int RPI_GetCMDMessage	(String* CMDMessage)
{
	String 		RspMessage_str = "", CMDMessage_str="";
	char 		RspTmp_char;
	int			NumberOfRsp = 0;
	int			i=0;

	RspMessage_str = "";							//Get Complete Message
	while(RPI.available()>0){
		RspTmp_char = RPI.read();
		if(RspTmp_char == 0x02){
			RspMessage_str += RspTmp_char;
			while(1){
				RspTmp_char = RPI.read();
				if(RspTmp_char == 0x03){
					RspMessage_str += RspTmp_char;
					break;
				}
				else if(RPI.available() == 0){
					break;
				}
				else{
					RspMessage_str += RspTmp_char;
				}
			}
			break;
		}
			
	}
	NumberOfRsp = RspMessage_str.length();
	
	//while(RPI.available()>0) 	RPI.read();			//Clear buffer of RPI Read-port

	if(RspMessage_str[0] == 0x02 && RspMessage_str[NumberOfRsp-1] == 0x03){
		/*for( i=0 ; i++ ; i<NumberOfRsp-2 ){
			CMDMessage_str[i] =  RspMessage_str[i+1];
		}*/
		*CMDMessage = RspMessage_str;
		return 1;
	}
	return 0;
}


void RPI_GetIP(String* IP)
{
	char 		RspTmp_char;
	String 		RspMessage_str = "", IP_temp	= "";
	byte		function = 0x30;
	int 		NumberOfRsp = 0;
	int 		i;

	//delay(300);
	while(RPI.available()>0) RPI.read();			//Clear buffer of RPI Read-port
	
	RPI_SendFuncDataToRPI(function);				//Release function 
	delay(1500);

					
	while(RPI.available()>0){						//Get Complete Message
		RspTmp_char = RPI.read();
		RspMessage_str += RspTmp_char;
	}
	NumberOfRsp = RspMessage_str.length();
	
	if(RspMessage_str[0] == 0x02 && RspMessage_str[1] == 0x33 && RspMessage_str[2] == 0x30 ){
		for(i=0 ; i<NumberOfRsp-3-1 ; i++){
			IP_temp = IP_temp + RspMessage_str[3+i];
		}
	}
	else if (RspMessage_str == ""){
		IP_temp = "RPI Booting";
	}
	else{
		IP_temp = "IP Error";
	}

	*IP = IP_temp;									//IP_temp to IP
	//*IP = RspMessage_str;
	
	return 0;
}

void RPI_GetTime(String* time)
{
	char 		RspTmp_char;
	String 		RspMessage_str = "";
	String		time_str="";
	byte		function = 0xA1;
	int 		NumberOfRsp = 0;
	int 		i;

	while(RPI.available()>0) RPI.read();			//Clear buffer of RPI Read-port

	RPI_SendFuncDataToRPI(function);				//Release function
	delay(100);

	RspMessage_str = "";							//Get Complete Message
	while(RPI.available()>0){
		RspTmp_char = RPI.read();
		RspMessage_str += RspTmp_char;
	}
	NumberOfRsp = RspMessage_str.length();

	if(RspMessage_str[0]==0x02 && RspMessage_str[1]==0x33 && RspMessage_str[2]==0x31){
		for( i=0 ; i<NumberOfRsp-4 ; i++){
			RspTmp_char = RspMessage_str[3+i];
			time_str += RspTmp_char;
		}
		*time = time_str;

	}
	else *time = "RPI";

	//*time = RspMessage_str;

	return 0;
}

void RPI_CreatTXTNameByTime(String* time)
{
	char 		RspTmp_char;
	String 		RspMessage_str = "";
	byte		function = 0x31;					
	int 		NumberOfRsp = 0;
	int 		i;

	while(RPI.available()>0) RPI.read();			//Clear buffer of RPI Read-port

	RPI_SendFuncDataToRPI(function);				//Release function
	delay(100);

	RspMessage_str = "";							//Get Complete Message
	while(RPI.available()>0){
		RspTmp_char = RPI.read();
		RspMessage_str += RspTmp_char;
	}
	NumberOfRsp = RspMessage_str.length();

	*time = RspMessage_str;

	return 0;
}

void RPI_CreatpHCalTXTNameByTime(String* time)
{
	char 		RspTmp_char;
	String 		RspMessage_str = "";
	byte		function = 0xA3;					
	int 		NumberOfRsp = 0;
	int 		i;

	while(RPI.available()>0) RPI.read();			//Clear buffer of RPI Read-port

	RPI_SendFuncDataToRPI(function);				//Release function
	delay(100);

	RspMessage_str = "";							//Get Complete Message
	while(RPI.available()>0){
		RspTmp_char = RPI.read();
		RspMessage_str += RspTmp_char;
	}
	NumberOfRsp = RspMessage_str.length();

	*time = RspMessage_str;

	return 0;
}

//---------------------Reference------------------------------------
void UU_RPI_GetIP(String* IP)
{
	byte function = 0x30;
	String CMD_data="";

	RPI.print(char(0x02));
	RPI.println(char(function));
  
  	delay(100);

  	char temp=' ';
	while (Serial1.available()>0) {
	    temp= Serial1.read();
	    if(temp==0x02)  CMD_data="";
	    CMD_data+=temp;
	}


  	*IP = CMD_data;
  	return 0;
}