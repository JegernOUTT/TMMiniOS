#ifndef MODEL_H
#define MODEL_H

#include "HeadersInclude.h"

class AbstractDevice;
class CycleController;
class EEpromSettings;
class Event;
class Property;
class SlaveAddressHandler;
class SlaveFactory;

/*
///   AbstractDevice   ///
*/
class AbstractDevice
{
private:
	unsigned short int iAddress;
	unsigned short eventMask;
	unsigned short bitCountable;

protected:
	AbstractDevice(short int, unsigned short int, unsigned short);
	SlaveFactory & slave;
	unsigned long  readErrror;
	unsigned long  writeErrror;

public:
	char				name[100];
	short int			propertyCount;
	protocolType		protocolType_t;

	Property **			properties;
	Event **			events;

	deviceType			deviceTypeId;


	virtual				~AbstractDevice(void);

	short int			addProperty(Property*, short int);
	short int			deleteProperty(short int);

	unsigned short int	getAddress();
	void				setAddress(unsigned short int);

	unsigned short		getEventMask();
	void				setEventMask(unsigned short);

	unsigned short		getCountableMask();
	void				setCountableMask(unsigned short);

	unsigned int		processRead(Property *);
	unsigned int		processWrite(Property *);
	virtual unsigned int getVal(Property *) = 0;
	virtual unsigned int setVal(Property *) = 0;
	void				writeDataToSlave(Property*);
	void				logProperty(Property*);

	void				eventsProcess();
	void				constructEvents();
};

class BskzhDevice:
	public AbstractDevice
{
public:
	BskzhDevice(unsigned short int, unsigned short);
	virtual ~BskzhDevice(void);
	unsigned int getVal(Property*) 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
	unsigned int setVal(Property*) 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};

class X304Device:
	public AbstractDevice
{
private:
	unsigned short err;

public:
	X304Device(unsigned short int, unsigned short);
	virtual ~X304Device(void);
	unsigned int getVal(Property*) 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
	unsigned int setVal(Property*) 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
	void loadSavedCountsFromEEPROM();
	void saveCountsToEEPROM(Property * prop);
};

class I7002Device:
	public AbstractDevice
{
public:
	I7002Device(unsigned short int, unsigned short);
	virtual ~I7002Device(void);
	unsigned int getVal(Property*) 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
	unsigned int setVal(Property*) 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};

class I7041Device:
	public AbstractDevice
{
public:
	I7041Device(unsigned short int, unsigned short);
	virtual ~I7041Device(void);
	unsigned int getVal(Property*) 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
	unsigned int setVal(Property*) 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};



/*
///   CycleController   ///
*/
class CycleController
{
private:
	unsigned short	deviceMask;
	unsigned short deviceMaskOld;
	void	readDeviceMask();

	PropertiesStruct * properties;
	PropertiesStruct * properties_old;
	void	readProperties();
	bool_t isPropertyNew();

	void checkMask();
	void propertiesToSlave();
	void next();
	void eventsProcess();
	void readNonModbusDevices();

private:
	void constructNewDevices();

private:
	CycleController(void);
	~CycleController(void);
	static CycleController * cycleController;
	SlaveFactory & slave;
	
	AbstractDevice ** devices;

	int deviceCount;
	int propertyCount;
	unsigned long timeTick;
	unsigned long attemptToLoad;
	int workFlag;
	bool_t writeProcess;
	bool_t readProcess;

public:
	static	CycleController& getInstance();

	void	init();
	void	read();

	AbstractDevice ** getDevices(unsigned short & deviceCount);
};



/*
///   EEpromSettings   ///
*/
class EEpromSettings
{
private:
	EEpromSettings(void);
	~EEpromSettings(void);
public:
	static void saveToEeprom(const PropertiesStruct *, const unsigned short);
	static PropertiesStruct * loadFromEeprom(unsigned short *);
};



/*
///   Event   ///
*/
class Event
{
protected:
	AbstractDevice * device;

public:
	Event(AbstractDevice *);
	virtual ~Event(void);
	virtual void process() = 0;
};

class SwitchOnSignalEvent : public Event
{
private:
	const int channelNumberFrom;
	const int channelNumberTo;

public:
	SwitchOnSignalEvent(AbstractDevice *);
	~SwitchOnSignalEvent(void);
	void process() 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};



/*
///   Property   ///
*/
class Property
{
private:
	void * nativeValue;

protected:
	virtual void calculate(){};

public:
	Property(void);
	virtual ~Property(void);
	char name[100];
	propertyType propType;
	unsigned int readErr;
	unsigned int writeErr;
	
	float			getValueFloat();
	short int		getValueInt();
	long			getValueLong();
	unsigned long	getValueUnsignedLong();
	bool_t			getValueBool();

	void			setValueFloat(float);
	void			setValueInt(short int);
	void			setValueLong(long);
	void			setValueUnsignedLong(unsigned long);
	void			setValueBool(bool_t);

	void *			getNativeValue();

	SlaveInformation slaveInformation;
	uValue value;
	uValue valueOld;
	countType sigType;
	bool_t isCountable;
};

class BskzhMass1Property :
	public Property
{
public:
	BskzhMass1Property(void);
	virtual ~BskzhMass1Property(void);
	void calculate() 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};

class BskzhMass2Property :
	public Property
{
public:
	BskzhMass2Property(void);
	virtual ~BskzhMass2Property(void);
	void calculate() 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};

class X304AnalogProperty :
	public Property
{
public:
	short int iChannel;
	X304AnalogProperty(short int);
	virtual ~X304AnalogProperty(void);
	void calculate() 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};

class X304DiscreteProperty :
	public Property
{
public:
	unsigned long	lastValue;
	unsigned int	timeToSave;
	unsigned int	velocityValue;
	
	unsigned long	timeTickCheck;
	unsigned long	timeTickSave;

public:
	short int iChannel;
	X304DiscreteProperty(short int);
	virtual ~X304DiscreteProperty(void);
	void calculate()
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};

class X304DiscreteOutputProperty :
	public Property
{
public:
	short int iChannel;
	X304DiscreteOutputProperty(short int);
	virtual ~X304DiscreteOutputProperty(void);
	void calculate()
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};

class I7002DiscreteInputProperty :
	public Property
{
public:
	short int iChannel;
	I7002DiscreteInputProperty(short int);
	virtual ~I7002DiscreteInputProperty(void);
	void calculate() 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};

class I7002DiscreteOutputProperty :
	public Property
{
public:
	short int iChannel;
	I7002DiscreteOutputProperty (short int);
	virtual ~I7002DiscreteOutputProperty (void);
	void calculate()
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};

class I7002AnalogProperty :
	public Property
{
public:
	short int iChannel;
	I7002AnalogProperty (short int);
	virtual ~I7002AnalogProperty (void);
	void calculate() 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};

class I7041DiscreteProperty :
	public Property
{
public:
	short int iChannel;
	I7041DiscreteProperty (short int);
	virtual ~I7041DiscreteProperty (void);
	void calculate() 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};

class ErrorProperty :
	public Property
{
public:
	ErrorProperty ();
	virtual ~ErrorProperty (void);
	void calculate() 
#ifdef MVS_2012_32BIT 
		override 
#endif
		;
};




/*
///   SlaveAddressHandler   ///
*/
class SlaveAddressHandler
{
private:
	SlaveAddressHandler(void);
	~SlaveAddressHandler(void);
public:
	static void setSlaveInformation(AbstractDevice *, SlaveInformation);
};



/*
///   SlaveFactory   ///
*/
class SlaveFactory
{
private:
	static SlaveFactory * slaveFactoryPointer;
	SlaveFactory(void);	
	~SlaveFactory(void);
	unsigned char	iMemory_DI[REGISTER_MAX];
	unsigned char	iMemory_DO[REGISTER_MAX];
	unsigned short	iMemory_AI[REGISTER_MAX];
	unsigned short	iMemory_AO[REGISTER_MAX];

public:
	static SlaveFactory & getSlaveFactoryPointer();

	void setDisreteInput	(SlaveInformation, const uValue &);
	void setCoils			(SlaveInformation, const uValue &);
	void setInputRegisters	(SlaveInformation, const uValue &);
	void setHoldingRegisters(SlaveInformation, const uValue &);

	uValue getDisreteInput	(SlaveInformation, size_t);
	uValue getCoils			(SlaveInformation, size_t);
	uValue getInputRegisters(SlaveInformation, size_t);
	uValue getHoldingRegisters(SlaveInformation, size_t);

	void registerClear		(SlaveInformation, int);
};



#ifdef VC_1992_16BIT
/*
///   Time   ///
*/

class Time
{
private:
	Time();
	~Time();
	static TIME_DATE * timedate;

public: 
	static void timeUpdate();
};
#endif

#endif MODEL_H