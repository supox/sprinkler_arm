#ifdef _TESTS
#include "TestHelpers.h"
using namespace TestHelpers;

#include "TimeListenersList.h"
#include "TimeManager.h"

static void TestTimeListenersList()
{
	TimeListenersList list;
		
	list.Add(5, NULL);
	list.Add(4, NULL);
	list.Add(6, NULL);
	list.Add(3, NULL);
	
	if(list.GetFirst()->Time != 3)
		while(true);
	list.RemoveFirst();
	if(list.GetFirst()->Time != 4)
		while(true);
	
	bool ret = list.Remove(5,NULL);
	if(!ret)
		while(true);
	
	if(list.GetFirst()->Time != 4)
		while(true);
	list.RemoveFirst();
	if(list.GetFirst()->Time != 6)
		while(true);
	
	ret = list.Remove(5,NULL);
	if(ret)
		while(true);
	
	ret = list.Remove(6,NULL);
	if(!ret)
		while(true);

	// Check clear
	list.Add(5, NULL);
	list.Add(4, NULL);
	list.Add(6, NULL);
	list.Add(3, NULL);
	list.Clear();
	if(list.GetFirst()!=NULL)
		while(true);

	list.Add(5, NULL);
	if(list.GetFirst()==NULL)
		while(true);
}

class MockTimeListener : public ITimeListener
{
	public:
		MockTimeListener () : HasNotified(false)
		{
		}
		virtual void TimeNotification()
		{
			HasNotified = true;
		}

		bool HasNotified;
};


static void TestTimeManager()
{
	MockTimeListener mockListener1;
	MockTimeListener mockListener2;
	
	TimeManager *manager = TimeManager::GetTimeManager();
	assert(manager == NULL);
	
	manager->SetSystemTime(1000);
	assert(manager->GetSystemTime() == 1000);
	
	manager->NotifyAt(&mockListener1, 1005);
	manager->NotifyAt(&mockListener2, 1010);
	manager->NotifyAt(&mockListener1, 1015);
	
	manager->NotifyListeners();
	
	if(mockListener1.HasNotified)
		while(true);
	if(mockListener2.HasNotified)
		while(true);
	
	manager->SetSystemTime(1005);
	manager->NotifyListeners();
	if(!mockListener1.HasNotified)
		while(true);
	if(mockListener2.HasNotified)
		while(true);
	mockListener1.HasNotified = false;

	// Check again, should not notify twice
	manager->NotifyListeners();
	if(mockListener1.HasNotified)
		while(true);
	if(mockListener2.HasNotified)
		while(true);

	manager->SetSystemTime(1007);
	manager->NotifyListeners();
	if(mockListener1.HasNotified)
		while(true);
	if(mockListener2.HasNotified)
		while(true);

	manager->SetSystemTime(1010);
	manager->NotifyListeners();
	if(mockListener1.HasNotified)
		while(true);
	if(!mockListener2.HasNotified)
		while(true);
	mockListener2.HasNotified = false;

	manager->SetSystemTime(1015);
	manager->NotifyListeners();
	if(!mockListener1.HasNotified)
		while(true);
	if(mockListener2.HasNotified)
		while(true);
	mockListener1.HasNotified = false;
}

void TestTimes()
{
	TestTimeManager();
	TestTimeListenersList();
}

#endif
