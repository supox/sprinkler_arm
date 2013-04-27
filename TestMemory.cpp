#ifdef _TESTS

#include "TestHelpers.h"
#include "Logger.h"
#include "CVector.h"
#include "Vector.h"
#include "List.h"
#include "IrrigationList.h"
#include "TimeListenersList.h"
#include "Scheduler.h"
#include "StringBuffer.h"

#include <stdio.h>
#include <stdlib.h>

using namespace TestHelpers;

#define NUMBER_OF_TESTS 10000

static void TestList()
{
	Logger::AddLine("Testing List.", Logger::DUMP);
	List<int> l;
	for(size_t test_num = 0 ; test_num < NUMBER_OF_TESTS; test_num++)
	{
		int numbers[100];
		for(size_t insert_index = 0 ; insert_index < 3 ; insert_index++) {
			for(size_t index = 0; index < 100 ; index++) {
				numbers[index]=index;
				l.Add(&numbers[index]);
			}
			while(!l.Empty())
				l.Dequeue();
		}		
	}
	Logger::AddLine("Testing List - passed.", Logger::DUMP);
}

static void TestIrrigationList()
{
	Logger::AddLine("Testing IrrigationList.", Logger::DUMP);
	IrrigationList l;
	Irrigation irr(1000, 5, Irrigation::TIME, 100);
	for(size_t test_num = 0 ; test_num < NUMBER_OF_TESTS; test_num++)
	{
		for(size_t insert_number = 0; insert_number<30; insert_number++)
			l.Add(irr);
		while(!l.Empty())	
		{
			Irrigation& irrigation = l.GetFirst();
			l.RemoveFirst();
		}
	}
	Logger::AddLine("Testing IrrigationList - passed.", Logger::DUMP);
}

static void TestTimeManagerList()
{
	Logger::AddLine("Testing TimeListenersList.", Logger::DUMP);
	TimeListenersList l;
	
	for(size_t test_num = 0 ; test_num < NUMBER_OF_TESTS; test_num++)
	{
		for(size_t index = 0 ; index < 10 ; index++)
			l.Add(index + 100, NULL);
		while(l.GetFirst() != NULL)
			l.RemoveFirst();
	}

	Logger::AddLine("Testing TimeListenersList - passed.", Logger::DUMP);
}

class MockTask:public ITask
{
	public:
		virtual void DoTask(){}
};

static void TestScheduler()
{
	Logger::AddLine("Testing Scheduler.", Logger::DUMP);
	
	MockTask mockTask;
	for(size_t test_num = 0 ; test_num < NUMBER_OF_TESTS; test_num++)
	{
		for(size_t index = 0 ; index < 10 ; index++)
			Scheduler::AddTask(&mockTask);
		for(size_t index = 0 ; index < 10 ; index++)
			Scheduler::RemoveTask(&mockTask);
	}

	Logger::AddLine("Testing Scheduler - passed.", Logger::DUMP);
}

static void TestCVector()
{
	Logger::AddLine("Testing CVector.", Logger::DUMP);
	CVector<int> v;
	for(size_t test_num = 0 ; test_num < NUMBER_OF_TESTS; test_num++)
	{
		for(size_t index = 0; index < 10 ; index++)
			v.Add(index);
		for(size_t index = 0; index < 5 ; index++)
			v.RemoveAt(index);
		for(size_t index = 0; index < 10 ; index++)
			v.Add(index);
		v.Clear();
		for(size_t index = 0; index < 10 ; index++)
			v.Add(index);
		for(size_t insert_index = 0 ; insert_index < 3 ; insert_index++) {
			for(size_t index = 0; index < 30 ; index++)
				v.Add(index);
			for(size_t index = 0; index < 30 ; index++)
				v.RemoveAt(0);
		}
	}
	Logger::AddLine("Testing CVector - passed.", Logger::DUMP);
}

static void TestVector()
{
	Logger::AddLine("Testing Vector.", Logger::DUMP);
	Vector<int> v;
	for(size_t test_num = 0 ; test_num < NUMBER_OF_TESTS; test_num++)
	{
		for(size_t index = 0; index < 10 ; index++)
			v.Add(index);
		for(size_t index = 0; index < 5 ; index++)
			v.RemoveAt(index);
		for(size_t index = 0; index < 10 ; index++)
			v.Add(index);
		v.Clear();
		for(size_t index = 0; index < 10 ; index++)
			v.Add(index);
		for(size_t insert_index = 0 ; insert_index < 3 ; insert_index++) {
			for(size_t index = 0; index < 30 ; index++)
				v.Add(index);
			for(size_t index = 0; index < 30 ; index++)
				v.RemoveAt(0);
		}
	}
	Logger::AddLine("Testing Vector - passed.", Logger::DUMP);
}

static void TestStringBuffer()
{
	Logger::AddLine("Testing StringBuffer.", Logger::DUMP);
	
	char buffer[255] = {'B'};
	for(size_t test_num = 0 ; test_num < NUMBER_OF_TESTS; test_num++)
	{
		StringBuffer sb;
		for(size_t index = 0; index < 5 ; index++)
		{
			sb.Write(buffer + index*10, (index + 5) * 20);
		}
	}
	Logger::AddLine("Testing StringBuffer - passed.", Logger::DUMP);
}

void TestMemory()
{
	TestStringBuffer();
	TestScheduler();
	TestTimeManagerList();
	TestIrrigationList();
	TestList();
	TestVector();
	TestCVector();
}


#endif
