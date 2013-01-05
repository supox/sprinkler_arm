#pragma once
#include "Vector.h"
#include "jsmntok_t.h"

namespace JSON
{
	enum jsmntype_t{
		JSMN_PRIMITIVE = 0,
		JSMN_OBJECT = 1,
		JSMN_ARRAY = 2,
		JSMN_STRING = 3
	} ;

	struct jsmntok_t {
		jsmntype_t type;
		int start;
		int end;
		int size;
	};	
	
	enum jsmnerr_t{
		/* Not enough tokens were provided */
		JSMN_ERROR_NOMEM = -1,
		/* Invalid character inside JSON string */
		JSMN_ERROR_INVAL = -2,
		/* The string is not a full JSON packet, more bytes expected */
		JSMN_ERROR_PART = -3,
		/* Everything was fine */
		JSMN_SUCCESS = 0
	} ;

	class Parser
	{
	public:
		Parser();
		~Parser();
		jsmnerr_t Parse(const char *js, Vector<jsmntok_t>& Tokens);
		
	private:
		jsmntok_t *alloc_token(Vector<jsmntok_t> &tokens);
		jsmnerr_t parse_string(const char *js, Vector<jsmntok_t> &tokens);
		void fill_token(jsmntok_t *token, jsmntype_t type, int start, int end);
		jsmnerr_t parse_primitive(const char *js, Vector<jsmntok_t> &tokens);
		
		unsigned int read_pos; /* offset in the JSON string */
		int toknext; /* next token to allocate */
		int toksuper; /* suporior token node, e.g parent object or array */		
	};

};
