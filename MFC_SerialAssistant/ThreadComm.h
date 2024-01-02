#pragma once
#include "Thread.h"

class CThreadComm:public CThread
{
public:
	CThreadComm();
	~CThreadComm();

public:
	virtual void run();

};

