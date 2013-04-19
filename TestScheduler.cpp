#ifdef _TESTS

#include <stdio.h>
#include <stdlib.h>
#include "TestHelpers.h"
#include "Scheduler.h"

using namespace TestHelpers;

class MockTask : public ITask
{
	public:
		MockTask (): HasTasked(false)
		{
		}
		virtual ~MockTask (){}
		virtual void DoTask()
		{
			HasTasked = true;	
		}
		bool HasTasked ;
};

void TestScheduler()
{
	MockTask m1;
	MockTask m2;
	MockTask m3;
	
	// Only m1
	Scheduler::AddTask(&m1);
	assert(m1.HasTasked==false);
	Scheduler::DoTasks();
	assert(m1.HasTasked==true);
	assert(m2.HasTasked==false);
	assert(m3.HasTasked==false);
	m1.HasTasked=false;
	
	// Empty
	Scheduler::DoTasks();
	assert(m1.HasTasked==false);
	assert(m2.HasTasked==false);
	assert(m3.HasTasked==false);
	
	// m2 and m3, no m1
	Scheduler::AddTask(&m2);
	Scheduler::AddTask(&m3);
	Scheduler::DoTasks();
	assert(m1.HasTasked==false);
	assert(m2.HasTasked==true);
	assert(m3.HasTasked==true);
	
}

#endif
