#ifndef __JSMN_H_
#define __JSMN_H_

#include "jsmntok_t.h"
#include "token_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	/* Not enough tokens were provided */
	JSMN_ERROR_NOMEM = -1,
	/* Invalid character inside JSON string */
	JSMN_ERROR_INVAL = -2,
	/* The string is not a full JSON packet, more bytes expected */
	JSMN_ERROR_PART = -3,
	/* Everything was fine */
	JSMN_SUCCESS = 0
} jsmnerr_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct {
	unsigned int read_pos; /* offset in the JSON string */
	int toknext; /* next token to allocate */
	int toksuper; /* suporior token node, e.g parent object or array */
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and vector of tokens, each describing
 * a single JSON object.
 */
jsmnerr_t jsmn_parse(jsmn_parser *parser, const char *js, TokenVector *tokens);

#ifdef __cplusplus
}
#endif

#endif /* __JSMN_H_ */
