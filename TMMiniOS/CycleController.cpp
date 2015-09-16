#include "Model.h"

#ifdef MVS_2012_32BIT 
	extern __int64 GetTimeTicks(void);
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
	slave(SlaveFactory::getSlaveFactoryPointer())
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
	unsigned int err = 0;
	readDeviceMask();
	readProperties();
	checkMask();

	if (isPropertyNew())
	{
		EEpromSettings::saveToEeprom(properties, deviceMask);
		constructNewDevices();
	}

	readNonModbusDevices();

	if (devices[deviceCount] != NULL && devices[deviceCount]->protocolType_t == MODBUS_PROTOCOL)
	{
		if ((GetTimeTicks() - timeTick > WAIT_PERIOD) || workFlag)							
		{
			timeTick = GetTimeTicks();

			if (readProcess)
			{
				err = devices[deviceCount]->processRead(devices[deviceCount]->properties[propertyCount]);
				
  				if (err < 500 || err > 503)
				{
					readProcess = false_t;
					writeProcess = true_t;
				}
			}

			if (writeProcess)
			{
				err = devices[deviceCount]->processWrite(devices[deviceCount]->properties[propertyCount]);

				if (err < 500 || err > 503)
				{
					readProcess = true_t;
					writeProcess = false_t;
				}
			}
			
			#ifdef MVS_2012_32BIT
			std::cout << "Device: " << devices[deviceCount]->name << "  Property: " << devices[deviceCount]->properties[propertyCount]->name << "  Value: " 
				<< devices[deviceCount]->properties[propertyCount]->getValueFloat() << std::endl;
			#endif
			
			SlaveInformation sv(INPUT_REGISTERS, 1500);
			uValue val(err);
			slave.setInputRegisters(sv, val);

			if (err >= 500 && err <= 503)
			{
				++attemptToLoad;
				workFlag = 1;
				if (attemptToLoad > ATTEMPTS_COUNT)
				{
					attemptToLoad = 0;
					workFlag = 0;                    
					next();
				}
			}
			else
			{
				attemptToLoad = 0;
				workFlag = 0;
				next();
			}
		}
	}
	else
	{
		next();
	}
}

void CycleController::next()
{
	if (devices[deviceCount] != NULL && deviceMask & (unsigned short)pow_2(deviceCount))
	{
		if (++propertyCount >= devices[deviceCount]->propertyCount)
		{
			propertyCount = 0;
			++deviceCount;
		}
	}
	else
	{
		propertyCount = 0;
		++deviceCount;
	}

	if (deviceCount >= MAX_DEVICE_COUNT)
	{
		propertyCount = 0;
		deviceCount = 0;
	}
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