#include "Model.h"

/*!!!   BskzhMass1Property   !!!*/
BskzhMass1Property::BskzhMass1Property(void)
{
	sprintf(name, "%s", "BSKZHMass1");
	propType = BSKZH_MASS1_PROPERTY;
	sigType = NON_COUNTABLE_PROPERTY;
}

BskzhMass1Property::~BskzhMass1Property(void)
{
}

void BskzhMass1Property::calculate() 
{

}



/*!!!   BskzhMass2Property   !!!*/
BskzhMass2Property::BskzhMass2Property(void)
{
	sprintf(name, "%s", "BSKZHMass2");
	propType = BSKZH_MASS2_PROPERTY;
	sigType = NON_COUNTABLE_PROPERTY;
}

BskzhMass2Property::~BskzhMass2Property(void)
{
}

void BskzhMass2Property::calculate() 
{

}



/*!!!   X304AnalogProperty   !!!*/
X304AnalogProperty::X304AnalogProperty(short int ch)
{
	sprintf(name, "%s", "X304Analog");
	sigType = NON_COUNTABLE_PROPERTY;
	iChannel = ch;
	propType = X304_ANALOG_PROPERTY;
}

X304AnalogProperty::~X304AnalogProperty(void)
{
}

void X304AnalogProperty::calculate() 
{

}



/*!!!   X304DiscreteProperty   !!!*/
X304DiscreteProperty::X304DiscreteProperty(short int ch)
{
	sprintf(name, "%s", "X304Discrete");
	sigType = COUNTABLE_PROPERTY;
	iChannel = ch;
	propType = X304_DISCRETE_PROPERTY;

	lastValue = 0;	//допольнительно проинициализирован в устройстве X304 loadFromEEprom
	#ifdef MVS_2012_32BIT
		timeToSave	=				60 * 10;	
	#else VC_1992_16BIT
		timeToSave  =				60000 * 10;
	#endif
	timeTickCheck = 0;
	timeTickSave = 0;
	velocityValue = 0;
}

X304DiscreteProperty::~X304DiscreteProperty(void)
{
}

void X304DiscreteProperty::calculate() 
{

}



/*!!!   X304DiscreteOutputProperty   !!!*/
X304DiscreteOutputProperty::X304DiscreteOutputProperty(short int ch)
{
	sprintf(name, "%s", "X304DiscreteOutput");
	sigType = COUNTABLE_PROPERTY;
	iChannel = ch;
	propType = X304_DISCRETE_OUTPUT_PROPERTY;
}

X304DiscreteOutputProperty::~X304DiscreteOutputProperty(void)
{
}

void X304DiscreteOutputProperty::calculate() 
{

}



/*!!!   I7002DiscreteInputProperty   !!!*/
I7002DiscreteInputProperty::I7002DiscreteInputProperty(short int ch)
{
	sprintf(name, "%s", "I7002DiscreteInput");
	sigType = COUNTABLE_PROPERTY;
	iChannel = ch;
	propType = I7002_DISCRETE_INPUT_PROPERTY;
}

I7002DiscreteInputProperty::~I7002DiscreteInputProperty(void)
{
}

void I7002DiscreteInputProperty::calculate() 
{

}



/*!!!   I7002DiscreteInputProperty   !!!*/
I7002DiscreteOutputProperty::I7002DiscreteOutputProperty(short int ch)
{
	sprintf(name, "%s", "I7002DiscreteOutput");
	sigType = NON_COUNTABLE_PROPERTY;
	iChannel = ch;
	propType = I7002_DISCRETE_OUTPUT_PROPERTY;
}

I7002DiscreteOutputProperty::~I7002DiscreteOutputProperty(void)
{

}

void I7002DiscreteOutputProperty::calculate() 
{

}



/*!!!   I7002AnalogProperty   !!!*/
I7002AnalogProperty::I7002AnalogProperty(short int ch)
{
	sprintf(name, "%s", "I7002Analog");
	sigType = NON_COUNTABLE_PROPERTY;
	iChannel = ch;
	propType = I7002_ANALOG_INPUT_PROPERTY;
}

I7002AnalogProperty::~I7002AnalogProperty(void)
{

}

void I7002AnalogProperty::calculate() 
{

}



/*!!!   I7041DiscreteProperty   !!!*/
I7041DiscreteProperty::I7041DiscreteProperty(short int ch)
{
	sprintf(name, "%s", "I7041Discrete");
	sigType = COUNTABLE_PROPERTY;
	iChannel = ch;
	propType = I7041_DISCRETE_INPUT_PROPERTY;
}

I7041DiscreteProperty::~I7041DiscreteProperty(void)
{

}

void I7041DiscreteProperty::calculate() 
{

}



/*!!!   ErrorProperty   !!!*/
ErrorProperty::ErrorProperty()
{
	sprintf(name, "%s", "Error");
	sigType = NON_COUNTABLE_PROPERTY;
	propType = ERROR_PROPERTY;
}

ErrorProperty::~ErrorProperty(void)
{

}

void ErrorProperty::calculate() 
{

}