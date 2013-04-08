#pragma once
#include "HttpParser.h"

HttpParser::HttpParser(StringBuffer& body) :
	settings(), m_body(body), m_message(), m_iMessageIndex(0), has_done(false)
{
	http_parser_init(&parser, HTTP_RESPONSE);
	settings.on_body = body_cb;
	settings.on_message_complete = message_complete_cb;
	parser.data = this;
}

HttpParser::~HttpParser()
{
}
		
void HttpParser::Write(StringBuffer& data)
{
	return Write(data.GetBuffer(), data.GetBufferSize());
}

void HttpParser::Write(const char* data, size_t buffer_length)
{
	m_message.Write(data, buffer_length);
}

HttpParser::ReturnCodes HttpParser::Parse()
{
	const size_t buffer_size = m_message.GetBufferSize() - m_iMessageIndex;

	if(buffer_size == 0)
		return ePending;
	
  size_t nparsed = http_parser_execute(&parser, &settings, m_message.GetBuffer() + m_iMessageIndex, buffer_size);
	m_iMessageIndex += nparsed;
 
	if(has_done)
		return eOk;
	if( nparsed != buffer_size )
		return eError;
	return ePending;
}


int HttpParser::body_cb (http_parser *p, const char *buf, size_t len)
{
	HttpParser* t = (HttpParser*) p->data;
	if(t != NULL ) {
		t->m_body.Write(buf,len);
		return 0;
	}
	return 1;
}

int HttpParser::message_complete_cb (http_parser *p)
{
	HttpParser* t = (HttpParser*) p->data;
	if(t != NULL) {
		t->has_done = true;
		return 0;
	}
	return 1;
}
