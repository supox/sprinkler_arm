#include "Communication.h"
#include "CommunicationTimeouts.h"
#include "TimeManager.h"
#include "HttpParser.h"
#include "GsmModem.h"
#include "GsmStatusParser.h"
#include "APNManager.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define WEB_SERVER "tranquil-ravine-7080.herokuapp.com"

namespace Communication
{
	using namespace GsmStates;

	static bool send_command(const char* command);
	static bool send_command_with_any_response(const char* command);
	static bool send_command_with_response(const char* command, const char* response);
	static bool wait_for_response(const char* response, size_t timeout);
	static bool wait_for_any_response(size_t timeout_ms);
	static bool gsm_send_post_data(const char* server, const char* url, StringBuffer& request, StringBuffer& response, bool post);
	static bool gsm_send_post_data_with_retry(const char* server, const char* url, StringBuffer& request, StringBuffer& response, bool post);

	static bool gsm_init();
	static bool send_apn_parameters();
	static bool setup_gprs();
	static bool send_request(const char* server, const char* url, StringBuffer& request, bool post);
	static bool read_response(StringBuffer& response);
	static bool send_server_address(const char *server);
	static void send_headers(const char *server, const char* url, const size_t data_size = 0, bool post = false); 
	static bool wait_for_headers_ready(size_t timeout_ms);
	static bool gsm_shut_down_gprs();
	static GsmStates::eGsmState get_current_gsm_state();
	
	static GsmModem& modem = GsmModem::GetModem();

	bool GetWebPage(const char* url, StringBuffer& sb)
	{
		StringBuffer request;
		return gsm_send_post_data_with_retry(WEB_SERVER, url, request, sb, false);
	}
	
	bool PostWebPage(const char* url, StringBuffer& request, StringBuffer& response)
	{
		return gsm_send_post_data_with_retry(WEB_SERVER, url, request, response, true);
	}
	
	bool gsm_send_post_data_with_retry(const char* server, const char* url, StringBuffer& request, StringBuffer& response, bool post) {
		bool ret = false;
		for(int retry_number = 0; !ret && retry_number < NUMBER_OF_COMMUNICATION_RETRIES; retry_number++)
		{
			ret = gsm_send_post_data(server, url, request, response, post);
		}
		return ret;
	}
	
	// Main communication flow
	bool gsm_send_post_data(const char* server, const char* url, StringBuffer& request, StringBuffer& response, bool post) {
		send_command("\r\n\r\n"); // Clear old commands from modem's buffer

		GsmStates::eGsmState state = get_current_gsm_state();
		switch(state)
		{
			default:
				if( !gsm_init() )
					return false;
				state = get_current_gsm_state();
				break;
			case IP_START:
			case IP_CONFIG:
			case IP_STATUS:
			case IP_CLOSE:
				// No need to init
				break;
		}

		switch(state)
		{
			case IP_CONFIG:
			case IP_STATUS:
				// No need to setup gprs.
				break;
			default:
				if( !setup_gprs() )
					return false;
					break;
		}		
		
		if(!send_request(server, url, request, post))
			return false;
		
		bool ret = read_response(response);
		
		// close data
		if(get_current_gsm_state() != GsmStates::IP_CLOSE)
			send_command_with_response("AT+CIPCLOSE", "CLOSE OK");
		
		modem.ClearBuffer();

		return ret;
	}

	bool setup_gprs()
	{
		// TODO - AT+CSQ, AT+CREG?, AT+CGATT?, 
		GsmStates::eGsmState state = get_current_gsm_state();
		if(state == GsmStates::IP_START ) {
			if(!send_command_with_response("AT+CIICR","OK"))
				return false;
		}
		if(!send_command_with_any_response("AT+CIFSR"))
			return false;
		if(!send_command_with_response("AT+CDNSORIP=1","OK"))
			return false;
		
		return true;
	}
	
	bool send_request(const char* server, const char* url, StringBuffer& request, bool post)
	{
		// Make sure modem is in the right state
		switch(get_current_gsm_state())
		{
			case GsmStates::IP_STATUS :
			case GsmStates::IP_CLOSE :
				break;
			default:
				return false;
		}
		
		if(!send_server_address(server))
			return false;
		
		if(!wait_for_response("CONNECT OK", CONNECTION_TIME_OUT))
			return false;

		send_command("AT+CIPSEND");
		if(!wait_for_headers_ready(CONNECTION_TIME_OUT))// Wait for ">"
			return false;
		send_headers(server, url, request.GetBufferSize(), post);
		send_command(request.GetBuffer());
		modem.Write(26);

		if(!wait_for_response("SEND OK", CONNECTION_TIME_OUT)) // Wait for "SEND OK"
			return false;
		
		return true;
	}
	
	bool read_response(StringBuffer& response)
	{
		// Read response:
		const int t0 = TimeManager::GetSystemTime();
		HttpParser httpParser(response);
		HttpParser::ReturnCodes returnCode = HttpParser::ePending;

		// read previously buffered bytes
		while(modem.ReadBuffer.NumberOfAvailableBytes)
			httpParser.Write((char*)TextBufferGetChar(&modem.ReadBuffer), 1);
		TextBufferClear(&modem.ReadBuffer);

		// poll bytes
		do
		{
			if(modem.ReadBytesFromModem()) {
				httpParser.Write((char*)modem.ReadBuffer.ReadingPosition, modem.ReadBuffer.NumberOfAvailableBytes);
				TextBufferClear(&modem.ReadBuffer);
			}
			else {
				returnCode = httpParser.Parse();
			}
			if(returnCode == HttpParser::ePending)
				TimeManager::DelayMs(100);
		}
		while( returnCode == HttpParser::ePending && TimeManager::GetSystemTime() - t0 < TRANSFER_TIME_OUT);

		return ( returnCode == HttpParser::eOk );
	}
	
	#define SERVER_ADDRESS_PREFIX "AT+CIPSTART=\"TCP\",\""
	#define SERVER_ADDRESS_POSTFIX "\",\"80\""
	bool send_server_address(const char* server)
	{
		const size_t command_size = sizeof(SERVER_ADDRESS_PREFIX)-1 + sizeof(SERVER_ADDRESS_POSTFIX)-1 + strlen(server) + 1;
		char* command = (char*)malloc(command_size);
		if(!command)
			return false;
		
		strcpy(command, SERVER_ADDRESS_PREFIX);
		strcat(command, server);
		strcat(command, SERVER_ADDRESS_POSTFIX);
		
		bool ret = send_command_with_response(command, "OK");
		
		free(command);
		return ret;
	}
	
	void send_headers(const char *server, const char* url, const size_t data_size /* = 0 */, bool post /* = false */)
	{
		if(post)
			modem.Write("POST ");
		else
			modem.Write("GET ");
		modem.Write( url );
		modem.Write( " HTTP/1.1\r\nContent-Type: application/json\r\nAccept: application/json\r\nHost: " );
		modem.Write( server );
		modem.Write( "\r\n" );
		if(post) {
			char length_str[10];
			snprintf(length_str, 10, "%d", data_size);
			modem.Write( "Content-Length: ");
			modem.Write( length_str );
			modem.Write( "\r\n" );
		}
		modem.Write("Connection: Close\r\n\r\n");
	}
	
	bool gsm_shut_down_gprs(void) {
		return send_command_with_response("AT+CIPSHUT", "SHUT OK");
	}

	GsmStates::eGsmState get_current_gsm_state()
	{
		if(!send_command("AT+CIPSTATUS"))
			return GsmStates::ERROR;

		char *line;
		for(int ntries=8; ntries ; ntries--) {
			line = modem.ReadLine(DEFAULT_TIME_OUT);
			if(!line) // timeout
				break;
			
			if(strcmp(line,"ERROR") == 0)
				break;
			
			GsmStates::eGsmState state = ParseGsmState(line);
			if(state != GsmStates::UNKNOWN)
				return state;
		}
		
		return GsmStates::UNKNOWN;
	}
	
	bool send_command(const char* command)
	{
		modem.ClearBuffer();

		modem.Write(command);
		modem.Write("\r\n");
		TimeManager::DelayMs(COMMAND_WAIT_TIME_MS);
		return true;
	}

	bool send_command_with_any_response(const char* command)
	{
		send_command(command);
		return wait_for_any_response(DEFAULT_TIME_OUT);
	}

	bool send_command_with_response(const char* command, const char* response)
	{
		send_command(command);
		return wait_for_response(response, DEFAULT_TIME_OUT);
	}

	bool wait_for_headers_ready(size_t timeout) {
		int t0 = TimeManager::GetSystemTime();
		do
		{
			if(modem.ReadBytesFromModem()) {
				while(modem.ReadBuffer.NumberOfAvailableBytes != 0) {
					if(TextBufferGetChar(&modem.ReadBuffer) == '>')
						return true;
				}
			}
			TimeManager::DelayMs(50);
		}
		while(TimeManager::GetSystemTime() - t0 < timeout);
		return false;
	}

	bool wait_for_any_response(size_t timeout) {
		return modem.ReadLine(timeout) != 0;
	}

	bool wait_for_response(const char* response, size_t timeout) {
		int ntries;
		char *p;
		for(ntries=8; ntries ; ntries--) {
			p = modem.ReadLine(timeout);
			if(!p) {
				return false;	/* terminate because of timeout */
			}
			if(strcmp(p,response) == 0) {
				return true;
			}
			else if(strcmp(p,"ERROR") == 0) {
				return false;
			}
		}
		return false;
	}

	bool gsm_init()
	{
		if(!modem.Init())
			return false;

		// Clear line, etc.
		modem.Write("\r\n\r\n");
		// reset profile
		if(!send_command_with_response("ATZ", "OK"))
		{
			// try to reset send state.
			modem.Write(26);
			TimeManager::DelayMs(400);
			modem.Write("\r\n\r\n");
			
			TimeManager::DelayMs(400);
			gsm_shut_down_gprs();
			TimeManager::DelayMs(400);

			if(!send_command_with_response("ATZ", "OK")) {
				// Try resetting the modem.
				modem.ResetModem();
				if(!send_command_with_response("ATZ", "OK"))
					return false;
			}
		}
		// Disable echo
		if(!send_command_with_response("ATE0", "OK"))
			return false;
		
		if(get_current_gsm_state() != GsmStates::IP_INITIAL) {
			send_command_with_response("AT+CIPSHUT","SHUT OK");
			TimeManager::DelayMs(400);			
		}
		
		if(!send_command_with_response("AT+CGATT=1","OK")) {
			// Case of modem power down. Try to restart.
			if(send_command_with_response("AT+CGATT=1","+CME ERROR: 107")) {
				modem.ResetModem();
				if(!send_command_with_response("ATE0", "OK"))
					return false;
			}
			return false;
		}
				
		if(!send_apn_parameters())
			return false;
		
		return true;
	}

	// Set up network and APN.
	bool send_apn_parameters()
	{
		// Query for the current network operator:
		send_command("AT+COPS?");
		char* line;
		char* network_operator = NULL;
		for(size_t retry_number = 0 ; retry_number < 3 ; retry_number++) {
			line = modem.ReadLine(DEFAULT_TIME_OUT);
			if(line == NULL)
				return false;
			if(strncmp(line, "+COPS: 0,0,", 11) == 0) { // search for the next '"'
				network_operator = line + 12;
				for(line = network_operator; *line && *line != '"' ; line++);
				*line = 0;
				break;
			}
		}
		if(network_operator == NULL)
			return false;
		
		// Setup operator according to the APN list:
		char apn_buffer[255];
		if(!APNManager::GetAPNParameters(network_operator, apn_buffer, sizeof(apn_buffer)))
			return false;
		
		if(!send_command_with_response("AT+CGDCONT=1,\"IP\",\"uinternet\"","OK"))
			return false;
		if(!send_command_with_response("AT+CDNSCFG=\"192.118.8.82\",\"192.118.8.83\"","OK"))
			return false;		
		if(!send_command_with_response(apn_buffer ,"OK")) // AT+CSTT="uinternet","orange","orange"
			return false;

		return true;
	}
	
};
