#include "Model.h"

Property::Property(void)
	: slaveInformation(INPUT_REGISTERS, 0), 
	value(), valueOld(1)
{
	nativeValue = malloc(MALLOC_NATIVE_VALUE_BYTES);
	memset(nativeValue, 0, MALLOC_NATIVE_VALUE_BYTES);
	readErr = 0;
	writeErr = 0;
	sigType = NON_COUNTABLE_PROPERTY;
	isCountable = false_t;
}

Property::~Property(void)
{
	free(nativeValue);
}

float Property::getValueFloat()
{
	calculate();
	return value.fVal;
}

short int Property::getValueInt()
{
	calculate();
	return value.iVal[0];
}

long Property::getValueLong()
{
	calculate();
	return value.lVal;
}

unsigned long Property::getValueUnsignedLong()
{
	calculate();
	return value.ulVal;
}

bool_t Property::getValueBool()
{
	calculate();
	return value.bVal[0];
}

void Property::setValueFloat(float val)
{
	value.fVal = val;
}

void Property::setValueInt(short int val)
{
	value.iVal[0] = val;
	value.iVal[1] = 0;
}

void Property::setValueLong(long val)
{
	value.lVal = val;
}

void Property::setValueUnsignedLong(unsigned long val)
{
	value.ulVal = val;
}

void Property::setValueBool(bool_t val)
{
	value.bVal[0] = val;
	value.bVal[1] = false_t;
	value.bVal[2] = false_t;
	value.bVal[3] = false_t;
}

void * Property::getNativeValue()
{
	return nativeValue;
}