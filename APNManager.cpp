#include "APNManager.h"
#include <string.h>

#define AT_PREFIX "AT+CSTT="
namespace APNManager
{
	bool GetAPNParameters(const char* network_operator, char* apn_buffer, const size_t buffer_size)
	{
		if(strcmp(network_operator, "Orange") == 0)
		{
			strncpy(apn_buffer, AT_PREFIX "\"uinternet\",\"orange\",\"orange\"", buffer_size);
			return true;
		}
		if(strcmp(network_operator, "Cellcom") == 0)
		{
			strncpy(apn_buffer, AT_PREFIX "\"internetg\",\"guest\",\"guest\"", buffer_size);
			return true;
		}
		if(strcmp(network_operator, "Pelephone") == 0)
		{
			strncpy(apn_buffer, AT_PREFIX "\"internet.pelephone.net.il\",\"pcl@3g\",\"pcl\"", buffer_size);
			return true;
		}
		if(strcmp(network_operator, "Hot Mobile") == 0)
		{
			strncpy(apn_buffer, AT_PREFIX "\"net.hotm\",\"\",\"\"", buffer_size);
			return true;
		}
		if(strcmp(network_operator, "Golan Telecom") == 0)
		{
			strncpy(apn_buffer, AT_PREFIX "\"internet.golantelecom.net.il\",\"\",\"\"", buffer_size);
			return true;
		}
		
		return false;
	}
};
