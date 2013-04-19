#pragma once
#include "ITask.h"

namespace Scheduler
{
	void DoTasks();
	void AddTask(ITask* task);
	void RemoveTask(ITask* task);
};
