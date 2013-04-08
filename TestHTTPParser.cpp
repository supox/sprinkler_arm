#ifdef _TESTS
#include "TestHelpers.h"
#include "HttpParser.h"
#include "StringBuffer.h"

#include <string.h>

using namespace TestHelpers;

const char raw[]= "HTTP/1.1 200 OK\r\n"
		 "Location: http://www.google.com/\r\n"
		 "Content-Length:  219  \r\n"
		 "\r\n"
		 "<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n"
		 "<TITLE>301 Moved</TITLE></HEAD><BODY>\n"
		 "<H1>301 Moved</H1>\n"
		 "The document has moved\n"
		 "<A HREF=\"http://www.google.com/\">here</A>.\r\n"
		 "</BODY></HTML>\r\nCLOSED\r\n";

const char body[] = "<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n"
			"<TITLE>301 Moved</TITLE></HEAD><BODY>\n"
			"<H1>301 Moved</H1>\n"
			"The document has moved\n"
			"<A HREF=\"http://www.google.com/\">here</A>.\r\n"
			"</BODY></HTML>\r\nCLOSED\r\n";


static void TestFull()
{
	const size_t raw_len = strlen(raw);
	StringBuffer sb_in, sb_out;
	HttpParser parser(sb_out);
	sb_in.Write(raw,raw_len);
	
	parser.Write(sb_in);
  HttpParser::ReturnCodes e = parser.Parse();

	assert(e == HttpParser::eOk);
	assert(sb_out.GetBufferSize()  == 219);
	assert(strncmp(sb_out.GetBuffer(), body, 219) == 0);
}

static void TestPartial()
{
	const size_t raw_len = strlen(raw);
	StringBuffer sb_in1, sb_in2, sb_out;
	HttpParser parser(sb_out);
	sb_in1.Write(raw, raw_len-15);
	sb_in2.Write(raw + raw_len-15, 15);
	parser.Write(sb_in1);
  HttpParser::ReturnCodes e = parser.Parse();
	assert(e == HttpParser::ePending);

	parser.Write(sb_in2);
  e = parser.Parse();
	assert(e == HttpParser::eOk);

	assert(sb_out.GetBufferSize()  == 219);
	assert(strncmp(sb_out.GetBuffer(), body, 219) == 0);
}

void TestHTTPParser()
{
	TestFull();
	TestPartial();
}

#endif
