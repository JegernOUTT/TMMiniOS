#include "Model.h"

#ifdef MVS_2012_32BIT 

using namespace std;

extern void UserInit();
extern void UserLoopFun();

int tmpErr = 0;
int tmpArr[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
unsigned int DOValues = 0;

int ModbusRTU_Master(int com, int modbusAddress, int functionCode, int to, int from, int howMuch, int timeout, int mode)
{
	SlaveFactory & slave = SlaveFactory::getSlaveFactoryPointer();
	SlaveInformation inf(DISCRETE_INPUTS, to + 1);
	uValue val;
	if (tmpErr == 0)
	{
		tmpErr = 502;
		tmpArr[0] = com;
		tmpArr[1] = modbusAddress;
		tmpArr[2] = functionCode;
		tmpArr[3] = to;
		tmpArr[4] = from;
		tmpArr[5] = howMuch;
		tmpArr[6] = timeout;
		tmpArr[7] = mode;
	}
	else if ((tmpErr == 502) && (
		(tmpArr[0] == com) || (tmpArr[1] == modbusAddress) || (tmpArr[2] == functionCode) 
		|| (tmpArr[3] == to) || (tmpArr[4] == from) || (tmpArr[5] == howMuch)
		|| (tmpArr[6] == timeout) || (tmpArr[7] == mode)
		))
	{
		tmpErr = 0;
	}
	else
	{
		tmpErr = 503;
	}

	switch (functionCode)
	{
	case 2:
		--inf.registerNumber;
		val.bVal[0] = true_t;
		val.bVal[1] = false_t;
		val.bVal[2] = false_t;
		val.bVal[3] = false_t;

		slave.setDisreteInput(inf, val);
		break;
	case 3:
		--inf.registerNumber;
		val.iVal[0] = 15;
		slave.setHoldingRegisters(inf, val);
		break;
	case 4:
		--inf.registerNumber;
		val.iVal[0] = 5;
		slave.setInputRegisters(inf, val);

		++inf.registerNumber;
		val.iVal[0] = 160;
		slave.setInputRegisters(inf, val);

		++inf.registerNumber;
		val.iVal[0] = 25002;
		slave.setInputRegisters(inf, val);
		
		break;
	default:
		break;
	}

	inf.registerNumber = to + 1;
	switch (modbusAddress)
	{
	case  10:
		--inf.registerNumber;
		val.iVal[0] = 1547;
		slave.setInputRegisters(inf, val);
		break;
	}

	return tmpErr;
}

float X304_AnalogIn(int iCh)
{
	return 0.5f + iCh;
};

unsigned short X304_DigitalIn()
{
	return 0xF;
};

__int64 GetTimeTicks(void)
{
	time_t time_now;
	time(&time_now);

	return static_cast<__int64>(time_now);
}

unsigned short X304_Init()
{
	return 0;
}

void X304_Write_All_DO(unsigned int val)
{
	DOValues = val;
}

unsigned short X304_Read_All_DO()
{
	return DOValues;
}

int main()
{
	/*
	!	TODO:
	!		-Добавить подсчет дискретных импульсов по маске extra1
	!		-Реализовать архивирование через определенное время на плату X600
	!		-Подумать про прерывания
	*/
	UserInit();
	while (true)
	{
		UserLoopFun();
	}

	return 0;
}

#endif
