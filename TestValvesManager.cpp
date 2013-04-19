#ifdef _TESTS
#include "TestHelpers.h"
using namespace TestHelpers;

#include "ValvesManager.h"
#include "Valf.h"
#include "TimeManager.h"

class MockValf : public Valf
{
public:
	MockValf(const int id, const int port_index) : Valf(id, port_index), state(false)
	{
	}
	virtual ~MockValf(){}
		
	virtual bool SetState(const bool is_open)
	{
		state = is_open;
		return true;
	}	
	bool state ;
};

void TestValvesManager()
{
	ValvesManager manager;
	Vector<ValfPtr> valves;
	Vector<Irrigation> irrigations;

	int id1 = 1, id2 = 2;
	int port_index1= 3, port_index2 = 4;
	
	MockValf* mock_valf1 = new MockValf(id1, port_index1);
	MockValf* mock_valf2 = new MockValf(id2, port_index2);
	ValfPtr p_valf1(mock_valf1);
	ValfPtr p_valf2(mock_valf2);
	valves.Add(p_valf1);
	valves.Add(p_valf2);


	// Test 1 - nothing to do
	manager.Update(valves, irrigations);
	assert( mock_valf1->state == false );
	assert( mock_valf2->state == false );
	
	// Test 2 - open valf 1 at second 10 and close after 12 seconds
	Irrigation i1(10, id1, Irrigation::TIME, 12);
	irrigations.Add(i1);
	
	TimeManager::SetSystemTime(15);
	manager.Update(valves, irrigations);
	assert( mock_valf1->state == true );
	assert( mock_valf2->state == false );

	TimeManager::SetSystemTime(23);
	manager.TimeNotification(TimeManager::GetSystemTime());
	assert( mock_valf1->state == false );
	assert( mock_valf2->state == false );

	// Test 3 - several irrigations
	irrigations.Clear();
	Irrigation i2(5, id1, Irrigation::TIME, 5);
	Irrigation i3(6, id2, Irrigation::TIME, 2);
	Irrigation i4(11, id1, Irrigation::TIME, 5);
	Irrigation i5(11, id2, Irrigation::TIME, 2);
	irrigations.Add(i2);
	irrigations.Add(i3);
	irrigations.Add(i4);
	irrigations.Add(i5);
	
	TimeManager::SetSystemTime(4);
	manager.Update(valves, irrigations);
	assert( mock_valf1->state == false );
	assert( mock_valf2->state == false );

	TimeManager::SetSystemTime(5);
	manager.TimeNotification(TimeManager::GetSystemTime());
	assert( mock_valf1->state == true );
	assert( mock_valf2->state == false );

	TimeManager::SetSystemTime(6);
	manager.TimeNotification(TimeManager::GetSystemTime());
	assert( mock_valf1->state == true );
	assert( mock_valf2->state == true );

	TimeManager::SetSystemTime(8);
	manager.TimeNotification(TimeManager::GetSystemTime());
	assert( mock_valf1->state == true );
	assert( mock_valf2->state == false );

	TimeManager::SetSystemTime(10);
	manager.TimeNotification(TimeManager::GetSystemTime());
	assert( mock_valf1->state == false );
	assert( mock_valf2->state == false );

	TimeManager::SetSystemTime(11);
	manager.TimeNotification(TimeManager::GetSystemTime());
	assert( mock_valf1->state == true );
	assert( mock_valf2->state == true );

	TimeManager::SetSystemTime(13);
	manager.TimeNotification(TimeManager::GetSystemTime());
	assert( mock_valf1->state == true );
	assert( mock_valf2->state == false );

	TimeManager::SetSystemTime(16);
	manager.TimeNotification(TimeManager::GetSystemTime());
	assert( mock_valf1->state == false );
	assert( mock_valf2->state == false );

	manager.ReportIrrigationLog("mockurl");
}

#endif
