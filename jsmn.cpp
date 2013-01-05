#include "jsmn.h"
#include <stdlib.h>

namespace JSON
{
	jsmn_parser::jsmn_parser() :
		read_pos(0), toknext(0), toksuper(-1)
	{
	}
	
	/*
	jsmn_parser::~jsmn_parser()
	{
	}
	*/
	
	jsmnerr_t jsmn_parser::parse(const char *js, std::vector<jsmntok_t> &tokens)
	{
		jsmnerr_t r;
		int i;
		jsmntok_t *token;

		for (; js[read_pos] != '\0'; read_pos++) {
			char c;
			jsmntype_t type;

			c = js[read_pos];
			switch (c) {
				case '{': case '[':
					token = alloc_token(tokens);
					if (token == NULL)
						return JSMN_ERROR_NOMEM;
					if (toksuper != -1) {
						tokens[toksuper].size++;
					}
					token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
					token->start = read_pos;
					toksuper = toknext - 1;
					break;
				case '}': case ']':
					type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
					for (i = toknext - 1; i >= 0; i--) {
						token = &tokens[i];
						if (token->start != -1 && token->end == -1) {
							if (token->type != type) {
								return JSMN_ERROR_INVAL;
							}
							toksuper = -1;
							token->end = read_pos + 1;
							break;
						}
					}
					/* Error if unmatched closing bracket */
					if (i == -1) return JSMN_ERROR_INVAL;
					for (; i >= 0; i--) {
						token = &tokens[i];
						if (token->start != -1 && token->end == -1) {
							toksuper = i;
							break;
						}
					}
					break;
				case '\"':
					r = parse_string(js, tokens);
					if (r < 0) return r;
					if (toksuper != -1)
						tokens[toksuper].size++;
					break;
				case '\t' : case '\r' : case '\n' : case ':' : case ',': case ' ': 
					break;
				/* In strict mode primitives are: numbers and booleans */
				case '-': case '0': case '1' : case '2': case '3' : case '4':
				case '5': case '6': case '7' : case '8': case '9':
				case 't': case 'f': case 'n' :
					r = parse_primitive(js, tokens);
					if (r < 0) return r;
					if (toksuper != -1)
						tokens[toksuper].size++;
					break;
				default:
					return JSMN_ERROR_INVAL;
			}
		}

		for (i = toknext - 1; i >= 0; i--) {
			token = &tokens[i];
			/* Unmatched opened object or array */
			if (token->start != -1 && token->end == -1) {
				return JSMN_ERROR_PART;
			}
		}

		return JSMN_SUCCESS;
	}	
	
	jsmntok_t *jsmn_parser::alloc_token(std::vector<jsmntok_t> &tokens)
	{
		jsmntok_t tok;
		tok.start = tok.end = -1;
		tok.size = 0;
		tokens.push_back(tok);
		toknext++;
		return &tokens[tokens.size()-1];
	}
	
	jsmnerr_t jsmn_parser::parse_string(const char *js, std::vector<jsmntok_t> &tokens) {
		jsmntok_t *token;
		int start = read_pos;
		
		read_pos++;

		/* Skip starting quote */
		for (; js[read_pos] != '\0'; read_pos++) {
			char c = js[read_pos];

			/* Quote: end of string */
			if (c == '\"') {
				token = alloc_token(tokens);
				if (token == NULL) {
					read_pos = start;
					return JSMN_ERROR_NOMEM;
				}
				fill_token(token, JSMN_STRING, start+1, read_pos);
				return JSMN_SUCCESS;
			}

			/* Backslash: Quoted symbol expected */
			if (c == '\\') {
				read_pos++;
				switch (js[read_pos]) {
					/* Allowed escaped symbols */
					case '\"': case '/' : case '\\' : case 'b' :
					case 'f' : case 'r' : case 'n'  : case 't' :
						break;
					/* Allows escaped symbol \uXXXX */
					case 'u':
						/* TODO */
						break;
					/* Unexpected symbol */
					default:
						read_pos = start;
						return JSMN_ERROR_INVAL;
				}
			}
		}
		read_pos = start;
		return JSMN_ERROR_PART;
	}
	
	void jsmn_parser::fill_token(jsmntok_t *token, jsmntype_t type, 
                            int start, int end) {
		token->type = type;
		token->start = start;
		token->end = end;
		token->size = 0;
	}
	
	jsmnerr_t jsmn_parser::parse_primitive(const char *js, std::vector<jsmntok_t> &tokens)
	{
		jsmntok_t *token;
		int start;

		start = read_pos;

		for (; js[read_pos] != '\0'; read_pos++) {
			switch (js[read_pos]) {
				case '\t' : case '\r' : case '\n' : case ' ' :
				case ','  : case ']'  : case '}' :
					goto found;
			}
			if (js[read_pos] < 32 || js[read_pos] >= 127) {
				read_pos = start;
				return JSMN_ERROR_INVAL;
			}
		}
	/* In strict mode primitive must be followed by a comma/object/array */
	read_pos = start;
	return JSMN_ERROR_PART;

	found:
		token = alloc_token(tokens);
		if (token == NULL) {
			read_pos = start;
			return JSMN_ERROR_NOMEM;
		}
		fill_token(token, JSMN_PRIMITIVE, start, read_pos);
		read_pos--;
		return JSMN_SUCCESS;
	}
}
