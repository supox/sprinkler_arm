#include "Scheduler.h"
#include "List.h"

namespace Scheduler
{
	static List<ITask> Tasks;

	void DoTasks()
	{
		for( ITask* task = Tasks.Dequeue() ; task != NULL ; task = Tasks.Dequeue())
		{
			task->DoTask();
		}
	}
	
	void AddTask(ITask* task) {
		Tasks.Add(task);
	}
	
	void RemoveTask(ITask* task) {
		Tasks.Remove(task);
	}
};
