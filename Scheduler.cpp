#include "Scheduler.h"
#include "List.h"
#include "CriticalSectionHelper.h"

namespace Scheduler
{
	static List<ITask> Tasks;

	void DoTasks()
	{
		ITask* task ;
		do
		{
			{
				CriticalSectionHelper c;
				task = Tasks.Dequeue();
			}
			if(task!=NULL)
				task->DoTask();
		}
		while(task!=NULL);
	}
	
	void AddTask(ITask* task) {
		Tasks.Add(task);
	}
	
	void RemoveTask(ITask* task) {
		CriticalSectionHelper c;

		Tasks.Remove(task);
	}
};
