#pragma once
#include <stddef.h>

namespace APNManager
{
	bool GetAPNParameters(const char* network_operator, char* apn_buffer, const size_t buffer_size);
};

