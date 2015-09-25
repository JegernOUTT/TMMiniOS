#include "Model.h"

#ifdef MVS_2012_32BIT 
	extern __int64 GetTimeTicks(void);
	extern int ModbusRTU_Master(int, int, int, int, int, int, int, int);
#endif
	    
//сложность моего алгоритма меньше n^2
unsigned long pow_2(int exp)
{
	unsigned long result = 1ul;

	for (int i = 0; i < exp; ++i)
	{
		result <<= 1;
	}

	return result;
}

CycleController * CycleController::cycleController = NULL;

CycleController::CycleController(void) : 
	slave(SlaveFactory::getSlaveFactoryPointer()), 
	deque(Deque::getInstance(MAX_DEVICE_COUNT * MAX_DEVICE_COUNT))
{
	devices = new AbstractDevice * [MAX_DEVICE_COUNT];

	for (int i = 0; i < MAX_DEVICE_COUNT; i++)
	{
		devices[i] = NULL;
	}

	properties = NULL;

	properties_old = new PropertiesStruct[MAX_DEVICE_COUNT];
	memset(properties_old, 0, sizeof(* properties_old) * MAX_DEVICE_COUNT);

	deviceMask			= 0;
	deviceMaskOld		= 0;

	deviceCount			= 0;
	propertyCount		= 0;
	timeTick			= 0;
	attemptToLoad		= 0;
	workFlag			= 0;

	writeProcess		= false_t;
	readProcess			= true_t;
	frameIsProcessed	= 0;
	currentFrame		= NULL;
}

CycleController::~CycleController(void)
{
	for (int i = 0; i < MAX_DEVICE_COUNT; i++)
	{
		delete devices[i];
		devices[i] = NULL;
	}

	delete [] devices;
	devices = NULL;

	delete [] properties;
	properties = NULL;
}

CycleController & CycleController::getInstance()
{
	if (cycleController == NULL)
	{
		cycleController = new CycleController();
	}

	return * cycleController;
}

void CycleController::init()
{
	SlaveInformation sv(INPUT_REGISTERS, 0);

	properties = EEpromSettings::loadFromEeprom(&deviceMask);
	propertiesToSlave();
}

void CycleController::read()
{
	readDeviceMask();
	readProperties();
	checkMask();

	if (isPropertyNew())
	{
		EEpromSettings::saveToEeprom(properties, deviceMask);
		constructNewDevices();
	}

	//Events for modbus devices
	for (int i = 0; i < MAX_DEVICE_COUNT; ++i)
	{
		if (devices[i] != NULL && devices[i]->protocolType_t == MODBUS_PROTOCOL)
		{
			devices[i]->eventsProcess();
		}
	}

	readNonModbusDevices();
	readModbusDevices();
}

void CycleController::propertiesToSlave()
{
	unsigned short tmpMask = deviceMask;
	int count = 0;
	
	if (tmpMask > 0)
	{
		SlaveInformation maskInformation(HOLDING_REGISTERS, DEVICE_MASK_ADDRESS);
		uValue val(deviceMask);
		slave.setHoldingRegisters(maskInformation, val);
	}

	while (tmpMask > 0)
	{
		if (tmpMask & 1)
		{
			SlaveInformation sv(HOLDING_REGISTERS, 0 + (count * 5));
			uValue val;

			val.iVal[0] = properties[count].deviceType;
			slave.setHoldingRegisters(sv, val);

			val.iVal[0] = properties[count].modbusAddress;
			sv.registerNumber = 1 + (count * 5);
			slave.setHoldingRegisters(sv, val);

			val.iVal[0] = properties[count].startSlaveAddress;
			sv.registerNumber = 2 + (count * 5);
			slave.setHoldingRegisters(sv, val);

			val.iVal[0] = properties[count].extra_1;
			sv.registerNumber = 3 + (count * 5);
			slave.setHoldingRegisters(sv, val);

			val.iVal[0] = properties[count].extra_2;
			sv.registerNumber = 4 + (count * 5);
			slave.setHoldingRegisters(sv, val);
		} 
		else
		{
			SlaveInformation sv(HOLDING_REGISTERS, 0 + (count * 5));
			uValue val;

			slave.setHoldingRegisters(sv, val);

			sv.registerNumber = 1 + (count * 5);
			slave.setHoldingRegisters(sv, val);

			sv.registerNumber = 2 + (count * 5);
			slave.setHoldingRegisters(sv, val);

			sv.registerNumber = 3 + (count * 5);
			slave.setHoldingRegisters(sv, val);

			sv.registerNumber = 4 + (count * 5);
			slave.setHoldingRegisters(sv, val);
		}

		tmpMask >>= 1;
		++count;
	}
}

void CycleController::readDeviceMask()
{
	SlaveInformation maskInformation(HOLDING_REGISTERS, DEVICE_MASK_ADDRESS);

	deviceMask = slave.getHoldingRegisters(maskInformation, 2).ulVal;
}

void CycleController::readProperties()
{
	unsigned short tmpMask = deviceMask;
	int count = 0;

	while (tmpMask > 0)
	{
		if (tmpMask & 1)
		{
			SlaveInformation sv(HOLDING_REGISTERS, 0 + (count * 5));
			
			properties[count].deviceType = slave.getHoldingRegisters(sv, 2).iVal[0];

			sv.registerNumber = 1 + (count * 5);	
			properties[count].modbusAddress = slave.getHoldingRegisters(sv, 2).iVal[0];

			sv.registerNumber = 2 + (count * 5);
			properties[count].startSlaveAddress = slave.getHoldingRegisters(sv, 2).iVal[0];

			sv.registerNumber = 3 + (count * 5);
			properties[count].extra_1 = slave.getHoldingRegisters(sv, 2).iVal[0];

			sv.registerNumber = 4 + (count * 5);
			properties[count].extra_2 = slave.getHoldingRegisters(sv, 2).iVal[0];
		} 
		else
		{
			properties[count].deviceType =			0;
			properties[count].modbusAddress =		0;
			properties[count].startSlaveAddress =	0;
			properties[count].extra_1 =				0;
			properties[count].extra_2 =				0;
		}

		tmpMask >>= 1;
		++count;
	}
}

bool_t CycleController::isPropertyNew()
{
	unsigned short tmpMask = deviceMask;
	int count = 0;
	bool_t isNew = false_t;

	while (tmpMask > 0)
	{
		if ((deviceMask != deviceMaskOld ||
			properties[count].deviceType != properties_old[count].deviceType) ||
			(properties[count].modbusAddress != properties_old[count].modbusAddress) ||
			(properties[count].startSlaveAddress != properties_old[count].startSlaveAddress) ||
			(properties[count].extra_1 != properties_old[count].extra_1) ||
			(properties[count].extra_2 != properties_old[count].extra_2))
			isNew = true_t;

		tmpMask >>= 1;
		++count;
	}

	if (isNew)
	{
		for (int i = 0; i < MAX_DEVICE_COUNT; i++)
		{
			properties_old[i].deviceType = properties[i].deviceType;
			properties_old[i].startSlaveAddress = properties[i].startSlaveAddress;
			properties_old[i].modbusAddress = properties[i].modbusAddress;
			properties_old[i].extra_1 = properties[i].extra_1;
			properties_old[i].extra_2 = properties[i].extra_2;
			deviceMaskOld = deviceMask;
		}
	}

	return isNew;
}

void CycleController::checkMask()
{
	for (int i = 0; i < MAX_DEVICE_COUNT; ++i)
	{
		if (
				((properties[i].deviceType <= 0)					|| (properties[i].deviceType > MAX_DEVICE_COUNT))
			||	((properties[i].modbusAddress < 0)					|| (properties[i].modbusAddress > 255))
			||	((properties[i].startSlaveAddress < 0)				|| (properties[i].startSlaveAddress > MODBUS_READ_ADDRESS))
			|| ((properties[i].extra_1 > pow_2(MAX_DEVICE_COUNT))	|| (properties[i].extra_2 > pow_2(MAX_DEVICE_COUNT)))
		   )
		{
			properties[i].deviceType =			0;
			properties[i].modbusAddress =		0;
			properties[i].startSlaveAddress =	0;
			properties[i].extra_1 =				0;
			properties[i].extra_2 =				0;

			deviceMask &= ~(unsigned short)(pow_2(i));
		}
	}
}

void CycleController::readNonModbusDevices()
{
	unsigned short err = 0;
	unsigned short tmpMask = deviceMask;
	int count = 0;

	while (tmpMask > 0)
	{
		if (tmpMask & 1)
		{
			if (devices[count] != NULL)
				if (devices[count]->protocolType_t != MODBUS_PROTOCOL)
				{
					for (int i = 0; i < devices[count]->propertyCount; i++)
					{
						devices[count]->processRead(devices[count]->properties[i]);
						err = devices[count]->processWrite(devices[count]->properties[i]);

						SlaveInformation sv(INPUT_REGISTERS, 1005);
						uValue val(err);
						slave.setInputRegisters(sv, val);
					}
				}
		}

		tmpMask >>= 1;
		++count;
	}
}

void CycleController::readModbusDevices() 
{
	if (deque.isEmpty())
	{
		for (int i = 0; i < MAX_DEVICE_COUNT; ++i)
		{
			if (devices[i] != NULL)
			{
				devices[i]->getTasksProperties();
				devices[i]->getTasksEvents();
			}
		}
	}

	if (frameIsProcessed <= 0)
	{
		if (GetTimeTicks() - timeTick > WAIT_PERIOD)							
		{
			timeTick = GetTimeTicks();

			currentFrame = deque.pop_front();

			if (currentFrame->taskTypeParameter == WRITE_TASK)
				nativeToRegisters();

			frameIsProcessed = ModbusRTU_Master(
				currentFrame->frame->comAddress, currentFrame->frame->modbusAddress, currentFrame->frame->functionCode,
				currentFrame->frame->toAddr, currentFrame->frame->fromAddr, currentFrame->frame->registerCountToRead,
				currentFrame->frame->timeout, currentFrame->frame->blockedMode
				);
		}
	}
	else if (frameIsProcessed >= 500 && frameIsProcessed <= 503)
	{
		if (currentFrame->taskTypeParameter == WRITE_TASK)
			nativeToRegisters();

		frameIsProcessed = ModbusRTU_Master(
			currentFrame->frame->comAddress, currentFrame->frame->modbusAddress, currentFrame->frame->functionCode,
			currentFrame->frame->toAddr, currentFrame->frame->fromAddr, currentFrame->frame->registerCountToRead,
			currentFrame->frame->timeout, currentFrame->frame->blockedMode
			);

		if (frameIsProcessed == 0)
		{
			if (currentFrame->taskTypeParameter == READ_TASK)
				nativeToRegisters();

			for (int i = 0; i < MAX_DEVICE_COUNT; ++i)
			{
				if (devices[i] != NULL)
					devices[i]->update();		
			}

			delete currentFrame->frame;
			delete currentFrame;
			currentFrame = NULL;
		}
	}
	
	SlaveInformation sv(INPUT_REGISTERS, 1500);
	uValue val(frameIsProcessed);
	slave.setInputRegisters(sv, val);
}

void CycleController::nativeToRegisters()
{
	/*
		1 ==> Reads  several  DO from device, then puts to iMemory_DO.
        2 ==> Reads  several  DI from device, then puts to iMemory_DI.
        3 ==> Reads  several  AO from device, then puts to iMemory_AO.
        4 ==> Reads  several  AI from device, then puts to iMemory_AI.
        5 ==> Writes     one  DO from iMemory_DO to device. 
        6 ==> Writes     one  AO from iMemory_AO to device.
        15 ==> Writes several DO from iMemory_DO to device.
        16 ==> Writes several AO from iMemory_AO to device.
	*/
	SlaveInformation sv(COILS, currentFrame->frame->toAddr);
	uValue val;
	int i = 0;
	unsigned short * usVal = (unsigned short *) currentFrame->nativeValueAddr;

	switch (currentFrame->frame->functionCode)
	{
	case 1:	
		for (i = 0; i < currentFrame->frame->registerCountToRead; ++i)
		{
			usVal[i] = slave.getCoils(sv, 2).ulVal;
			++sv.registerNumber;
		}
		break;

	case 2:	
		for (i = 0; i < currentFrame->frame->registerCountToRead; ++i)
		{
			usVal[i] = slave.getDisreteInput(sv, 2).ulVal;
			++sv.registerNumber;
		}
		break;

	case 3:	
		for (i = 0; i < currentFrame->frame->registerCountToRead; ++i)
		{
			usVal[i] = slave.getHoldingRegisters(sv, 2).ulVal;
			++sv.registerNumber;
		}
		break;

	case 4: 
		for (i = 0; i < currentFrame->frame->registerCountToRead; ++i)
		{
			usVal[i] = slave.getInputRegisters(sv, 2).ulVal;
			++sv.registerNumber;
		}					
		break;

	case 15: 
		for (i = 0; i < currentFrame->frame->registerCountToRead; ++i)
		{
			val.ulVal =  usVal[i];
			slave.setCoils(sv, val);
			++sv.registerNumber;
		}
		break;

	case 16:
		for (i = 0; i < currentFrame->frame->registerCountToRead; ++i)
		{
			val.ulVal =  usVal[i];
			slave.setHoldingRegisters(sv, val);
			++sv.registerNumber;
		}
		break;
	}
}

//При добавлении устройства добавить в этот метод
//Подумать про оптимизацию
void CycleController::constructNewDevices()
{
	SlaveInformation slaveInformation(INPUT_REGISTERS, 0);

	for (int i = 0; i < MAX_DEVICE_COUNT; ++i)
	{
		if (deviceMask & (unsigned short)pow_2(i))
		{
			switch (properties[i].deviceType)
			{
			case BSKZH_DEVICE:
				slaveInformation.registerNumber = properties[i].startSlaveAddress;

				if (devices[i] != NULL)
				{
					if (devices[i]->deviceTypeId == BSKZH_DEVICE)
					{
						devices[i]->setAddress(properties[i].modbusAddress);
						devices[i]->setCountableMask(properties[i].extra_1);
						devices[i]->setEventMask(properties[i].extra_2);
						SlaveAddressHandler::setSlaveInformation(devices[i], slaveInformation);
					}
					else
					{
						delete devices[i];
						devices[i] = new BskzhDevice(properties[i].modbusAddress, properties[i].extra_2);
						devices[i]->setCountableMask(properties[i].extra_1);
						SlaveAddressHandler::setSlaveInformation(devices[i], slaveInformation);
					}
				} 
				else
				{
					devices[i] = new BskzhDevice(properties[i].modbusAddress, properties[i].extra_2);
					devices[i]->setCountableMask(properties[i].extra_1);
					SlaveAddressHandler::setSlaveInformation(devices[i], slaveInformation);
				}
				break;

			case I7002_DEVICE:
				slaveInformation.registerNumber = properties[i].startSlaveAddress;

				if (devices[i] != NULL)
				{
					if (devices[i]->deviceTypeId == I7002_DEVICE)
					{
						devices[i]->setAddress(properties[i].modbusAddress);
						devices[i]->setCountableMask(properties[i].extra_1);
						devices[i]->setEventMask(properties[i].extra_2);
						SlaveAddressHandler::setSlaveInformation(devices[i], slaveInformation);
					}
					else
					{
						delete devices[i];
						devices[i] = new I7002Device(properties[i].modbusAddress, properties[i].extra_2);
						devices[i]->setCountableMask(properties[i].extra_1);
						SlaveAddressHandler::setSlaveInformation(devices[i], slaveInformation);
					}
				}
				else
				{
					devices[i] = new I7002Device(properties[i].modbusAddress, properties[i].extra_2);
					devices[i]->setCountableMask(properties[i].extra_1);
					SlaveAddressHandler::setSlaveInformation(devices[i], slaveInformation);
				}
				break;

			case I7041_DEVICE:
				slaveInformation.registerNumber = properties[i].startSlaveAddress;

				if (devices[i] != NULL)
				{
					if (devices[i]->deviceTypeId == I7041_DEVICE)
					{
						devices[i]->setAddress(properties[i].modbusAddress);
						devices[i]->setCountableMask(properties[i].extra_1);
						devices[i]->setEventMask(properties[i].extra_2);
						SlaveAddressHandler::setSlaveInformation(devices[i], slaveInformation);
					}
					else
					{
						delete devices[i];
						devices[i] = new I7041Device(properties[i].modbusAddress, properties[i].extra_2);
						devices[i]->setCountableMask(properties[i].extra_1);
						SlaveAddressHandler::setSlaveInformation(devices[i], slaveInformation);
					}
				}
				else
				{
					devices[i] = new I7041Device(properties[i].modbusAddress, properties[i].extra_2);
					devices[i]->setCountableMask(properties[i].extra_1);
					SlaveAddressHandler::setSlaveInformation(devices[i], slaveInformation);
				}
				break;

			case X304_DEVICE:
				slaveInformation.registerNumber = properties[i].startSlaveAddress;

				if (devices[i] != NULL)
				{
					if (devices[i]->deviceTypeId == X304_DEVICE)
					{
						devices[i]->setAddress(properties[i].modbusAddress);
						devices[i]->setCountableMask(properties[i].extra_1);
						devices[i]->setEventMask(properties[i].extra_2);
						SlaveAddressHandler::setSlaveInformation(devices[i], slaveInformation);
					}
					else
					{
						delete devices[i];
						devices[i] = new X304Device(properties[i].modbusAddress, properties[i].extra_2);
						devices[i]->setCountableMask(properties[i].extra_1);
						SlaveAddressHandler::setSlaveInformation(devices[i], slaveInformation);
					}
				}
				else
				{
					devices[i] = new X304Device(properties[i].modbusAddress, properties[i].extra_2);
					devices[i]->setCountableMask(properties[i].extra_1);
					SlaveAddressHandler::setSlaveInformation(devices[i], slaveInformation);
				}
				break;
			}

		} 
		else
		{
			delete devices[i];
			devices[i] = NULL;
			deviceCount = 0;
			propertyCount = 0;
		}
	}
}
/*
AbstractDevice * getDeviceInstanse(deviceType type)
{
	switch (type)
	{
	case BSKZH_DEVICE: break;
	case I7002_DEVICE: break;
	case X304_DEVICE: break;
	case I7041_DEVICE: break;
	}
}
*/
AbstractDevice ** CycleController::getDevices(unsigned short & devCount)
{
	devCount = deviceMask;
	return devices;
}