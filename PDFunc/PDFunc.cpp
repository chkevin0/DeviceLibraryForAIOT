/*
============================================
	Photodiode(PD) device library code
	by 210312 KevinYW-Chen.

	Vendor  : 
	Github	: https://github.com/chkevin0/DeviceLibraryForAIOT

	Release	date : 2021-06-11
	
	Remark:
		1.Need to be used with ADS1115 to improve the resolution of ADC.
		2.The laser or LED light source is adjusted with TSL2591 to adjust the light intensity.
		3.TSL2591 : Voltage regulator chip
============================================
*/

#include "PDFunc.h"


#define HMI      Serial2
#define RS485    Serial3 

//-------------PGA----------------------------------
#define ADS1115_PGA_6P144           0x00
#define ADS1115_PGA_4P096           0x01
#define ADS1115_PGA_2P048           0x02 // default
#define ADS1115_PGA_1P024           0x03
#define ADS1115_PGA_0P512           0x04
#define ADS1115_PGA_0P256           0x05
#define ADS1115_PGA_0P256B          0x06
#define ADS1115_PGA_0P256C          0x07

//-------------Rate----------------------------------

#define ADS1115_RATE_8              0x00
#define ADS1115_RATE_16             0x01
#define ADS1115_RATE_32             0x02
#define ADS1115_RATE_64             0x03
#define ADS1115_RATE_128            0x04 // default
#define ADS1115_RATE_250            0x05
#define ADS1115_RATE_475            0x06
#define ADS1115_RATE_860            0x07

ADS1115 PD_ADS1115(ADS1115_DEFAULT_ADDRESS); 												//很重要

void PD_ADS1115_Initialize(uint8_t PD_ADS1115_Resolution, uint8_t PD_ADS1115_SamplingRate)
{
	//String PD_Resolution = PD_ADS1115_Resolution;
	//String PD_SamplingRate = PD_ADS1115_SamplingRate;

	//String PD_Resolution = "ADS1115_PGA_1P024";
	//String PD_SamplingRate = "ADS1115_RATE_16";

	uint8_t PD_Resolution = PD_ADS1115_Resolution;
	uint8_t PD_SamplingRate = PD_ADS1115_SamplingRate;

	Wire.begin();
  	PD_ADS1115.initialize();
  	PD_ADS1115.setGain(PD_Resolution);
  	PD_ADS1115.setRate(PD_SamplingRate);

  	//PD_ADS1115.setGain(ADS1115_PGA_1P024);
  	//PD_ADS1115.setRate(ADS1115_RATE_16);

}

bool PD_ADS1115_GetLightIntensity(double* Light_Intensity)
{
	double Light_Intensity_avg=0;
	double Maximum_effective_voltage_mV=200;
	int i;

  	for ( i=0 ; i<5 ; i++){ 
  		Light_Intensity_avg += PD_ADS1115.getConversionP0GND();
  	}
  	Light_Intensity_avg	= Light_Intensity_avg/5;
  	*Light_Intensity 	= Light_Intensity_avg;
  	
  	//判斷光強是否過高，0:過高   /   1:正常
  	if(Light_Intensity_avg*PD_ADS1115.getMvPerCount() >= Maximum_effective_voltage_mV){
  		return 0;
  	}
  	return 1;
}

int PD_ADS1115_GetSampleIntensityANDAbsorbance(double Standard_Intensity, double* Sample_Intensity, double* Sample_Absorbance)
{
	double Sample_Intensity_avg=1;
	double Sample_Absorbance_temp;

	//判斷Sample的光強在有效範圍
	if( PD_ADS1115_GetLightIntensity( &Sample_Intensity_avg ) ){
		*Sample_Intensity = Sample_Intensity_avg;

		//判斷Sample的光強過低
		if(Sample_Intensity_avg<1){
			*Sample_Intensity = -1;
			*Sample_Absorbance = -1;
			return -1;
		}
	}
		//判斷Sample的光強過高
	else {
		*Sample_Intensity = 9999;
		*Sample_Absorbance = 9999;
		return 0;
	}

	//運算吸收值
	Sample_Absorbance_temp = log10(Standard_Intensity / Sample_Intensity_avg)*16384;	
	*Sample_Absorbance = Sample_Absorbance_temp;

	return 1;
}