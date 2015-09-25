#include "Model.h"

SlaveFactory * SlaveFactory::slaveFactoryPointer = NULL;

SlaveFactory::SlaveFactory(void)
{
	memset(iMemory_DI, 0, REGISTER_MAX);
	memset(iMemory_DO, 0, REGISTER_MAX);
	memset(iMemory_AI, 0, REGISTER_MAX * 2);
	memset(iMemory_AO, 0, REGISTER_MAX * 2);


#ifdef _DEBUG
	/*iMemory_AO[DEVICE_MASK_ADDRESS] = 3;

	iMemory_AO[0] = BSKZH_DEVICE;
	iMemory_AO[1] = 2;
	iMemory_AO[2] = 0;
	iMemory_AO[3] = 0;
	iMemory_AO[4] = 1;

	iMemory_AO[5] = BSKZH_DEVICE;
	iMemory_AO[6] = 3;
	iMemory_AO[7] = 4;
	iMemory_AO[8] = 0;
	iMemory_AO[9] = 1;*/
#endif

#ifdef VC_1992_16BIT
	Set_COMEnableMode(COM_PORT_2, _Programming);
	InitModbus(iMemory_DI, iMemory_DO, (int*)iMemory_AI, (int*)iMemory_AO); 	
#endif
}

SlaveFactory::~SlaveFactory(void)
{

}

SlaveFactory & SlaveFactory::getSlaveFactoryPointer()
{
	if (slaveFactoryPointer == NULL)
	{
		slaveFactoryPointer = new SlaveFactory();
	}
	return * slaveFactoryPointer;
}

void SlaveFactory::setDisreteInput(SlaveInformation slaveInformation, const uValue & value)
{
	if (slaveInformation.registerNumber < REGISTER_MAX)
	{
		if (value.ulVal >= 1)
			* (iMemory_DI + slaveInformation.registerNumber) = 1;
		else
			* (iMemory_DI + slaveInformation.registerNumber) = 0;
	}
}
void SlaveFactory::setCoils(SlaveInformation slaveInformation, const uValue & value)
{
	if (slaveInformation.registerNumber < REGISTER_MAX)
	{
		if (value.ulVal >= 1)
			* (iMemory_DO + slaveInformation.registerNumber) = 1;
		else
			* (iMemory_DO + slaveInformation.registerNumber) = 0;
	}
}
void SlaveFactory::setInputRegisters(SlaveInformation slaveInformation, const uValue & value)
{
	if (slaveInformation.registerNumber < REGISTER_MAX)
	{
		unsigned long * adr = (unsigned long *)(iMemory_AI + slaveInformation.registerNumber);
		* adr = value.ulVal;
	}
}
void SlaveFactory::setHoldingRegisters(SlaveInformation slaveInformation, const uValue & value)
{
	if (slaveInformation.registerNumber < REGISTER_MAX)
	{
		unsigned long * adr = (unsigned long * )(iMemory_AO + slaveInformation.registerNumber);
		* adr = value.ulVal;
	}
}

uValue SlaveFactory::getDisreteInput(SlaveInformation slaveInformation, size_t bytesCount)
{
	uValue value;
	value.lVal = 0;
	
	switch (bytesCount)
	{
	case 2:
		value.iVal[0] = iMemory_DI[slaveInformation.registerNumber];
		value.iVal[1] = 0;
		break;
	case 4:
		value.ulVal = iMemory_DI[slaveInformation.registerNumber];
		break;
	default:
		break;
	}

	return value;
}
uValue SlaveFactory::getCoils(SlaveInformation slaveInformation, size_t bytesCount)
{
	uValue value;
	value.lVal = 0;
	
	switch (bytesCount)
	{
	case 2:
		value.iVal[0] = iMemory_DO[slaveInformation.registerNumber];
		value.iVal[1] = 0;
		break;
	case 4:
		value.ulVal = iMemory_DO[slaveInformation.registerNumber];
		break;
	default:
		break;
	}

	return value;
}
uValue SlaveFactory::getInputRegisters(SlaveInformation slaveInformation, size_t bytesCount)
{
	uValue value;
	value.lVal = 0;
	
	switch (bytesCount)
	{
	case 2:
		value.iVal[0] = iMemory_AI[slaveInformation.registerNumber];
		value.iVal[1] = 0;
		break;
	case 4:
		value.ulVal = iMemory_AI[slaveInformation.registerNumber];
		break;
	default:
		break;
	}

	return value;
}
uValue SlaveFactory::getHoldingRegisters(SlaveInformation slaveInformation, size_t bytesCount)
{
	uValue value;
	value.lVal = 0;
	
	switch (bytesCount)
	{
	case 2:
		value.iVal[0] = iMemory_AO[slaveInformation.registerNumber];
		value.iVal[1] = 0;
		break;
	case 4:
		value.ulVal = iMemory_AO[slaveInformation.registerNumber];
		break;
	default:
		break;
	}

	return value;
}

void SlaveFactory::registerClear(SlaveInformation slaveInformation, int regCount)
{
	switch (slaveInformation.registerType)
	{
	case COILS:	
		memset((iMemory_DO + slaveInformation.registerNumber), 0, regCount * sizeof(short int));
		break;

	case DISCRETE_INPUTS:
		memset((iMemory_DI + slaveInformation.registerNumber), 0, regCount * sizeof(short int));
		break;

	case HOLDING_REGISTERS:	
		memset((iMemory_AO + slaveInformation.registerNumber), 0, regCount * sizeof(short int));
		break;

	case INPUT_REGISTERS:
		memset((iMemory_AI + slaveInformation.registerNumber), 0, regCount * sizeof(short int));
		break;

	default:
		break;
	}
}