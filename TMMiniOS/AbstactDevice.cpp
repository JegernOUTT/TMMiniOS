#include "Model.h"

AbstractDevice::AbstractDevice(short int propCount, unsigned short int addrr, unsigned short evMask) : 
	slave(SlaveFactory::getSlaveFactoryPointer()),
	deque(Deque::getInstance(MAX_DEVICE_COUNT * MAX_DEVICE_COUNT))
{
	events = new Event * [MAX_DEVICE_COUNT]; // MAX_DEVICE_COUNT потому что max для unsigned short mask
	for (int i = 0; i < MAX_DEVICE_COUNT; i++)
	{
		events[i] = NULL;
	}

	protocolType_t = MODBUS_PROTOCOL;
	propertyCount = propCount;
	iAddress = addrr;
	eventMask = evMask;
	bitCountable = 0;
	readErrror = 0;
	writeErrror = 0;

	constructEvents();
	slave = SlaveFactory::getSlaveFactoryPointer();
	properties = new Property * [propertyCount];
}

AbstractDevice::~AbstractDevice(void)
{     
	int i = 0;
	for (i = 0; i < propertyCount; i++)
	{
		properties[i]->setValueUnsignedLong(0);
		writeDataToSlave();
		delete properties[i];
	}

	for (i = 0; i < MAX_DEVICE_COUNT; i++)
	{
		delete events[i];
	}

	delete [] properties;
	delete [] events;
}

short int AbstractDevice::addProperty(Property * propTmp, short int propId)
{
	if (propId <= propertyCount)
	{
		properties[propId] = propTmp;
		return 0;
	} 
	else return -1;
}

short int AbstractDevice::deleteProperty(short int propId)
{
	if (propId <= propertyCount)
	{
		properties[propId]->~Property();
		properties[propId] = NULL;
		return 0;
	} 
	else return -1;
}

unsigned short int	AbstractDevice::getAddress()
{
	return iAddress;
}
	
void AbstractDevice::setAddress(unsigned short int val)
{
	iAddress = val;
}

void AbstractDevice::writeDataToSlave()
{
	for (int i = 0; i < propertyCount; ++i)
	{
		if (properties[i]->isCountable)
		{
			properties[i]->setValueFloat((* (unsigned long *) properties[i]->getNativeValue()) / 10.0f);
		};

		if (& slave != NULL)
		{
			switch (properties[i]->slaveInformation.registerType)
			{
			case DISCRETE_INPUTS: 
				slave.setDisreteInput(properties[i]->slaveInformation, properties[i]->value);
				break;
			case COILS: 
				slave.setCoils(properties[i]->slaveInformation, properties[i]->value);
				break;
			case INPUT_REGISTERS: 
				slave.setInputRegisters(properties[i]->slaveInformation, properties[i]->value);
				break;
			case HOLDING_REGISTERS: 
				slave.setHoldingRegisters(properties[i]->slaveInformation, properties[i]->value);
				break;
			}
		}
	}

}

void AbstractDevice::logProperty()
{

}

unsigned int AbstractDevice::processRead(Property * prop)
{
	readErrror = getVal(prop);
	if (readErrror >= 500) 
		return readErrror;

	writeDataToSlave();
	logProperty();

	return readErrror;
}

unsigned int AbstractDevice::processWrite(Property * prop)
{
	eventsProcess();
	writeErrror = setVal(prop);
	
	return writeErrror;
}

void AbstractDevice::eventsProcess()
{
	unsigned short tmpMask = eventMask;
	int count = 0;

	while (tmpMask > 0)
	{
		if (tmpMask & 1)
		{
			events[count]->process();
		}
		tmpMask >>= 1;
		++count;
	}
}

void AbstractDevice::constructEvents()
{
	unsigned short tmpMask = eventMask;
	int count = 0;

	//Очистка для возможной реконструкции
	for (int i = 0; i < MAX_DEVICE_COUNT; i++)
	{
		delete events[i];
		events[i] = NULL;
	}

	if (eventMask <= MAX_DEVICE_COUNT)
	{
		while (tmpMask > 0)
		{
			if (tmpMask & 1)
			{
				switch (count)
				{
				case SWITCH_ON_SIGNAL_EVENT: events[count] = new SwitchOnSignalEvent(this); break;
				/*case 1: break;
				case 2: break;
				case 3: break;
				case 4: break;
				case 5: break;
				case 6: break;
				case 7: break;
				case 8: break;
				case 9: break;
				case 10: break;
				case 11: break;
				case 12: break;
				case 13: break;
				case 14: break;
				case 15: break;*/
				default:
					break;
				}
			}
			tmpMask >>= 1;
			++count;
		}
	}
}

unsigned short AbstractDevice::getEventMask()
{
	return eventMask;
}

void AbstractDevice::setEventMask(unsigned short evMask)
{
	if (evMask >= MAX_DEVICE_COUNT)
	{
		evMask = MAX_DEVICE_COUNT; 
	} 
	else
	{
		eventMask = evMask;
	}
	
	constructEvents();
}

void AbstractDevice::setCountableMask(unsigned short countMask)
{
	if (countMask >= MAX_DEVICE_COUNT)
	{
		bitCountable = MAX_DEVICE_COUNT; 
	} 
	else
	{
		bitCountable = countMask;
	}
	
	unsigned short tmpCountMask = bitCountable;
	int count = 0;

	bool_t arrMask[MAX_DEVICE_COUNT];		//16 разрядов
	memset(arrMask, 0, sizeof(arrMask));

	while (tmpCountMask > 0)
	{
		if (tmpCountMask & 1)
		{
			arrMask[count] = true_t;
		}
		else
		{
			arrMask[count] = false_t;
		}

		++count;
		tmpCountMask >>= 1;
	}
	count = 0;

	for (int i = 0; i < propertyCount; ++i)
	{
		if (properties[i] != NULL)
			if (properties[i]->sigType == COUNTABLE_PROPERTY)
			{
				properties[i]->isCountable = arrMask[count];
				++count;
			}
	}
}

unsigned short AbstractDevice::getCountableMask()
{
	return bitCountable;
}

void AbstractDevice::getTasksProperties() 
{

}

void AbstractDevice::getTasksEvents() 
{

}

void AbstractDevice::update() 
{
	writeDataToSlave();
	logProperty();	
}

bool_t AbstractDevice::isOwnProperty(Property * property)
{
	for (int i = 0; i < propertyCount; ++i)
	{
		if (property == properties[i])
			return true_t;
	}

	return false_t;
}