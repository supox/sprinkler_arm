/* 
 * File:   token_vector.h
 * Author: supox
 *
 * Created on October 13, 2012, 4:12 PM
 */

#ifndef TOKEN_VECTOR_H
#define	TOKEN_VECTOR_H

#include "jsmntok_t.h"
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t size;
    size_t limit;
    jsmntok_t* tokens;
} TokenVector;

TokenVector* token_vector_new(void);
void token_vector_destroy(TokenVector* v);
bool token_vector_add(TokenVector* v, jsmntok_t* t);
jsmntok_t token_vector_get(TokenVector* v, size_t index);
jsmntok_t* token_vector_get_pointer(TokenVector* v, size_t index);
size_t token_vector_size(TokenVector *v);

#ifdef __cplusplus
}
#endif

#endif	/* TOKEN_VECTOR_H */

