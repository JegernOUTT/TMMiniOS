#ifndef HEADERSINCLUDE_H
#define HEADERSINCLUDE_H

#define	_CRT_SECURE_NO_WARNINGS

#ifdef VC_1992_16BIT
	#include "lib/7188e.h"
	#include "lib/mbtcp.h"
	#include "lib/tcpip32.h"
	#include "lib/vxcomm.h"
	#include "lib/X304.h"
	#include "math.h"       
	#include "STDLIB.h"     
	#include <stdio.h>      
#else MVS_2012_32BIT
	#include <stdio.h>
	#include <iostream>
	#include <ctime>
#endif
                                                    
const int	COM_PORT_1 =				1;
const int	COM_PORT_2 =				2;
const int	TIME_OUT =					1000;
const int	MAX_DEVICE_COUNT =			16;		
const int	DEVICE_MASK_ADDRESS =		1000;
const int	ATTEMPTS_COUNT =			1500;
const int	MODBUS_READ_ADDRESS =		900;
#ifdef MVS_2012_32BIT
const int	WAIT_PERIOD =				0;	
#else VC_1992_16BIT
const int	WAIT_PERIOD =				200;
#endif
const int	MALLOC_NATIVE_VALUE_BYTES = 16;
const int	REGISTER_MAX =				2000;
#define POP_ERROR 0xD5555555
             
typedef enum { false_t = 0, true_t = 1} bool_t;  
typedef unsigned int size_t;

enum deviceType
{
	BSKZH_DEVICE		= 1,
	I7002_DEVICE		= 2,
	X304_DEVICE			= 3,
	I7041_DEVICE		= 4
};

enum propertyType
{
	BSKZH_MASS1_PROPERTY			= 100,
	BSKZH_MASS2_PROPERTY			= 101,
	X304_ANALOG_PROPERTY			= 102,
	X304_DISCRETE_PROPERTY			= 103,
	X304_DISCRETE_OUTPUT_PROPERTY	= 104,
	I7002_DISCRETE_INPUT_PROPERTY	= 105,
	I7002_DISCRETE_OUTPUT_PROPERTY	= 106,
	I7002_ANALOG_INPUT_PROPERTY		= 107,
	I7041_DISCRETE_INPUT_PROPERTY	= 108,
	ERROR_PROPERTY					= 109
};

enum eventType
{
	SWITCH_ON_SIGNAL_EVENT			= 0
};

enum regType
{
	DISCRETE_INPUTS		= 0,
	COILS				= 1,
	INPUT_REGISTERS		= 3,
	HOLDING_REGISTERS	= 4
};

enum protocolType
{
	MODBUS_PROTOCOL		= 1,
	XBOARD_PROTOCOL		= 2
};

enum countType
{
	COUNTABLE_PROPERTY		= 1,
	NON_COUNTABLE_PROPERTY	= 2
};

enum taskType
{
	READ_TASK	= 50,
	WRITE_TASK	= 51
};

struct SlaveInformation
{
	SlaveInformation(regType registType, long registNumber)
	{
		memset(this, 0, sizeof(* this));
		registerType = registType;
		registerNumber = registNumber;
	};
	regType registerType;
	long registerNumber;
};

struct PropertiesStruct
{
	PropertiesStruct(short devT = 0, short modAddress = 0, short startAddress = 0, short ext_1 = 0, short ext_2 = 0)
	{
		memset(this, 0, sizeof(* this));

		deviceType = devT;
		modbusAddress = modAddress;
		startSlaveAddress = startAddress;
		extra_1 = ext_1;
		extra_2 = ext_2;
	}

	short deviceType;
	short modbusAddress;
	short startSlaveAddress;
	unsigned short extra_1;
	unsigned short extra_2;
};

struct ModbusRtuMasterFrame
{
	ModbusRtuMasterFrame (int com, int modbusAddr, int function, int to, int from, int howMuch, int tmout, int mode)
	{
		memset(this, 0, sizeof(* this));
		comAddress = com;
		modbusAddress = modbusAddr;
		functionCode = function;
		toAddr = to;
		fromAddr = from;
		registerCountToRead = howMuch;
		timeout = tmout;
		blockedMode = mode;
	}
	~ModbusRtuMasterFrame(){};

	int comAddress;
	int modbusAddress;
	int functionCode;
	int toAddr;
	int fromAddr;
	int registerCountToRead;
	int timeout;
	int blockedMode;
} ;

union uValue
{
	uValue(unsigned long ulval_param = 0)
	{
		memset(this, 0, sizeof(* this));
		ulVal = ulval_param;
	}
	long lVal;
	short int iVal[2];
	bool_t bVal[4];
	float fVal;
	unsigned long ulVal;
};


#endif HEADERSINCLUDE_H
