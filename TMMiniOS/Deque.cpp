#include "Model.h"

Deque * Deque::deque = NULL;

Deque::Deque(long elCnt) :
	errFrame(new ModbusRtuMasterFrame(POP_ERROR, POP_ERROR, POP_ERROR, 
	POP_ERROR, POP_ERROR, POP_ERROR, POP_ERROR, POP_ERROR))
{
	maxElementCount = elCnt;
	memAlloc = new TaskFrame * [maxElementCount * 2]();
	head = memAlloc;
	head += maxElementCount;
	back = head;
	elementCount = 0;
	err = new unsigned long;
	* err = 0xD5555555;
}

Deque::~Deque(void)
{
	delete memAlloc;
	delete err;
}

Deque & Deque::operator=(Deque & deque)
{   
	int i = 0;
	
	for (i = 0; i < elementCount / 2; ++i)
	{
		this->push_back(deque[i]);
	}

	for (i = elementCount / 2; i < elementCount; ++i)
	{
		this->push_front(deque[i]);
	}

	return *this;
}

TaskFrame * Deque::operator[](const int num) const
{
	if (num < elementCount && num >= 0)
		return * (back + num);
	else 
	{
		return new TaskFrame(errFrame, (void *) err, READ_TASK);
	}
}

bool_t Deque::isEmpty()
{
	if (elementCount == 0)
	{
		stabilize();
		return true_t;
	}
	else
		return false_t;
}

int Deque::push_back(TaskFrame * const element)
{
	isEmpty();

	if ((back - 1) >= memAlloc)
	{
		back -= 1;
		* back = element;
		++elementCount;
		return 0;
	}
	else
	{
		return -1;
	}
}

int Deque::push_front(TaskFrame * const element)  
{
	isEmpty();

	TaskFrame ** addr = memAlloc;
	addr += maxElementCount * 2;
	
	if ((head + 1) <= addr)
	{
		*head = element;
		head += 1;
		++elementCount;
		return 0;

	}
	else
	{
		return -1;
	}
}

TaskFrame * Deque::pop_back()
{

	if (!isEmpty())
	{
		if (back != head)
			back += 1;
		--elementCount;
		TaskFrame * returnVal = * (back - 1);
		* (back - 1) = NULL;
		return returnVal;
	}
	else 
	{
		return new TaskFrame(errFrame, (void *) err, READ_TASK);
	}
}

TaskFrame * Deque::pop_front()
{
	if (!isEmpty())
	{
		head -= 1;
		--elementCount;
		TaskFrame * returnVal = * head;
		* head = NULL;
		return returnVal;
	}
	else 
	{
		return new TaskFrame(errFrame, (void *) err, READ_TASK);
	}
}

int Deque::stabilize()
{
	head = memAlloc;
	head += maxElementCount;
	back = head;
	/*
	!	Make an recounting and moving the array to center of allocated memory
	*/
	return 0;
}

long Deque::getElementCount()
{
	return elementCount;
}

long Deque::getMaxElementCount()
{
	return maxElementCount;
}

int Deque::clearDeque_unsafe()
{
	for (int i = 0; i < elementCount; ++i)
	{
		delete * (memAlloc + i);
		* (memAlloc + i) = NULL;
	}
	
	elementCount = 0;

	head = memAlloc;
	head += maxElementCount;
	back = head;

	return 0;
}

Deque & Deque::getInstance(long maxCount)
{
	if (deque == NULL)
	{
		deque = new Deque(maxCount);
	}
	return * deque;
}