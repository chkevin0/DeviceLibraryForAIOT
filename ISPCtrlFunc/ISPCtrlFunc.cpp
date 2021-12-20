/*
============================================
	Industry Syringe Pump (MSP1-D1) device library code
	by 210312 KevinYW-Chen

	Vendor  : https://www.longerpump.com/
	Github	: https://github.com/chkevin0/DeviceLibraryForAIOT

	Release	date : 2021-03-26
	ChangeLog:
		2021-04-13
		Optimized "ConfCorOfRspMsg(String)" function，priority to confirm ETX = 03h.

		2021-06-16
		Add functions "ISP_CheckStatus"、"ISP_CheckIdle" and "ISP_CheckIdle_TimeOut".

		2021-07-27
		Optimize the function of ISP_Init_SpeedSet，decompose the function into 
		"ISP_Initialization" and "ISP_SpeedSet".

============================================
*/

#include "ISPCtrlFunc.h"
#define RS485    Serial3
#define Idle 	1
#define Busy 	0   

void ISP_SendRawCMD(uint8_t address_number, String RawCMD)
{
	char 	address;
	char 	data[100];
 	char 	checksum=0;
 	int 	x=0;
 	int 	NumberOfRawCMD;

 	address = address_number + 0x30;
 	NumberOfRawCMD				=	RawCMD.length();

	data[0]						=	0x02;				//STX 02h
	data[1]						=	address+0x01;		//Pump address
	data[2]						=	0x31;				//Sequence Nnmber 0x31
	 
	//Raw CMD to Data array 
	for(x=0 ; x < NumberOfRawCMD ; x++){
	data[3+x]	=	RawCMD[x];							//Raw CMD Data Block
	}

	data[3+NumberOfRawCMD]		=	'R';				//Data Block 'R':Excute command (Important)	
	data[4+NumberOfRawCMD]		=	0x03;				//ETX 03h
	
	//Produce the checkcum
	for(x=0 ; x < 5+NumberOfRawCMD ; x++){
 	checksum ^=	data[x];
 	}
 	data[5+NumberOfRawCMD]		=	checksum;			//checksum
 	
 	//Releass CMD
 	for(x=0 ; x < 6+NumberOfRawCMD ; x++){
 	RS485.write(data[x]);
 	}
	
	delay(250);
	return 0;
}

void ISP_Terminate(uint8_t address_number)
{
	char 	address = address_number + 0x30;
	char 	data[10];
 	char 	checksum=0;
 	int 	x=0;

 	data[0]		=	0x02;				//STX 02h
 	data[1]		=	address+0x01;		//Pump address
 	data[2]		=	0x31;				//Sequence Nnmber 0x31
 	data[3]		=	'T';				//Data Block : Terminate all command
 	data[4]		=	'R';				//Data Block 'R':Excute command (Important)	
 	data[5]		=	0x03;				//ETX 03h
 	for(x=0 ; x < 6 ; x++){
 		checksum ^=	data[x];
 	}
 	data[6]		=	checksum;			//checksum

 	//Releass CMD
 	for(x=0 ; x < 7 ; x++){
 		RS485.write(data[x]);
 	}
	return 0;
}

void ISP_ValveCtrl_Input(uint8_t address_number)
{
	char 	address = address_number + 0x30;
 	char 	data[10];
 	char 	checksum=0;
 	int 	x=0, i=0;

 	data[0]		=	0x02;				//STX 02h
 	data[1]		=	address+0x01;		//Pump address
 	data[2]		=	0x31;				//Sequence Nnmber 0x31
 	data[3]		=	'I';				//Data Block
 	data[4]		=	'R';				//Data Block 'R':Excute command (Important)	
 	data[5]		=	0x03;				//ETX 03h
 	for(x=0 ; x < 6 ; x++){
 		checksum ^=	data[x];
 	}
 	data[6]		=	checksum;			//checksum

 	//Releass CMD
 	for(i=0 ; i<2 ; i++){
	 	for(x=0 ; x < 7 ; x++){
	 		RS485.write(data[x]);
	 	}
	delay(500);
	ISP_CheckIdle(data[1]);
 	}
 	return 0;
}

void ISP_ValveCtrl_Output(uint8_t address_number)
{
	char 	address = address_number + 0x30;
 	char 	data[10];
 	char 	checksum=0;
 	int 	x=0, i=0;

 	data[0]		=	0x02;				//STX 02h
 	data[1]		=	address+0x01;		//Pump address
 	data[2]		=	0x31;				//Sequence Nnmber 0x31
 	data[3]		=	'O';				//Data Block
 	data[4]		=	'R';				//Data Block 'R':Excute command (Important)	
 	data[5]		=	0x03;				//ETX 03h
 	for(x=0 ; x < 6 ; x++){
 		checksum ^=	data[x];
 	}
 	data[6]		=	checksum;			//checksum


 	//Releass CMD
 	for(i=0 ; i<2 ; i++){
	 	for(x=0 ; x < 7 ; x++){
	 		RS485.write(data[x]);
	 	}
	delay(500);
	ISP_CheckIdle(data[1]);
 	}
 	return 0;
}

void ISP_MovePlunger_AbsPos(uint8_t address_number, int position_int)
{
	char 	address = address_number + 0x30;
	char 	data[20];
 	char 	checksum=0;
 	int 	x=0;
 	int 	NumberOfPositionChar;
 	String	position =  String(position_int);

 	NumberOfPositionChar	=	position.length();		//Arduino Dedicated Function : Count numbers of char in String

	data[0]					=	0x02;					//STX 02h
	data[1]					=	address+0x01;			//Pump address
	data[2]					=	0x31;					//Sequence Nnmber 0x31
	data[3]					=	'A';					//Data Block
	 
	//Abs.Position info to Data array 
	for(x=0 ; x < NumberOfPositionChar ; x++){
	data[4+x]	=	position[x];						//Position info Data Block
	}
	data[4+NumberOfPositionChar]		=	'R';		//Data Block 'R':Excute command (Important)	
	data[5+NumberOfPositionChar]		=	0x03;		//ETX 03h
	
	//Produce the checkcum
	for(x=0 ; x < 6+NumberOfPositionChar ; x++){
 	checksum ^=	data[x];
 	}
 	data[6+NumberOfPositionChar]		=	checksum;	//checksum
 	
 	//Releass CMD
 	for(x=0 ; x < 7+NumberOfPositionChar ; x++){
 	RS485.write(data[x]);
 	}
	
	return 0;
}

void ISP_MovePlunger_AbsPosRecPos(uint8_t address_number, int position_int, int* RecordPosition_ptr)
{
	char 	address = address_number + 0x30;
	char 	data[20];
 	char 	checksum=0;
 	int 	x=0;
 	int 	NumberOfPositionChar;
 	String 	position = String(position_int);


 	//*RecordPosition_ptr		=	position.toInt();		//Arduino Dedicated Function : String type to int type
	
	//test 					=	position.toInt();
	//*RecordPosition_ptr		=	test;
	*RecordPosition_ptr		=	position_int;	
 	NumberOfPositionChar	=	position.length();		//Arduino Dedicated Function : Count numbers of char in String

	data[0]					=	0x02;					//STX 02h
	data[1]					=	address+0x01;			//Pump address
	data[2]					=	0x31;					//Sequence Nnmber 0x31
	data[3]					=	'A';					//Data Block
	 
	//Abs.Position info to Data array 
	for(x=0 ; x < NumberOfPositionChar ; x++){
	data[4+x]	=	position[x];						//Position info Data Block
	}
	data[4+NumberOfPositionChar]		=	'R';		//Data Block 'R':Excute command (Important)	
	data[5+NumberOfPositionChar]		=	0x03;		//ETX 03h
	
	//Produce the checkcum
	for(x=0 ; x < 6+NumberOfPositionChar ; x++){
 	checksum ^=	data[x];
 	}
 	data[6+NumberOfPositionChar]		=	checksum;	//checksum
 	
 	//Releass CMD
 	for(x=0 ; x < 7+NumberOfPositionChar ; x++){
 	RS485.write(data[x]);
 	}
	
	return 0;
}

void MovePlungerAbsPos_test(uint8_t address_number, String position)
{
	char 	address = address_number + 0x30;
	char 	data[20];
 	char 	checksum=0;
 	int 	x=0;
 	
	//if(position>10000)return 0;
	//else{
		data[0]		=	0x02;			//STX 02h
	 	data[1]		=	address+0x01;	//Pump address
	 	data[2]		=	0x31;			//Sequence Nnmber 0x31
	 	data[3]		=	'A';			//Data Block
	 	data[4]		=	'0';			//Data Block
	 	data[5]		=	'0';			//Data Block
	 	data[6]		=	'9';			//Data Block
	 	data[7]		=	'R';			//Data Block 'R':Excute command (Important)		
	 	data[8]		=	0x03;			//ETX 03h
	//Produce the checkcum
	 	for(x=0 ; x < 9 ; x++){
 		checksum ^=	data[x];
 		}
 		data[9]		=	checksum;	//checksum
 		//Releass CMD
 		for(x=0 ; x < 10 ; x++){
 		RS485.write(data[x]);
 		}
	//}
	return 0;
}
   
void ISP_MovePlunger_RelDisplUp(uint8_t address_number, int RelDisplacement_int, int threshold, int *RecordPosition_ptr)
{
	char 	address = address_number + 0x30;
	char 	data[20];
 	char 	checksum=0;
 	int 	x=0;
 	int 	NumberOfRelDisplacementChar;
 	String 	RelDisplacement = String(RelDisplacement_int);

 	if((*RecordPosition_ptr) - RelDisplacement_int < threshold )	return 0;
 	else{
	 	*RecordPosition_ptr		=	*RecordPosition_ptr - RelDisplacement_int;

	 	NumberOfRelDisplacementChar	=	RelDisplacement.length();		

		data[0]					=	0x02;					//STX 02h
		data[1]					=	address+0x01;			//Pump address
		data[2]					=	0x31;					//Sequence Nnmber 0x31
		data[3]					=	'D';					//Data Block
		 
		//Abs.Position info to Data array 
		for(x=0 ; x < NumberOfRelDisplacementChar ; x++){
		data[4+x]	=	RelDisplacement[x];						//Position info Data Block
		}
		data[4+NumberOfRelDisplacementChar]		=	'R';		//Data Block 'R':Excute command (Important)	
		data[5+NumberOfRelDisplacementChar]		=	0x03;		//ETX 03h
		
		//Produce the checkcum
		for(x=0 ; x < 6+NumberOfRelDisplacementChar ; x++){
	 	checksum ^=	data[x];
	 	}
	 	data[6+NumberOfRelDisplacementChar]		=	checksum;	//checksum
	 	
	 	//Releass CMD
	 	for(x=0 ; x < 7+NumberOfRelDisplacementChar ; x++){
	 	RS485.write(data[x]);
	 	}
		
	}
	return 0;
}

void ISP_MovePlunger_RelDisplDown(uint8_t address_number, int RelDisplacement_int, int threshold, int *RecordPosition_ptr)
{
	char 	address = address_number + 0x30;
	char 	data[20];
 	char 	checksum=0;
 	int 	x=0;
 	int 	NumberOfRelDisplacementChar;
 	String 	RelDisplacement = String(RelDisplacement_int);
 	

 	if((*RecordPosition_ptr) + RelDisplacement_int > threshold )	return 0;
 	else{
	 	*RecordPosition_ptr		=	*RecordPosition_ptr + RelDisplacement_int;
	 	NumberOfRelDisplacementChar	=	RelDisplacement.length();		

		data[0]					=	0x02;					//STX 02h
		data[1]					=	address+0x01;			//Pump address
		data[2]					=	0x31;					//Sequence Nnmber 0x31
		data[3]					=	'P';					//Data Block
		 
		//Abs.Position info to Data array 
		for(x=0 ; x < NumberOfRelDisplacementChar ; x++){
		data[4+x]	=	RelDisplacement[x];						//Position info Data Block
		}
		data[4+NumberOfRelDisplacementChar]		=	'R';		//Data Block 'R':Excute command (Important)	
		data[5+NumberOfRelDisplacementChar]		=	0x03;		//ETX 03h
		
		//Produce the checkcum
		for(x=0 ; x < 6+NumberOfRelDisplacementChar ; x++){
	 	checksum ^=	data[x];
	 	}
	 	data[6+NumberOfRelDisplacementChar]		=	checksum;	//checksum
	 	
	 	//Releass CMD
	 	for(x=0 ; x < 7+NumberOfRelDisplacementChar ; x++){
	 	RS485.write(data[x]);
	 	}
	}
	return 0;
}

int ConfCorOfRspMsg(String RspMessage)	//Confirm Correctness Of Responses Message
{
	int 	x;
	int 	NumberOfRsp		=	0;
	char 	checksum 		=	0;
	bool	RspCompleteness	=	0;

  	//Confirm Completeness of Responses Message Data  
  	NumberOfRsp		=	RspMessage.length();
  	if(RspMessage[NumberOfRsp-2] == 0x03)	RspCompleteness =	1;
  	else{
  		RspCompleteness	=	0;
  		return 0;
  	}	
  	
  	//Confirm Correctness of Checksum
  	if(RspCompleteness ==	1){
  		for(x=0 ; x < NumberOfRsp-1 ; x++){
 		checksum ^=	RspMessage[x];
 		}
 		if(RspMessage[NumberOfRsp-1]	==	checksum)	return 1;
 		else {
  			return 0;
  		}	
  	}
 	

  	return 0;
}

bool ISP_CheckStatus(uint8_t address_number, uint8_t* ErrorCode)
{
	String 	RspMessage_str			=	"";
	unsigned long 	DelayTime		=	250;
	int 	NumberOfRsp				=	0;
	String 	CMD 					=	"Q";
	char 	RspTmp_char				=	"";
	char 	status 					=	0;			//,ErrorCode		= 0;
	uint8_t	TimeOut_times			=	0;


	while(RS485.available()>0) RS485.read();		// clear buffer of RS485 port
		
	ISP_SendRawCMD(address_number, CMD);
	delay(500);

	RspMessage_str = "";
	while (RS485.available() > 0){					// Receive Message
		RspTmp_char 	   	= RS485.read();
		RspMessage_str     += RspTmp_char;	
	}
	NumberOfRsp = RspMessage_str.length();
	
	if(ConfCorOfRspMsg(RspMessage_str) == 0){		// 0:RspMessage ERROR
		*ErrorCode = 0xff;
		HMI_ChangePco("syringe_pump", "status", "RED");
		HMI_ShowDataToTXT("syringe_pump.status", String(*ErrorCode) );
		return 0;
	}	


	*ErrorCode 	=	 RspMessage_str[2];
	status 		=	((RspMessage_str[2]	>>	5) == 3) ? 1 : 0 ;		// 0:Busy	/ 1:Idle

	if(*ErrorCode == 96){
    	HMI_ChangePco("syringe_pump", "status", "806");
    	HMI_ShowDataToTXT("syringe_pump.status", "Idle");
    }
    else{
    	HMI_ChangePco("syringe_pump", "status", "RED");
    	HMI_ShowDataToTXT("syringe_pump.status", String(*ErrorCode) );
    }
	return status;
}

bool ISP_CheckIdle(uint8_t address_number)							// 0:Busy	/ 1:Idle
{
	uint8_t 	ErrorCode;
	
	delay(150);
    if(ISP_CheckStatus(address_number, &ErrorCode) ==  Idle){
    	return 1;
    }
    else return 0;
}

bool ISP_CheckIdle_TimeOut(uint8_t address_number, uint16_t TimeOut_Sec)		// 0:Busy	/ 1:Idle
{
	uint16_t TimeOut_times = 0;

	while(1){
		if ( ISP_CheckIdle(address_number) )	return 1;
		if(TimeOut_times > TimeOut_Sec)			return 0;
		TimeOut_times++;
		delay(1000);
	}
}

void ISP_FlushPump(uint8_t address_number, int frequency_int)
{
	int 	x;

	for(x=0 ; x < frequency_int ; x++){
		ISP_SendRawCMD(address_number,"IA1000");
		ISP_CheckIdle(address_number);

		ISP_SendRawCMD(address_number,"OA0");
		ISP_CheckIdle(address_number);
	}

	return 0;
}

void ISP_Initialization(uint8_t address_number)
{
	String RawCMD;

	RawCMD = "Y20";

	ISP_SendRawCMD(address_number, RawCMD);
}

void ISP_SpeedSet(uint8_t address_number, int SpeedRange_int)
{
	String RawCMD;

	RawCMD = "S" + String(SpeedRange_int);

	ISP_SendRawCMD(address_number, RawCMD);
}

void ISP_Init_SpeedSet(uint8_t address_number, int SpeedRange_int)
{
	char 	address = address_number + 0x30;
	char 	data[20];
 	char 	checksum=0;
 	int 	x=0;
 	int 	NumberOfSpeedRange;
 	String  SpeedRange = String(SpeedRange_int);

 	// data[20]	=	{0x02,address,0x31,'Y','2','0','I','S'};
 	NumberOfSpeedRange	=	SpeedRange.length();

	data[0]		=	0x02;					//STX 02h
	data[1]		=	address+0x01;			//Pump address
	data[2]		=	0x31;					//Sequence Nnmber 0x31
	data[3]		=	'Y';					//Data Block
	data[4]		=	'2';					//Data Block
	data[5]		=	'0';					//Data Block
	data[6]		=	'I';					//Data Block
	data[7]		=	'S';					//Data Block
	 
	//Abs.Position info to Data array 
	for(x=0 ; x < NumberOfSpeedRange ; x++){		//ISP Speed 
	data[8+x]	=	SpeedRange[x];					
	}
	data[8+NumberOfSpeedRange]		=	'R';		//Data Block 'R':Excute command (Important)	
	data[9+NumberOfSpeedRange]		=	0x03;		//ETX 03h
	
	//Produce the checkcum
	for(x=0 ; x < 10+NumberOfSpeedRange ; x++){
 	checksum ^=	data[x];
 	}
 	data[10+NumberOfSpeedRange]		=	checksum;	//checksum
 	
 	//Releass CMD
 	for(x=0 ; x < 11+NumberOfSpeedRange ; x++){
 	RS485.write(data[x]);
 	}
	
	return 0;
}



