#include "Model.h"

#ifdef VC_1992_16BIT

TIME_DATE * Time::timedate = NULL;

Time::Time()
{
}

Time::~Time()
{
}

void Time::timeUpdate()
{
	 GetTimeDate(timedate);	
}

#endif VC_1992_16BIT

