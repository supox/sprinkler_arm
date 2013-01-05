#include <stdlib.h>

#include "jsmn.h"

/**
 * Allocates a fresh unused token from the token pull.
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser, TokenVector *tokens) {
	jsmntok_t tok;
	tok.start = tok.end = -1;
	tok.size = 0;
#ifdef JSMN_PARENT_LINKS
	tok->parent = -1;
#endif
        if(token_vector_add(tokens, &tok))  {
            parser->toknext++;
            return token_vector_get_pointer(tokens, token_vector_size(tokens)-1);
        }
        else
            return NULL;
}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token(jsmntok_t *token, jsmntype_t type, 
                            int start, int end) {
	token->type = type;
	token->start = start;
	token->end = end;
	token->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
static jsmnerr_t jsmn_parse_primitive(jsmn_parser *parser, const char *js,
		TokenVector *tokens) {
	jsmntok_t *token;
	int start;

	start = parser->read_pos;

	for (; js[parser->read_pos] != '\0'; parser->read_pos++) {
		switch (js[parser->read_pos]) {
#ifndef JSMN_STRICT
			/* In strict mode primitive must be followed by "," or "}" or "]" */
			case ':':
#endif
			case '\t' : case '\r' : case '\n' : case ' ' :
			case ','  : case ']'  : case '}' :
				goto found;
		}
		if (js[parser->read_pos] < 32 || js[parser->read_pos] >= 127) {
			parser->read_pos = start;
			return JSMN_ERROR_INVAL;
		}
	}
#ifdef JSMN_STRICT
	/* In strict mode primitive must be followed by a comma/object/array */
	parser->read_pos = start;
	return JSMN_ERROR_PART;
#endif

found:
	token = jsmn_alloc_token(parser, tokens);
	if (token == NULL) {
		parser->read_pos = start;
		return JSMN_ERROR_NOMEM;
	}
	jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->read_pos);
#ifdef JSMN_PARENT_LINKS
	token->parent = parser->toksuper;
#endif
	parser->read_pos--;
	return JSMN_SUCCESS;
}

/**
 * Filsl next token with JSON string.
 */
static jsmnerr_t jsmn_parse_string(jsmn_parser *parser, const char *js, TokenVector *tokens) {
	jsmntok_t *token;

	int start = parser->read_pos;

	parser->read_pos++;

	/* Skip starting quote */
	for (; js[parser->read_pos] != '\0'; parser->read_pos++) {
		char c = js[parser->read_pos];

		/* Quote: end of string */
		if (c == '\"') {
			token = jsmn_alloc_token(parser, tokens);
			if (token == NULL) {
				parser->read_pos = start;
				return JSMN_ERROR_NOMEM;
			}
			jsmn_fill_token(token, JSMN_STRING, start+1, parser->read_pos);
#ifdef JSMN_PARENT_LINKS
			token->parent = parser->toksuper;
#endif
			return JSMN_SUCCESS;
		}

		/* Backslash: Quoted symbol expected */
		if (c == '\\') {
			parser->read_pos++;
			switch (js[parser->read_pos]) {
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
					parser->read_pos = start;
					return JSMN_ERROR_INVAL;
			}
		}
	}
	parser->read_pos = start;
	return JSMN_ERROR_PART;
}

/**
 * Parse JSON string and fill tokens.
 */
jsmnerr_t jsmn_parse(jsmn_parser *parser, const char *js, TokenVector *tokens) {
	jsmnerr_t r;
	int i;
	jsmntok_t *token;

	for (; js[parser->read_pos] != '\0'; parser->read_pos++) {
		char c;
		jsmntype_t type;

		c = js[parser->read_pos];
		switch (c) {
			case '{': case '[':
				token = jsmn_alloc_token(parser, tokens);
				if (token == NULL)
					return JSMN_ERROR_NOMEM;
				if (parser->toksuper != -1) {
					token_vector_get_pointer(tokens, parser->toksuper)->size++;
#ifdef JSMN_PARENT_LINKS
					token->parent = parser->toksuper;
#endif
				}
				token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
				token->start = parser->read_pos;
				parser->toksuper = parser->toknext - 1;
				break;
			case '}': case ']':
				type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
#ifdef JSMN_PARENT_LINKS
				if (parser->toknext < 1) {
					return JSMN_ERROR_INVAL;
				}
				token = &tokens[parser->toknext - 1];
				for (;;) {
					if (token->start != -1 && token->end == -1) {
						token->end = parser->read_pos + 1;
						parser->toksuper = token->parent;
						break;
					}
					if (token->parent == -1) {
						break;
					}
					token = &tokens[token->parent];
				}
#else
				for (i = parser->toknext - 1; i >= 0; i--) {
					token = token_vector_get_pointer(tokens, i);
					if (token->start != -1 && token->end == -1) {
						if (token->type != type) {
							return JSMN_ERROR_INVAL;
						}
						parser->toksuper = -1;
						token->end = parser->read_pos + 1;
						break;
					}
				}
				/* Error if unmatched closing bracket */
				if (i == -1) return JSMN_ERROR_INVAL;
				for (; i >= 0; i--) {
					token = token_vector_get_pointer(tokens, i);
					if (token->start != -1 && token->end == -1) {
						parser->toksuper = i;
						break;
					}
				}
#endif
				break;
			case '\"':
				r = jsmn_parse_string(parser, js, tokens);
				if (r < 0) return r;
				if (parser->toksuper != -1)
                                    token_vector_get_pointer(tokens, parser->toksuper)->size++;
				break;
			case '\t' : case '\r' : case '\n' : case ':' : case ',': case ' ': 
				break;
#ifdef JSMN_STRICT
			/* In strict mode primitives are: numbers and booleans */
			case '-': case '0': case '1' : case '2': case '3' : case '4':
			case '5': case '6': case '7' : case '8': case '9':
			case 't': case 'f': case 'n' :
#else
			/* In non-strict mode every unquoted value is a primitive */
			default:
#endif
				r = jsmn_parse_primitive(parser, js, tokens);
				if (r < 0) return r;
				if (parser->toksuper != -1)
					token_vector_get_pointer(tokens, parser->toksuper)->size++;
				break;

#ifdef JSMN_STRICT
			/* Unexpected char in strict mode */
			default:
				return JSMN_ERROR_INVAL;
#endif

		}
	}

	for (i = parser->toknext - 1; i >= 0; i--) {
            token = token_vector_get_pointer(tokens, i);
		/* Unmatched opened object or array */
		if (token->start != -1 && token->end == -1) {
			return JSMN_ERROR_PART;
		}
	}

	return JSMN_SUCCESS;
}

/**
 * Creates a new parser based over a given  buffer with an array of tokens 
 * available.
 */
void jsmn_init(jsmn_parser *parser) {
	parser->read_pos = 0;
	parser->toknext = 0;
	parser->toksuper = -1;
}

