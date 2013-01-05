#include "token_vector.h"
#include "jsmntok_t.h"
#include <stdlib.h>
#include <assert.h>

static bool token_vector_reallocate_buffer(TokenVector* v, const size_t new_number_of_elements);

TokenVector* token_vector_new(void) {
    TokenVector* v = (TokenVector*) malloc(sizeof (TokenVector));
    if (v) {
        v->size = 0;
        v->tokens = NULL;
        token_vector_reallocate_buffer(v, 10);
    }
    return v;
}

void token_vector_destroy(TokenVector* v) {
    free(v->tokens);
    free(v);
}

bool token_vector_add(TokenVector* v, jsmntok_t* t) {
    if (v->size == v->limit) {
        if(!token_vector_reallocate_buffer(v, v->limit * 2))
            return false;
    }

    v->tokens[v->size] = *t;
    ++v->size;
    
    return true;
}

jsmntok_t token_vector_get(TokenVector* v, size_t index) {
    if (index < v->size)
        return v->tokens[index];
    else
        assert(false);
		
		return v->tokens[0];
}

jsmntok_t* token_vector_get_pointer(TokenVector* v, size_t index) {
    if (index < v->size)
        return &v->tokens[index];
    else
        assert(false);
		
		return &v->tokens[0];
}

bool token_vector_reallocate_buffer(TokenVector* v, const size_t new_number_of_elements) {
    jsmntok_t* new_tokens = realloc(v->tokens, sizeof(jsmntok_t) * new_number_of_elements);
    if(new_tokens != NULL) {
        v->limit = new_number_of_elements;
        v->tokens = new_tokens;
        return true;
    }
    return false;
}

size_t token_vector_size(TokenVector *v) {
    return v->size;
}
