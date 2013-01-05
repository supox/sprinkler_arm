/* 
 * File:   jsmn_tok_t.h
 * Author: supox
 *
 * Created on October 13, 2012, 4:13 PM
 */

#ifndef JSMN_TOK_T_H
#define	JSMN_TOK_T_H

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum {
	JSMN_PRIMITIVE = 0,
	JSMN_OBJECT = 1,
	JSMN_ARRAY = 2,
	JSMN_STRING = 3
} jsmntype_t;


/**
 * JSON token description.
 * @param		type	type (object, array, string etc.)
 * @param		start	start position in JSON data string
 * @param		end		end position in JSON data string
 */
typedef struct {
	jsmntype_t type;
	int start;
	int end;
	int size;
#ifdef JSMN_PARENT_LINKS
	int parent;
#endif
} jsmntok_t;


#endif	/* JSMN_TOK_T_H */

