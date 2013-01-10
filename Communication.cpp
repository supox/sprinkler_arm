#include "Communication.h"
#include "CommunicationTimeouts.h"
#include "TimeManager.h"

#include <stdio.h>
#include <string.h>

namespace Communication
{
	static bool send_command(const char* command);
	static bool send_command_with_response(const char* command, const char* response);
	static bool send_part_of_command(const char* command);
	static bool wait_for_response(const char* response, size_t timeout);
	static bool wait_for_any_response(size_t timeout_ms);

	static void send_headers(const char *server, const char* url, const size_t data_size); 
	static bool wait_for_headers_ready(size_t timeout_ms);
	static bool gsm_init();
	static void gsm_hw_send_byte(const char ch);
	static bool gsm_hw_has_pending_byte();
	static char* gsm_hw_get_line();
	static void gsm_hw_init();
	static bool gsm_hw_has_init = false;	
	static bool gsm_has_init = false;

	
	#define STR0 "{\"id\":1,\"identifier\":\"12345\",\"machine_version\":\"1.0\",\"mac_address\":\"FF:FF:FF:FF:FF:FF\",\"refresh_rate_seconds\":3600,\"latitude\":0,\"longitude\":null,\"created_at\":\"2012-11-09 17:34:39\",\"updated_at\":\"2012-11-09 17:34:39\",\"main_valve_timing\":\"simultaneous\",\"main_valve_delay\":0,\"main_valf\":0}"
	#define STR1 "{\"sensors\":[{\"id\":1,\"port_index\":0, \"type\":\"water_meter\"},{\"id\":2,\"port_index\":1,\"type\":\"water_meter\"}],\"alarms\":[{\"port_index\":1,\"alarm_value\":5.0,\"condition_type\":\"greater_than\"}]}\n"
	#define STR2 "[{\"id\":4,\"port_index\":3},{\"id\":5,\"port_index\":4}]\n"
	#define STR3 "[{\"start_time\":1350339360,\"valf_id\":4,\"irrigation_mode\":\"time\",\"amount\":2},{\"start_time\":1350339360,\"valf_id\":5,\"irrigation_mode\":\"time\",\"amount\":4},{\"start_time\":1350944160,\"valf_id\":4,\"irrigation_mode\":\"time\",\"amount\":2},{\"start_time\":1350944160,\"valf_id\":5,\"irrigation_mode\":\"volume\",\"amount\":4}]\n\n"
	#define STR4 "{\"time\":1350339320}\n\n"
	
	bool GetWebPage(const char* url, StringBuffer& sb)
	{
		static int state = 0;
		switch(state)
		{
			case 0:
				sb.Write(STR0, sizeof(STR0));
				break;
			case 1:
				// Sensors
				sb.Write(STR1, sizeof(STR1));
				break;
			case 2:
				// Valves
				sb.Write(STR2, sizeof(STR2));
				break;
			case 3: // Irrigations
				sb.Write(STR3, sizeof(STR3));
				break;
			case 4: // Irrigations
				sb.Write(STR4, sizeof(STR4));
				break;
			default:
				return false;
		}
		state++;
		return true;
	}
	
	bool PostWebPage(const char* url, StringBuffer& request, StringBuffer& response)
	{
		return false;
	}
	
	bool gsm_send_post_data(const char* server, const char* url, StringBuffer& request, StringBuffer& response) {
		bool ret = false;

		if(!gsm_has_init && !gsm_init() )
			return false;

		if(!send_command_with_response("AT+CGATT=1","OK"))
			return false;

		if(!send_command_with_response("AT+CIICR","OK"))
			return false;
		if(!send_command("AT+CIFSR"))
			return false;
		if(!send_command_with_response("AT+CDNSORIP=1","OK"))
			return false;

		send_part_of_command("AT+CIPSTART=\"TCP\",\"");
		send_part_of_command( server );
		if(!send_command_with_response("\",\"80\"", "OK"))
			return false;
		if(!wait_for_response("CONNECT OK", CONNECTION_TIME_OUT))
			return false;

		send_part_of_command("AT+CIPSEND\r\n");
		if(!wait_for_headers_ready(CONNECTION_TIME_OUT))// Wait for ">"
			return false;
		send_headers(server, url, request.GetBufferSize());
		send_command(request.GetBuffer());
		gsm_hw_send_byte(26);

		if(!wait_for_response("SEND OK", CONNECTION_TIME_OUT)) // Wait for "SEND OK"
			return false;

		int t0 = TimeManager::GetSystemTime();
		do
		{
		}
		while(TimeManager::GetSystemTime() - t0 < TRANSFER_TIME_OUT);
		
		for(int tries = 0 ; tries < 4 ; tries++) {
			if(wait_for_response("{\"result\": \"OK\"}", CONNECTION_TIME_OUT)) {
				ret = true;
				break;
			}
		}

		send_command("AT+CIPCLOSE");
		send_command("AT+CIPSHUT");
		return ret;
	}

	void send_headers(const char *server, const char* url, const size_t data_size)
	{
		char length_str[10];
		snprintf(length_str, 10, "%d", data_size);
		send_part_of_command("POST ");
		send_part_of_command( url );
		send_part_of_command( " HTTP/1.1\r\nContent-Type: application/json\r\nAccept: application/json\r\nHost: " );
		send_part_of_command( server );
		send_part_of_command( "\r\nContent-Length: ");
		send_part_of_command( length_str );
		send_part_of_command("\r\nConnection: Close\r\n\r\n");
	}
	
	bool gsm_shut_down_gprs(void) {
		return send_command("at+cipshut");
	}

	bool send_command(const char* command)
	{
		send_part_of_command(command);
		send_part_of_command("\r\n");
		return wait_for_any_response(DEFAULT_TIME_OUT);
	}

	bool send_command_with_response(const char* command, const char* response)
	{
		send_part_of_command(command);
		send_part_of_command("\r\n");

		return wait_for_response(response, DEFAULT_TIME_OUT);
	}

	bool wait_for_headers_ready(size_t timeout) {
		int t0 = TimeManager::GetSystemTime();
		do
		{
			if(gsm_hw_has_pending_byte())
				return true;
		}
		while(TimeManager::GetSystemTime() - t0 < timeout);
		return false;
	}

	char* wait_for_new_line(size_t timeout) {
		char* response;
		int t0 = TimeManager::GetSystemTime();
		do
		{
			response = gsm_hw_get_line();
			if(response)
				return response;
		}
		while(TimeManager::GetSystemTime() - t0 < timeout);
		return NULL;
	}

	bool wait_for_any_response(size_t timeout) {
		return wait_for_new_line(timeout) != 0;
	}

	bool wait_for_response(const char* response, size_t timeout) {
		int ntries;
		char *p;
		for(ntries=8; ntries ; ntries--) {
			p = wait_for_new_line(timeout);
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

	bool send_part_of_command(const char* command)
	{
		// TODO...
		printf("%s",command);
		return true;
	}

	bool gsm_init()
	{
		if(!gsm_hw_has_init)
		{
			gsm_hw_init();
			// Disable echo
			if(!send_command_with_response("ATE0\r\n", "OK"))
				return false;
			gsm_hw_has_init = true;
		}

		if(!send_command_with_response("AT+CGATT=1","OK"))
			return false;
		if(!send_command_with_response("AT+CGDCONT=1,\"IP\",\"uinternet\"","OK"))
			return false;
		if(!send_command_with_response("AT+CDNSCFG=\"192.118.8.82\",\"192.118.8.83\"","OK"))
			return false;
		if(!send_command_with_response("AT+CSTT=\"uinternet\",\"orange\",\"orange\"","OK"))
			return false;
		gsm_has_init = true;
		return true;
	}
		
	void gsm_hw_send_byte(const char ch)
	{
		// TODO...
	}

	bool gsm_hw_has_pending_byte()
	{
		// TODO
		return true;
	}
	
	char* gsm_hw_get_line()
	{
		// TODO
		return NULL;
	}
	void gsm_hw_init()
	{
		// TODO...
	}
	
	
};

extern "C" {
	void USART3_IRQHandler()
	{
			// TODO - handle interrupt
	}
}
