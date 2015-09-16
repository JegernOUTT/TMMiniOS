/********************************************************************/
/*    7188XB + X304 head file                                       */
/*                                                                  */
/*   [October/07/2005] by Liam                                      */
/*   [May,21,2002] by Kevin Version : 1.00                          */
/********************************************************************/
/********************************************************************/
/*  X304: 3 channels A/D (+/-5V)                                    */
/*        1 channels D/A (+/-5V)                                    */
/*        4 channels D/I                                            */
/*        4 channels D/O                                            */
/* [Caution]                                                        */
/*  The EEPROM block 7 on X board is used to store A/D & D/A        */
/*  calibration settings.                                           */
/*  When you use the  EEPROM on X board, don't overwrite it.        */
/********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#define X304_DigitalIn      X304_Read_All_DI
#define X304_DigitalOut     X304_Write_All_DO

int X304_Init(void);
// Return value: 0   ==> success
// Return value: <>0 ==> error
// Bit0: 1 ==> Reads A/D Gain falure
// Bit1: 1 ==> Reads A/D Offset falure
// Bit2: 1 ==> Reads D/A Gain falure
// Bit3: 1 ==> Reads D/A Offset falure  

float Read_AD_CalibrationGain(void);
//Return 10.0 when no setting in EEPROM

float Read_AD_CalibrationOffset(void);
//Return 10.0 when no setting in EEPROM

float Read_DA_CalibrationGain(void);
//Return 10.0 when no setting in EEPROM

float Read_DA_CalibrationOffset(void);
// Return 10.0 when no setting in EEPROM

float X304_AnalogIn(int iChannel);
// iChannel: 0~2

void X304_AnalogOut(int iChannel,float fValue);
// iChannel: 0
// fValue: -5.0 ~ 5.0

int X304_Read_All_DI(void);
// Return data = 0x00~0x0f
// Return 1 => open
//             Logic high level (+3.5V ~ +30V)
// Return 0 => close to GND
//             Logic low level (0V ~ +1V)

int X304_Read_One_DI(int iChannel);
// iChannel = 0 ~ 3
// Return 1 => open
//             Logic high level (+3.5V ~ +30V)
// Return 0 => close to GND
//             Logic low level (0V ~ +1V)

void X304_Write_All_DO(int iOutValue);
// iOutValue: 0x00 ~ 0x0f

void X304_Write_One_DO(int iChannel, int iStatus);
// iChannel = 0 ~ 3
// iStatus = 1 => Status is ON
// iStatus = 0 => Status is OFF

int X304_Read_All_DO(void);
// Return data = 0x00~0x0f

int X304_Read_One_DO(int iChannel);
// iChannel = 0 ~ 3
// Return  1 => ON
// Return  0 => OFF

extern float    fAD_Gain,fAD_Offset;
extern float    fDA_Gain,fDA_Offset;

#ifdef __cplusplus
     }
#endif
