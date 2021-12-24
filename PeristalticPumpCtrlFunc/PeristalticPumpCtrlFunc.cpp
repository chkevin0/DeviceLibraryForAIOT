/*
============================================
	Peristaltic Pump device library code，
	by 210312 KevinYW-Chen.
	
	Peristaltic Pump is used for low-precision behavior, such as: add water and drain.

	Vendor  : http://www.snfluid.cn/
	Github	: https://github.com/chkevin0/DeviceLibraryForAIOT

	Release	date : 2021-04-26

	ChangeLog:
		2021-04-28
		Add functions "Peristaltic_ReadSpeed" and "Peristaltic_ReadMaxSpeed".

	Remark:
		Must use Uart-485 converter
============================================
*/

#include "PeristalticPumpCtrlFunc.h"

#define HMI         Serial2
#define RS485    	Serial3 

void Peristaltic_PumpInit(void)
{
	RS485.begin     (9600, SERIAL_8N2);
}

/*
"Peristaltic_SetAddr":Pump device address can only be set individually
Address : 00 (Broadcast Address)
*/
void Peristaltic_SetAddr	(uint8_t SetAddress)
{
	uint8_t CMD[8]	= {00,06,00,03,00,SetAddress,00,00};
	int x;

	CMD[6] = Peristaltic_CaculateCRC(CMD,  6)%256;
  	CMD[7] = Peristaltic_CaculateCRC(CMD,  6)>>8;

  	//Releass CMD
	for(x=0 ; x < 8 ; x++){
	RS485.write(CMD[x]);
	}

	SendStrToHmi("main.value.txt=\"Setadd" + String(SetAddress) + "\"");
}

void Peristaltic_Launch(uint8_t address)
{
	uint8_t CMD[8]	= {address,06,00,00,00,01,00,00};
	int x;

	CMD[6] = Peristaltic_CaculateCRC(CMD,  6)%256;
  	CMD[7] = Peristaltic_CaculateCRC(CMD,  6)>>8;

  	//Releass CMD
	for(x=0 ; x < 8 ; x++){
	RS485.write(CMD[x]);
	}

	//SendStrToHmi("main.value.txt=\"Launch\"");

	return 0;
}

//Use "Delay" function
void Peristaltic_Launch_Duration(uint8_t address, float duration)
{	
	Peristaltic_Launch(address);

	delay(duration*1000);

	Peristaltic_Stop(address);

	return 0;
}

void Peristaltic_Stop(uint8_t address)
{
	uint8_t CMD[8]	= {address,06,00,00,00,00,00,00};
	int x;

	CMD[6] = Peristaltic_CaculateCRC(CMD,  6)%256;
  	CMD[7] = Peristaltic_CaculateCRC(CMD,  6)>>8;

  	//Releass CMD
	for(x=0 ; x < 8 ; x++){
	RS485.write(CMD[x]);
	}

	//SendStrToHmi("main.value.txt=\"Stop\"");

	return 0;
}

void Peristaltic_SetSpeed(uint8_t address, uint16_t PumpSpeed)
{
	uint8_t CMD[8]	= {address,06,00,02,00,00,00,00};
	int x;

	CMD[4] = PumpSpeed 				>> 8;
	CMD[5] = PumpSpeed  			% 256;
	CMD[6] = Peristaltic_CaculateCRC(CMD,  6)	% 256;
  	CMD[7] = Peristaltic_CaculateCRC(CMD,  6)	>> 8;

  	//Releass CMD
	for(x=0 ; x < 8 ; x++){
	RS485.write(CMD[x]);
	}

	SendStrToHmi("main.value.txt=\"S:" + String(PumpSpeed) + "\"");

	return 0;
}

void Peristaltic_SetForward	(uint8_t address)
{
	uint8_t CMD[8]	= {address,06,00,01,00,01,00,00};
	int x;

	Peristaltic_Stop(address);
	delay(500);

	CMD[6] = Peristaltic_CaculateCRC(CMD,  6)	% 256;
  	CMD[7] = Peristaltic_CaculateCRC(CMD,  6)	>> 8;

  	//Releass CMD
	for(x=0 ; x < 8 ; x++){
	RS485.write(CMD[x]);
	}

	delay(100);
	Peristaltic_Launch(address);

	//SendStrToHmi("main.value.txt=\"Forward\"");

	return 0;
}

void Peristaltic_SetReverse	(uint8_t address)
{
	uint8_t CMD[8]	= {address,06,00,01,00,00,00,00};
	int x;
	
	Peristaltic_Stop(address);
	delay(500);

	CMD[6] = Peristaltic_CaculateCRC(CMD,  6)	% 256;
  	CMD[7] = Peristaltic_CaculateCRC(CMD,  6)	>> 8;

  	//Releass CMD
	for(x=0 ; x < 8 ; x++){
	RS485.write(CMD[x]);
	}

	delay(100);
	Peristaltic_Launch(address);

	//SendStrToHmi("main.value.txt=\"Reverse\"");

	return 0;
}

int Peristaltic_ReadSpeed(uint8_t address)
{
	uint8_t CMD[8]			= {address,03,00,02,00,01,00,00};
	uint8_t ResponsesData[8] ;
	String 	RspMessage_str	=	"";
	int 	NumberOfRsp		=	0;
	int 	x;
	uint16_t NumberA_int16, NumberB_int16;
	uint8_t NumberC_int8, NumberD_int8;
	int 	result;
	char 	RspTmp_char;
	
	CMD[6] = Peristaltic_CaculateCRC(CMD,  6)%256;
  	CMD[7] = Peristaltic_CaculateCRC(CMD,  6)>>8;

  		
  	while(RS485.available()>0) RS485.read();			//Clear buffer of RS485 Read-port
	//Releass CMD
	for(x=0 ; x < 8 ; x++){
	RS485.write(CMD[x]);
	}

	delay(100);

	RspMessage_str = "";
	while (RS485.available() > 0){
		RspTmp_char 	   	= RS485.read();
		RspMessage_str 		+= RspTmp_char;	
	}
	NumberOfRsp		   	= RspMessage_str.length();

	//RspMessage_str : String type converte to uint16_t type
  	NumberA_int16 = RspMessage_str[4];
  	NumberB_int16 = RspMessage_str[5];
  	result   = (NumberA_int16 << 8) + NumberB_int16;

  	for(x=0 ; x < 8 ; x++)	ResponsesData[x] = RspMessage_str[x];

  	NumberC_int8 = (Peristaltic_CaculateCRC(ResponsesData, 6)%256);
  	NumberD_int8 = (Peristaltic_CaculateCRC(ResponsesData, 6)>>8);

  	// if( (NumberC_int8 == ResponsesData[6]) && (NumberD_int8 == ResponsesData[7]) )
	//  	SendStrToHmi("main.value.txt=\"R" + String(result) + "\"");
  	// else SendStrToHmi("main.value.txt=\"Error\"");

	return result;
}

int Peristaltic_ReadMaxSpeed(uint8_t address)
{
	uint8_t CMD[8]			= {address,03,00,06,00,01,00,00};
	uint8_t ResponsesData[8];
	String 	RspMessage_str	=	"";
	int 	NumberOfRsp		=	0;
	int 	x;
	uint16_t NumberA_int16, NumberB_int16;
	uint8_t NumberC_int8, NumberD_int8;
	int 	result;
	char 	RspTmp_char;
	
	CMD[6] = Peristaltic_CaculateCRC(CMD,  6)%256;
  	CMD[7] = Peristaltic_CaculateCRC(CMD,  6)>>8;

  	// SendStrToHmi("main.value.txt=\"RdMax\"");
  		
  	while(RS485.available()>0) RS485.read();			//Clear RS485 read buff
	//Releass CMD
	for(x=0 ; x < 8 ; x++){
	RS485.write(CMD[x]);
	}

	delay(100);

	RspMessage_str = "";
	while (RS485.available() > 0){
		RspTmp_char 	   	= RS485.read();
		RspMessage_str 		+= RspTmp_char;	
	}
	NumberOfRsp		   	= RspMessage_str.length();

	//RspMessage_str : String type converte to uint16_t type
  	NumberA_int16 = RspMessage_str[4];
  	NumberB_int16 = RspMessage_str[5];
  	result   = (NumberA_int16 << 8) + NumberB_int16;

  	for(x=0 ; x < 8 ; x++)	ResponsesData[x] = RspMessage_str[x];

  	NumberC_int8 = (Peristaltic_CaculateCRC(ResponsesData, 6)%256);
  	NumberD_int8 = (Peristaltic_CaculateCRC(ResponsesData, 6)>>8);

  	// if( (NumberC_int8 == ResponsesData[6]) && (NumberD_int8 == ResponsesData[7]) )
	//  	SendStrToHmi("main.value.txt=\"M" + String(result) + "\"");
  	// else SendStrToHmi("main.value.txt=\"Error\"");

	return result;
}


uint16_t Peristaltic_CaculateCRC(uint8_t* Data, uint16_t len)
{
	uint16_t n,i;
	uint16_t crc=0xffff;
	for (n=0;n<len;n++){
		crc=crc^Data[n];
		for (i=0;i<8;i++){
			if((crc&1)==1){
				crc=crc>>1;
				crc=crc^0xA001;
			}
			else{
				crc=crc>>1;
			}
		}
	}
	
	return(crc);
}


