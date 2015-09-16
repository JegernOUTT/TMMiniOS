#include "Model.h"

EEpromSettings::EEpromSettings(void)
{
}

EEpromSettings::~EEpromSettings(void)
{
}

void EEpromSettings::saveToEeprom(const PropertiesStruct* properties, const unsigned short mask)
{
#ifdef MVS_2012_32BIT
	unsigned short tmpMask = mask;
	int count = 0;

	FILE * fp;
	fp = fopen("tmp.b", "wb");

	fwrite((void *)&tmpMask, sizeof(tmpMask), 1, fp);
	while (tmpMask > 0)
	{
		if (tmpMask & 1)
		{
			fwrite((void * )(properties + count), sizeof(PropertiesStruct), 1, fp);
		}

		++count;
		tmpMask >>= 1;
	}

	fclose(fp);
#else VC_1992_16BIT
	EE_WriteEnable();        
	
	unsigned short tmpMask = mask;
	unsigned int count = 0, address = 0;      
                
    EE_MultiWrite_A(3, address, sizeof(unsigned short), (char * ) & tmpMask); 
    address += sizeof(unsigned short);
                                                                              
	while (tmpMask > 0)
	{
		if (tmpMask & 1)
		{
			EE_MultiWrite_A(3, address, sizeof(PropertiesStruct), (char * ) (properties + count)); 
    		address += sizeof(PropertiesStruct);
		}

		++count;
		tmpMask >>= 1;
	}
           
	EE_WriteProtect(); 

#endif
}
	
PropertiesStruct * EEpromSettings::loadFromEeprom(unsigned short * maskOut)
{
#ifdef MVS_2012_32BIT 
	PropertiesStruct * properties = new PropertiesStruct[MAX_DEVICE_COUNT];
	memset(properties, 0, sizeof(PropertiesStruct) * MAX_DEVICE_COUNT);
	int count = 0;
	unsigned short tmpMask = 0;

	FILE * fp;
	fp = fopen("tmp.b", "rb");

	fread(&tmpMask, sizeof(tmpMask), 1, fp);
	* maskOut = tmpMask;

	while (* maskOut > 0)
	{
		if (* maskOut & 1)
		{
			fread((void * )(properties + count), sizeof(PropertiesStruct), 1, fp);
		}

		++count;
		* maskOut >>= 1;
	}

	* maskOut = tmpMask;

	fclose(fp);

	return properties;

#else VC_1992_16BIT
 	PropertiesStruct * properties = new PropertiesStruct[MAX_DEVICE_COUNT];
	memset(properties, 0, sizeof(PropertiesStruct) * MAX_DEVICE_COUNT);
	unsigned int count = 0, address = 0;
	unsigned short tmpMask = 0;
	
	EE_MultiRead(3, address, sizeof(unsigned short), (char * ) & tmpMask);        
	address += sizeof(unsigned short);
	* maskOut = tmpMask;

	while (* maskOut > 0)
	{
		if (* maskOut & 1)
		{
			EE_MultiRead(3, address, sizeof(PropertiesStruct), (char * ) (properties + count)); 
    		address += sizeof(PropertiesStruct);
		}

		++count;
		* maskOut >>= 1;
	}              
    
    * maskOut = tmpMask;  
	return properties;
#endif
}