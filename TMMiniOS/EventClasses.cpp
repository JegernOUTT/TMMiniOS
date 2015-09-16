#include "Model.h"

SwitchOnSignalEvent::SwitchOnSignalEvent(AbstractDevice * dev)
	: Event(dev),
	channelNumberFrom(1), channelNumberTo(2)
{
	device = dev;
}

SwitchOnSignalEvent::~SwitchOnSignalEvent(void)
{

}

void SwitchOnSignalEvent::process() 
{
	int tmpPropCount = 0, j = 0;
	bool_t value = false_t;

	if (device->deviceTypeId == I7002_DEVICE 
		|| device->deviceTypeId == X304_DEVICE)
	{
		for (j = 0; j < device->propertyCount; j++)
		{
			if (device->properties[j]->propType == I7002_DISCRETE_INPUT_PROPERTY 
				|| device->properties[j]->propType == X304_DISCRETE_PROPERTY)
			{
				++tmpPropCount;
			}

			if (tmpPropCount == channelNumberFrom)
			{
				value = device->properties[j]->getValueBool();
			}
		}

		tmpPropCount = 0;
		for (j = 0; j < device->propertyCount; j++)
		{
			if (device->properties[j]->propType == I7002_DISCRETE_OUTPUT_PROPERTY 
				|| device->properties[j]->propType == X304_DISCRETE_OUTPUT_PROPERTY)
			{
				++tmpPropCount;
			}

			if (tmpPropCount == channelNumberTo)
			{
				device->properties[j]->setValueBool(value);
			}
		}
	}
} 
