#pragma once

class ITask
{
public:
	virtual ~ITask(){}
	virtual void DoTask() = 0 ;
};
