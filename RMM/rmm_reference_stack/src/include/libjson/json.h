/**
 * Copyright (c)  2015, Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __LIBJSON_JSON_H__
#define __LIBJSON_JSON_H__

#include "libutils/types.h"

#if 0
typedef signed long long int64;
#endif

typedef enum {
	JSON_OBJECT,
	JSON_ARRAY,
	JSON_STRING,
	JSON_INTEGER,
	JSON_REAL,
	JSON_TRUE,
	JSON_FALSE,
	JSON_NULL
} json_type;

/* MUST BE the first member of json_xxx_t */
typedef struct json_value {
	json_type type;
	struct json_value *next;
} json_t;

/* An object is an unordered set of name/value pairs. */
typedef struct json_pair {
	char   *name;
	json_t *value;

	struct json_pair *next;
} json_pair_t;

typedef struct  {
	json_t json;

	json_pair_t *next;
	json_pair_t *tail;
} json_object_t;

/* An array is an ordered collection of values. */
typedef struct {
	json_t json;

	int size;
	json_t *next;
	json_t *tail;
} json_array_t;

typedef struct {
	json_t json;
	char *value;
} json_string_t;

typedef struct {
	json_t json;
	double value;
} json_real_t;

typedef struct {
	json_t json;
	int64 value;
} json_integer_t;


#define json_to_object(json_)	((json_object_t *)(json_))
#define json_to_array(json_)	((json_array_t *)(json_))
#define json_to_string(json_)	((json_string_t *)(json_))
#define json_to_real(json_)		((json_real_t *)(json_))
#define json_to_integer(json_)	((json_integer_t *)(json_))

extern void    json_free(json_t *json);
extern json_t *json_object(void);
extern json_t *json_object_get(const json_t *json, const char *key);
extern int     json_object_add(json_t *json, const char *key, json_t *value);
extern int 	   json_object_remove(json_t *json, char *key);
extern json_t *json_array(void);
extern int     json_array_size(json_t *json);
extern json_t *json_array_get(json_t *json, int index);
extern int     json_array_add(json_t *json, json_t *value);
extern json_t *json_string(const char *value);
extern char   *json_string_value(json_t *json);
extern json_t *json_integer(int64 value);
extern int64   json_integer_value(json_t *json);
extern int     json_integer_set(json_t *json, int64 value);
extern json_t *json_real(double value);
extern double  json_real_value(json_t *json);
extern int     json_real_set(json_t *json, double value);
extern double  json_number_value(json_t *json);
extern json_t *json_true(void);
extern json_t *json_false(void);
extern json_t *json_null(void);


extern json_t *json_parse(char *string);
extern json_t *json_parse_with_len(char *string, int len);
extern int     json_format(json_t *json, char *output, int sz);
extern void    json_print(json_t *json);


extern int add_json_integer(json_t *result, const char *name, int input);
extern int add_json_string(json_t *result, const char *name, const char *input);
extern int add_json_bool(json_t *result, const char *name, int input);



#endif
