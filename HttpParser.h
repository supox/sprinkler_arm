#pragma once
#include "StringBuffer.h"
#include "http_parser.h"

class HttpParser
{
	public:
		enum ReturnCodes{ eOk, eError, ePending };

		HttpParser(StringBuffer& body);
		~HttpParser();
		
		void Write(StringBuffer& data);
		void Write(const char* data, size_t buffer_length);
		ReturnCodes Parse();

	private:
		static int body_cb (http_parser *p, const char *buf, size_t len);
		static int message_complete_cb (http_parser *p);

		http_parser_settings settings ;
		StringBuffer& m_body;
		StringBuffer m_message;
		size_t m_iMessageIndex;
		bool has_done;
};
