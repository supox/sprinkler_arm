#ifdef _TESTS

#include "Tests.h"
#include "TestHelpers.h"
#include "GsmStatusParser.h"

using namespace TestHelpers;
using namespace Communication;

void TestGsmStatusParser()
{
	assert(ParseGsmState("STATE: IP INITIAL") == GsmStates::IP_INITIAL);
	assert(ParseGsmState("STATE: IP START") == GsmStates::IP_START);
	assert(ParseGsmState("STATE: IP CONFIG") == GsmStates::IP_CONFIG);
	assert(ParseGsmState("STATE: IP IND") == GsmStates::IP_IND);
	assert(ParseGsmState("STATE: IP GPRSACT") == GsmStates::IP_GPRSACT);
	assert(ParseGsmState("STATE: TCP/UDP CONNECTING") == GsmStates::TCP_UDP_CONNECTING);
	assert(ParseGsmState("STATE: IP CLOSE") == GsmStates::IP_CLOSE);
	assert(ParseGsmState("STATE: CONNECT OK") == GsmStates::CONNECT_OK);

	const GsmStates::eGsmState state = GsmStates::IP_INITIAL;
	assert(state > GsmStates::ERROR);
	assert(state > GsmStates::UNKNOWN);
	assert(state == GsmStates::IP_INITIAL);
	assert(state < GsmStates::IP_START);
	assert(state < GsmStates::IP_CONFIG);
	assert(state < GsmStates::IP_IND);
	assert(state < GsmStates::IP_GPRSACT);
	assert(state < GsmStates::IP_STATUS);
	assert(state < GsmStates::TCP_UDP_CONNECTING);
	assert(state < GsmStates::CONNECT_OK);
	assert(state < GsmStates::IP_CLOSE);
}

#endif
