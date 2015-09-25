#include "Model.h"

#ifdef MVS_2012_32BIT
extern __int64 GetTimeTicks(void);
extern int ModbusRTU_Master(int, int, int, int, int, int, int, int);
extern float X304_AnalogIn(int);
extern unsigned short X304_DigitalIn();
extern unsigned short X304_Init();
extern unsigned short X304_Read_All_DO();
extern void X304_Write_All_DO(unsigned int);
#endif
extern unsigned long pow_2(int);

/*!!!   BskzhDevice   !!!*/
BskzhDevice::BskzhDevice(unsigned short int addrr, unsigned short evMask) :
	AbstractDevice(3, addrr, evMask)
{
	sprintf(name, "%s ch: %d", "BSKZH", addrr);
	deviceTypeId = BSKZH_DEVICE;
	properties[0] = new BskzhMass1Property();
	properties[1] = new BskzhMass2Property();
	properties[2] = new ErrorProperty();

	nativeValue = malloc(sizeof(unsigned short) * 20);
	memset(nativeValue, 0, sizeof(unsigned short) * 20);
}

BskzhDevice::~BskzhDevice(void)
{
	free(nativeValue);
}

void BskzhDevice::getTasksProperties()		// Задать деку свойствами 
{
	TaskFrame * frame = new TaskFrame(new ModbusRtuMasterFrame (COM_PORT_2, getAddress(), 4, MODBUS_READ_ADDRESS - 1, 50, 20, 1000, 0), 
		nativeValue, READ_TASK);
	deque.push_back(frame);
}

void BskzhDevice::update()					//Из native value в slave modbus
{
	for (int i = 0; i < propertyCount; ++i)
	{
		unsigned short * values = (unsigned short *) nativeValue;

		for (int i = 0; i < propertyCount; ++i)
		{
			if (properties[i]->propType == BSKZH_MASS1_PROPERTY)
			{
				properties[i]->setValueFloat((float)((values[0] * 4294967296.0) + (values[1] * 65536.0) + values[2]) / 10000000.0);
			}
			else if (properties[i]->propType == BSKZH_MASS2_PROPERTY)
			{
				properties[i]->setValueFloat((float)((values[0 + 9] * 4294967296.0) + (values[1 + 9] * 65536.0) + values[2 + 9]) / 10000000.0);
			}
			else if (properties[i]->propType == ERROR_PROPERTY)
			{
				properties[i]->setValueUnsignedLong(readErrror);
			}
		}

		AbstractDevice::update();
	}
}


/*!!!   X304Device   !!!*/
X304Device::X304Device(unsigned short int addrr, unsigned short evMask) :
	AbstractDevice(12, addrr, evMask)
{         
	err = 0;
	int i = 0;
	sprintf(name, "%s ch: %d", "X304", addrr);
	deviceTypeId = X304_DEVICE;
	protocolType_t = XBOARD_PROTOCOL;

	for (i = 0; i < 3; i++)
	{
		properties[i] = new X304AnalogProperty(i);
	}

	for (i = 3; i < 7; i++)
	{
		properties[i] = new X304DiscreteProperty(i - 3);
	}

	for (i = 7; i < 11; i++)
	{
		properties[i] = new X304DiscreteOutputProperty(i - 7);
	}

	properties[11] = new ErrorProperty();

	err = X304_Init();	

	loadSavedCountsFromEEPROM();
}

X304Device::~X304Device(void)
{

}

unsigned int X304Device::getVal(Property * prop)
{
	if (err == 0)
	{
		unsigned short discreteInputValues = 0, discreteOutputValues = 0;
		discreteInputValues = X304_DigitalIn();
		discreteOutputValues = X304_Read_All_DO();

		prop->readErr = err;

		if (prop->propType == X304_ANALOG_PROPERTY)
		{
			X304AnalogProperty * property = (X304AnalogProperty *) prop;
			prop->setValueFloat(X304_AnalogIn(property->iChannel));
		}
		else if (prop->propType == X304_DISCRETE_PROPERTY)
		{
			X304DiscreteProperty * property = (X304DiscreteProperty *) prop;

			if (discreteInputValues & (unsigned short)pow_2(property->iChannel))
			{
				prop->setValueInt(1);
			}
			else
			{
				prop->setValueInt(0);
			}

			if (property->isCountable)
			{
				if (property->getValueBool() != property->valueOld.bVal[0])
				{
					if (!property->getValueBool())
					{
						(* (unsigned long *)property->getNativeValue())++;
						saveCountsToEEPROM(property);
						
						SlaveInformation inf(INPUT_REGISTERS, 500);
						uValue val(property->lastValue);
						slave.setInputRegisters(inf, val);

						inf.registerNumber = 502;
						val.ulVal = property->velocityValue;
						slave.setInputRegisters(inf, val);
					}
				}
			}
			else
			{
				(*(unsigned long *)property->getNativeValue()) = 0;
			}
			
			property->valueOld.bVal[0] = property->getValueBool();
		}
		else if (prop->propType == X304_DISCRETE_OUTPUT_PROPERTY)
		{
			X304DiscreteOutputProperty * property = (X304DiscreteOutputProperty *) prop;

			if (discreteOutputValues & (unsigned short)pow_2(property->iChannel))
			{
				prop->setValueInt(1);
			}
			else
			{
				prop->setValueInt(0);
			}
		}
		else if (prop->propType == ERROR_PROPERTY)
		{
			prop->setValueUnsignedLong(prop->readErr);
		}
	}
	else
	{
		err = X304_Init();
	}

	return err;
}

unsigned int X304Device::setVal(Property * prop)
{
	unsigned short val = 0;

	for (int i = 0; i < this->propertyCount; ++i)
	{
		if (this->properties[i]->propType == X304_DISCRETE_OUTPUT_PROPERTY)
		{
			X304DiscreteOutputProperty & x304Property = * (X304DiscreteOutputProperty *) this->properties[i];

			if (x304Property.getValueBool())
			{
				val |= (unsigned short)pow_2(x304Property.iChannel);
			}
			else
			{
				val &= ~(unsigned short)pow_2(x304Property.iChannel);
			}
		}
	}

	X304_Write_All_DO(val);

	return 0;
}

void X304Device::loadSavedCountsFromEEPROM()
{
	//проинициализировать lastValue
	for (int i = 0; i < this->propertyCount; ++i)
	{
		if (this->properties[i]->propType == X304_DISCRETE_PROPERTY)
		{
			X304DiscreteProperty * discreteProperty = (X304DiscreteProperty *) this->properties[i];
			#ifdef MVS_2012_32BIT
				char buff[200];
				FILE * fp;

				sprintf(buff, "tmp_x304ch.b_%d", discreteProperty->iChannel);
				fp = fopen(buff, "rb");
				
				if (fp != NULL)
				{
					fread(& discreteProperty->lastValue, sizeof(unsigned long), sizeof(unsigned long), fp);
					fclose(fp);
				}
			#else VC_1992_16BIT
				EE_WriteEnable();        
	
				EE_MultiRead(4, 50 + discreteProperty->iChannel, sizeof(unsigned long), (char * ) & discreteProperty->lastValue); 
           
				EE_WriteProtect(); 
			#endif

			* (unsigned long *) discreteProperty->getNativeValue() = discreteProperty->lastValue;
		}
	}
}

void X304Device::saveCountsToEEPROM(Property * prop)
{
	#ifdef MVS_2012_32BIT
		unsigned long	checkTime =	60		* 5;	
	#else VC_1992_16BIT
		unsigned long	checkTime =	60000	* 5;
	#endif

	if (prop->propType == X304_DISCRETE_PROPERTY)
	{
		X304DiscreteProperty * discreteProperty = (X304DiscreteProperty *) prop;
		if (GetTimeTicks() - discreteProperty->timeTickCheck > checkTime)
		{
			discreteProperty->timeTickCheck = GetTimeTicks();
			//заход в процедуру проверки - каждые 5 минут
			//Найти количество тиков в 5 минут
			//Сравнить с таблицей и выбрать правильное время сохранения тиков
			discreteProperty->velocityValue = ( * (unsigned long *) discreteProperty->getNativeValue()) - discreteProperty->lastValue;
			if (discreteProperty->velocityValue == 0)
			{
				#ifdef MVS_2012_32BIT
					unsigned long	checkTime =	60		* 60;	
				#else VC_1992_16BIT
					unsigned long	checkTime =	60000	* 60;
				#endif
			} 
			else if (discreteProperty->velocityValue >= 1 && discreteProperty->velocityValue <= 2)
			{
				#ifdef MVS_2012_32BIT
					unsigned long	checkTime =	60		* 20;	
				#else VC_1992_16BIT
					unsigned long	checkTime =	60000	* 20;
				#endif			
			}
			else if (discreteProperty->velocityValue > 2 && discreteProperty->velocityValue <= 5)
			{
				#ifdef MVS_2012_32BIT
					unsigned long	checkTime =	60		* 10;	
				#else VC_1992_16BIT
					unsigned long	checkTime =	60000	* 10;
				#endif			
			}
			else if (discreteProperty->velocityValue > 5 && discreteProperty->velocityValue <= 10)
			{
				#ifdef MVS_2012_32BIT
					unsigned long	checkTime =	60		* 5;	
				#else VC_1992_16BIT
					unsigned long	checkTime =	60000	* 5;
				#endif			
			}
			else if (discreteProperty->velocityValue > 10)
			{
				#ifdef MVS_2012_32BIT
					unsigned long	checkTime =	60		* 1;	
				#else VC_1992_16BIT
					unsigned long	checkTime =	60000	* 1;
				#endif			
			}

			discreteProperty->lastValue = * (unsigned long *) discreteProperty->getNativeValue();
		}

		if (GetTimeTicks() - discreteProperty->timeTickSave > discreteProperty->timeToSave)
		{
			discreteProperty->timeTickSave = GetTimeTicks();
			//Заход в процедуру сохранения - высчитанное значение timeToSave, 5 min по умолчанию
			//Процедура сохранения тиков с расчитанным временным интервалом (интервал задаётся в самом начале)
			#ifdef MVS_2012_32BIT
				char buff[200];
				FILE * fp;

				sprintf(buff, "tmp_x304ch.b_%d", discreteProperty->iChannel);
				fp = fopen(buff, "wb");

				fwrite((unsigned long *) discreteProperty->getNativeValue(), sizeof(unsigned long), sizeof(unsigned long), fp);

				fclose(fp);
			#else VC_1992_16BIT
				EE_WriteEnable();        
	
				EE_MultiWrite_A(4, 50 + discreteProperty->iChannel, sizeof(unsigned long), (char * ) discreteProperty->getNativeValue()); 
           
				EE_WriteProtect(); 
			#endif
		}
	}
}


/*!!!   I7002Device   !!!*/
I7002Device::I7002Device(unsigned short int addrr, unsigned short evMask) :
	AbstractDevice(14, addrr, evMask)
{            
	int i = 0;
	sprintf(name, "%s ch: %d", "I7002", addrr);
	deviceTypeId = I7002_DEVICE;

	for (i = 0; i < 5; i++)
	{
		properties[i] = new I7002DiscreteInputProperty(i);
	}

	for (i = 5; i < 9; i++)
	{
		properties[i] = new I7002AnalogProperty(i - 5);
	}

	for (i = 9; i < 13; i++)
	{
		properties[i] = new I7002DiscreteOutputProperty(i - 9);
	}

	properties[13] = new ErrorProperty();

	nativeValue = malloc(sizeof(unsigned short) * 14);
	memset(nativeValue, 0, sizeof(unsigned short) * 14);
}

I7002Device::~I7002Device(void)
{
	free(nativeValue);
}

void I7002Device::getTasksProperties()		// Задать деку свойствами 
{
	unsigned short * usVal = (unsigned short *) nativeValue;
	int propCount = 0, i = 0;

	//Analog frame
	TaskFrame * frame = new TaskFrame(new ModbusRtuMasterFrame (COM_PORT_2, getAddress(), 4, MODBUS_READ_ADDRESS - 1, 0, 4, 1000, 0), 
		usVal, READ_TASK);
	deque.push_back(frame);

	//Discrete Output frame
	usVal += 4;
	frame = new TaskFrame(new ModbusRtuMasterFrame (COM_PORT_2, getAddress(), 2, MODBUS_READ_ADDRESS - 1, 0, 4, 1000, 0), 
		usVal, READ_TASK);
	deque.push_back(frame);

	//Discrete Input frames
	usVal += 4;
	bool_t isPropCountable = false_t;
	for (i = 0; i < propertyCount; ++i)
	{
		if (properties[i]->propType == I7002_DISCRETE_INPUT_PROPERTY)
			if (properties[i]->isCountable)
				isPropCountable = true_t;
	}
	
	if (!isPropCountable)
	{
		frame = new TaskFrame(new ModbusRtuMasterFrame (COM_PORT_2, getAddress(), 1, MODBUS_READ_ADDRESS - 1, 32, 4, 1000, 0), 
			usVal, READ_TASK);
		deque.push_back(frame);
	}
	else
	{
		for (i = 0; i < propertyCount; ++i)
		{
			if (properties[i]->propType == I7002_DISCRETE_INPUT_PROPERTY)
			{
				if (properties[i]->isCountable)
				{
					frame = new TaskFrame(new ModbusRtuMasterFrame (COM_PORT_2, getAddress(), 3, MODBUS_READ_ADDRESS - 1, 96 + propCount, 1, 1000, 0), 
						usVal, READ_TASK);
					deque.push_back(frame);
				}
				else
				{
					frame = new TaskFrame(new ModbusRtuMasterFrame (COM_PORT_2, getAddress(), 1, MODBUS_READ_ADDRESS - 1, 32 + propCount, 1, 1000, 0), 
						usVal, READ_TASK);
					deque.push_back(frame);					
				}	

				usVal += 1;
				++propCount;
			}
		}
	}
}

void I7002Device::getTasksEvents()
{
	//Discrete Output frame
	TaskFrame * frame;
	int i = 0;
	unsigned short * usVal = (unsigned short *) nativeValue;
	usVal += 4;

	for (i = 0; i < propertyCount; ++i)
	{
		if (properties[i]->propType == I7002_DISCRETE_OUTPUT_PROPERTY)
		{
			* usVal = properties[i]->getValueUnsignedLong();
			usVal += 1;
		}
	}
	usVal -= 4;

	frame = new TaskFrame(new ModbusRtuMasterFrame (COM_PORT_2, getAddress(), 15, MODBUS_READ_ADDRESS - 1, 0, 4, 1000, 0), 
		usVal, WRITE_TASK);
	deque.push_front(frame);	
}

void I7002Device::update()					//Из native value в slave modbus
{
	for (int i = 0; i < propertyCount; ++i)
	{
		unsigned short * values = (unsigned short *) nativeValue;
		int analogCount = 0, discreteInputCount = 0, discreteOutputCount = 0;

		for (int i = 0; i < propertyCount; ++i)
		{
			if (properties[i]->propType == I7002_ANALOG_INPUT_PROPERTY)
			{
				short * val = (short *) (values + analogCount);
				properties[i]->setValueFloat(* val / 1000.0f);
				++analogCount;
			}
			else if (properties[i]->propType == I7002_DISCRETE_OUTPUT_PROPERTY)
			{
				properties[i]->setValueInt(values[4 + discreteOutputCount]);
				++discreteOutputCount;
			}
			else if (properties[i]->propType == I7002_DISCRETE_INPUT_PROPERTY)
			{
				if (properties[i]->isCountable)
				{
					* (unsigned long *)properties[i]->getNativeValue() = values[8 + discreteInputCount];
				}
				else
				{
					properties[i]->setValueInt(values[8 + discreteInputCount]);
				}
				++discreteInputCount;
			}
			else if (properties[i]->propType == ERROR_PROPERTY)
			{
				properties[i]->setValueUnsignedLong(readErrror);
			}
		}

		AbstractDevice::update();
	}
}


/*!!!   I7041Device   !!!*/
I7041Device::I7041Device(unsigned short int addrr, unsigned short evMask) :
	AbstractDevice(15, addrr, evMask)
{            
	int i = 0;
	sprintf(name, "%s ch: %d", "I7041", addrr);
	deviceTypeId = I7041_DEVICE;

	for (i = 0; i < 14; i++)
	{
		properties[i] = new I7041DiscreteProperty(i);
	}

	properties[14] = new ErrorProperty();

	nativeValue = malloc(sizeof(unsigned short) * 15);
	memset(nativeValue, 0, sizeof(unsigned short) * 15);
}

I7041Device::~I7041Device(void)
{
	free(nativeValue);
}

void I7041Device::getTasksProperties()		// Задать деку свойствами 
{
	unsigned short * usVal = (unsigned short *) nativeValue;
	int propCount = 0;
	TaskFrame * frame;

	//Discrete Input frames
	bool_t isPropCountable = false_t;
	for (int i = 0; i < propertyCount; ++i)
	{
		if (properties[i]->propType == I7041_DISCRETE_INPUT_PROPERTY)
			if (properties[i]->isCountable)
				isPropCountable = true_t;
	}
	
	if (!isPropCountable)
	{
		frame = new TaskFrame(new ModbusRtuMasterFrame (COM_PORT_2, getAddress(), 1, MODBUS_READ_ADDRESS - 1, 32, 14, 1000, 0), 
			usVal, READ_TASK);
		deque.push_back(frame);
	}
	else
	{
		for (int i = 0; i < propertyCount; ++i)
		{
			if (properties[i]->propType == I7041_DISCRETE_INPUT_PROPERTY)
			{
				if (properties[i]->isCountable)
				{
					frame = new TaskFrame(new ModbusRtuMasterFrame (COM_PORT_2, getAddress(), 4, MODBUS_READ_ADDRESS - 1, 0 + propCount, 1, 1000, 0), 
						usVal, READ_TASK);
					deque.push_back(frame);
				}
				else
				{
					frame = new TaskFrame(new ModbusRtuMasterFrame (COM_PORT_2, getAddress(), 1, MODBUS_READ_ADDRESS - 1, 32 + propCount, 14, 1000, 0), 
						usVal, READ_TASK);
					deque.push_back(frame);					
				}	

				usVal += 1;
				++propCount;
			}
		}
	}
}

void I7041Device::update()					//Из native value в slave modbus
{
	for (int i = 0; i < propertyCount; ++i)
	{
		unsigned short * values = (unsigned short *) nativeValue;
		int discreteInputCount = 0;

		for (int i = 0; i < propertyCount; ++i)
		{
			if (properties[i]->propType == I7041_DISCRETE_INPUT_PROPERTY)
			{
				if (properties[i]->isCountable)
				{
					* (unsigned long *)properties[i]->getNativeValue() = values[discreteInputCount];
				}
				else
				{
					properties[i]->setValueInt(values[discreteInputCount]);
				}
				++discreteInputCount;
			}
			else if (properties[i]->propType == ERROR_PROPERTY)
			{
				properties[i]->setValueUnsignedLong(readErrror);
			}
		}

		AbstractDevice::update();
	}
}