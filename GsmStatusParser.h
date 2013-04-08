#pragma once

namespace Communication
{
	namespace GsmStates{
		enum eGsmState{ 
			ERROR = 0,
			UNKNOWN,
			
			IP_INITIAL,
			IP_START,
			IP_CONFIG,
			IP_IND,
			IP_GPRSACT,			
			IP_STATUS,
			CONNECT_OK,
			TCP_UDP_CONNECTING,
			IP_CLOSE
			
		};
	}
	GsmStates::eGsmState ParseGsmState(const char* state_buffer);
};
