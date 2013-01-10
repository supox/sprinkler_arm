#ifdef _TESTS

#include "IrrigationList.h"
#include "TestHelpers.h"
using namespace TestHelpers;

void TestIrrigationList()
{
	IrrigationList list;
	int t1 = 2, t2 = 8, t3 = 1, t4 = 10;
	
	Irrigation i1(t1, 2, Irrigation::TIME, 4);
	Irrigation i2(t2, 10, Irrigation::TIME, 232);
	Irrigation i3(t3, 7, Irrigation::TIME, 1);
	Irrigation i4(t4, 1, Irrigation::TIME, 40);
	
	assert(list.Empty());
	list.Add(i1);
	assert(!list.Empty());
	list.Add(i2);
	list.Add(i3);
	list.Add(i4);
	assert(!list.Empty());
	
	assert(list.GetFirst() == i3);
	list.RemoveFirst();
	assert(!list.Empty());
	assert(list.GetFirst() == i1);	

	bool ret = list.Remove(i2);
	assert(!list.Empty());	
	assert(ret);
		
	assert(list.GetFirst() == i1);
	list.RemoveFirst();
	assert(!list.Empty());	
	assert(list.GetFirst() == i4);
	assert(!list.Empty());
	
	ret = list.Remove(i1);
	assert(!ret);

	ret = list.Remove(i4);
	assert(ret);
	assert(list.Empty());
	
	// Check clear
	list.Add(i2);
	list.Add(i3);
	list.Add(i4);
	list.Add(i1);
	list.Clear();
	assert(list.Empty());

	list.Add(i1);
	assert(!list.Empty());
	assert(list.GetFirst()==i1);
}

#endif
