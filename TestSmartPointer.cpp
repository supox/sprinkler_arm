#ifdef _TESTS

#include "SmartPointer.h"

class MockClass
{
public:
	MockClass()
	{
	}
	~MockClass()
	{
		DeleteCounts++;
	}
	static int DeleteCounts;
};
int MockClass::DeleteCounts=0;
typedef SmartPointer<MockClass> MockClassPtr;

void assert(bool b)
{
	if(!b)
		while(true);
}

void TestSmartPointer()
{
	// Test normal:
	MockClass::DeleteCounts = 0;
	{
		MockClassPtr p(new MockClass());
	}
	assert(MockClass::DeleteCounts == 1);
		
	// Test reference:
	MockClass::DeleteCounts = 0;
	{
		MockClassPtr p1(new MockClass());
		{
			MockClassPtr p2 = p1;
		}
		assert(MockClass::DeleteCounts == 0);
	}
	assert(MockClass::DeleteCounts == 1);
	
	// Test assignment:
	MockClass::DeleteCounts = 0;
	{
		MockClassPtr p1(new MockClass());
		MockClassPtr p2 = p1;
	}
	assert(MockClass::DeleteCounts == 1);

	// Test twice:
	MockClass::DeleteCounts = 0;
	{
		MockClassPtr p1(new MockClass());
		MockClassPtr p2(new MockClass());
	}
	assert(MockClass::DeleteCounts == 2);
	
	// Test nulls:
	MockClass::DeleteCounts = 0;
	{
		MockClassPtr p();
	}
	assert(MockClass::DeleteCounts == 0);
	
	// Test operator
	MockClass::DeleteCounts = 0;
	{
		MockClass *p = new MockClass();
		MockClassPtr sp(p);
		assert( &(*sp) == p);
		assert( sp.get() == p);
	}
	assert(MockClass::DeleteCounts == 1);
}

#endif
