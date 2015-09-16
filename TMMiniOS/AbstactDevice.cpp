#include "Model.h"

AbstractDevice::AbstractDevice(short int propCount, unsigned short int addrr, unsigned short evMask) : 
	slave(SlaveFactory::getSlaveFactoryPointer())
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
		writeDataToSlave(properties[i]);
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

void AbstractDevice::writeDataToSlave(Property* prop)
{
	if (prop->isCountable)
	{
		prop->setValueFloat((*(unsigned long *)prop->getNativeValue()) / 10.0f);
	};

	if (&slave != NULL)
	{
		switch (prop->slaveInformation.registerType)
		{
		case DISCRETE_INPUTS: 
			slave.setDisreteInput(prop->slaveInformation, prop->value);
			break;
		case COILS: 
			slave.setCoils(prop->slaveInformation, prop->value);
			break;
		case INPUT_REGISTERS: 
			slave.setInputRegisters(prop->slaveInformation, prop->value);
			break;
		case HOLDING_REGISTERS: 
			slave.setHoldingRegisters(prop->slaveInformation, prop->value);
			break;
		}
	}
}

void AbstractDevice::logProperty(Property* prop)
{

}

unsigned int AbstractDevice::processRead(Property* prop)
{
	readErrror = getVal(prop);
	if (readErrror >= 500) 
		return readErrror;

	writeDataToSlave(prop);
	logProperty(prop);

	return readErrror;
}

unsigned int AbstractDevice::processWrite(Property* prop)
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