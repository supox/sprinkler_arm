#pragma once
#include "StringBuffer.h"

namespace Communication
{
	bool GetWebPage(const char* url, StringBuffer& sb);
	bool PostWebPage(const char* url, StringBuffer& request, StringBuffer& response);
};
