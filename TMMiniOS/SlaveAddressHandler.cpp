#include "Model.h"

SlaveAddressHandler::SlaveAddressHandler(void)
{
}

SlaveAddressHandler::~SlaveAddressHandler(void)
{
}

void SlaveAddressHandler::setSlaveInformation(AbstractDevice * device, SlaveInformation startAddress)
{
	for (int i = 0; i < device->propertyCount; i++)
	{
		device->properties[i]->slaveInformation.registerNumber = startAddress.registerNumber;
		device->properties[i]->slaveInformation.registerType = startAddress.registerType;

		startAddress.registerNumber += 2;
	}
}