#include "GsmStatusParser.h"
#include <String.h>

namespace Communication
{
	using namespace GsmStates;
	
	eGsmState ParseGsmState(const char* state_buffer)
	{
		size_t str_len = strlen(state_buffer);
		if(strncmp(state_buffer, "STATE: ", 7) != 0 )
			return UNKNOWN;
		state_buffer+=7;
		str_len-=7;
		
		
		if(strncmp(state_buffer, "IP ", 3) == 0 ) {
			state_buffer+=3;
			str_len-=3;
			if(strncmp(state_buffer, "INITIAL", str_len) == 0) return IP_INITIAL;
			if(strncmp(state_buffer, "START", str_len) == 0) return IP_START;
			if(strncmp(state_buffer, "CONFIG", str_len) == 0) return IP_CONFIG;
			if(strncmp(state_buffer, "IND", str_len) == 0) return IP_IND;
			if(strncmp(state_buffer, "GPRSACT", str_len) == 0) return IP_GPRSACT;
			if(strncmp(state_buffer, "STATUS", str_len) == 0) return IP_STATUS;
			if(strncmp(state_buffer, "CLOSE", str_len) == 0) return IP_CLOSE;
		}
		else {
			if(strncmp(state_buffer, "TCP/UDP CONNECTING", str_len) == 0) return TCP_UDP_CONNECTING;
			if(strncmp(state_buffer, "CONNECT OK", str_len) == 0) return CONNECT_OK;
		}
		
		return UNKNOWN;
	}
	
};
