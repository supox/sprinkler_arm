#include "TestHelpers.h"

namespace TestHelpers
{
	void assert(bool b)
	{
		if(!b)
			while(true);
	}
};
