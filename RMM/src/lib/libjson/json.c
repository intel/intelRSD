/**
 * Copyright (c)  2015-2017 Intel Corporation.
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

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include "libjson/json.h"
#include "libsecurec/safe_lib.h"


/*****************************************************************************/
/*****************************************************************************/

#define TOKEN_INVALID         -1
#define TOKEN_EOF              0
#define TOKEN_STRING         256
#define TOKEN_INTEGER        257
#define TOKEN_REAL           258
#define TOKEN_TRUE           259
#define TOKEN_FALSE          260
#define TOKEN_NULL           261

typedef struct {
	char *data;
	int   pos;
} stream_t;	

typedef struct {
#define TOKENSZ	4095
	int  _len;
	//char _buf[TOKENSZ + 1];
	char *_buf;
	int  _max_len;
} token_t;

typedef struct {
	int result;		/* TOKEN_XXX */

	union {
		char *string;
		int64 integer;
		double real;
	} value;

	stream_t stream;
	token_t  token;
} lex_t;

/*** Token ***/
static inline void token_init(token_t *token)
{
	token->_len = 0;
}

static inline char *token_string(token_t *token)
{
	token->_buf[token->_len] = '\0';
	return token->_buf;
}

static inline void token_replace(token_t *token, char c)
{
	if (token->_len > 0)
		token->_buf[token->_len - 1] = c;
}

static inline void token_push(token_t *token, char c)
{
	//if (token->_len == TOKENSZ) {
	if (token->_len == token->_max_len) {
		printf("ERROR: TOKENSZ(%d) is small!\n", token->_max_len);
		return;
	}

	token->_buf[token->_len++] = c;
}

static inline void token_pop(token_t *token)
{
	if (token->_len > 0)
		--token->_len;
}

/*** Stream ***/
static inline void stream_init(stream_t *stream, char *data)
{
	stream->data = data;
	stream->pos = 0;
}

static inline char stream_get(stream_t *stream)
{
	char c;

	c = stream->data[stream->pos];
	if (c == '\0')
		return (char)EOF;

	stream->pos++;
	return c;
}

static inline void stream_unget(stream_t *stream)
{
	if (stream->pos > 0)
		stream->pos--;
}

/*** Lex ***/
static void lex_init(lex_t *lex, char *data)
{
	lex->result = TOKEN_INVALID;

	stream_init(&lex->stream, data);
	token_init(&lex->token);
}

static void lex_close(lex_t *lex)
{
	if (lex->result == TOKEN_STRING && lex->value.string != NULL) {
		free(lex->value.string);
		lex->value.string = NULL;
	}
}

static inline char *lex_token(lex_t *lex)
{
	return token_string(&lex->token);
}

static char lex_get(lex_t *lex)
{
	return stream_get(&lex->stream);
}

static void lex_save(lex_t *lex, char c)
{
	token_push(&lex->token, c);
}

static char lex_read(lex_t *lex)
{
	char c;

	c = stream_get(&lex->stream);
	token_push(&lex->token, c);

	return c;
}

static void lex_back(lex_t *lex)
{
	stream_unget(&lex->stream);
	token_pop(&lex->token);
}

static void lex_replace(lex_t *lex, char c)
{
	token_replace(&lex->token, c);
}

static void lex_scan_string(lex_t *lex)
{
	char c, t;
	char *token;

	lex->value.string = NULL;
	lex->result = TOKEN_INVALID;

	c = lex_read(lex);
	while (c != '"') {
		if (c == (char)EOF)
			return;

		if (c != '\\') {
			c = lex_read(lex);
			continue;
		}

		c = lex_get(lex);
		switch (c) {
			case '"': 
			case '\\': 
			case '/':
				t = c;
				break;

			case 'b': t = '\b'; break;
			case 'f': t = '\f'; break;
			case 'n': t = '\n'; break;
			case 'r': t = '\r'; break;
			case 't': t = '\t'; break;

			default:
				printf("Invalid Escape value: %c!\n", c);
				return;
		}
		lex_replace(lex, t);

		c = lex_read(lex);
	}

	lex_replace(lex, '\0');	/* overwite the last \" */
	token = lex_token(lex);
	lex->value.string = strdup(token + 1);	/* skip the first \" */
	if (!lex->value.string)
		return;

	lex->result = TOKEN_STRING;
}

static void lex_scan_number(lex_t *lex, char c)
{
	double real;
	char *token, *end;

	lex->result = TOKEN_INVALID;

	if (c == '-')
		c = lex_read(lex);

	if (c == '0') {
		c = lex_read(lex);
		if (isdigit(c))
			return;
	} else if (isdigit(c)) {
		c = lex_read(lex);
		while (isdigit(c))
			c = lex_read(lex);
	} else {
		return;
	}

	if (c != '.' && c != 'E' && c != 'e') {
		int64 integer;

		lex_back(lex);
		token = lex_token(lex);
		errno = 0;
		integer = strtoll(token, &end, 10);
		if (errno == ERANGE)
			return;

		lex->result = TOKEN_INTEGER;
		lex->value.integer = integer;
		return;
	}

	if (c == '.') {
		c = lex_get(lex);
		if (!isdigit(c))
			return;
		lex_save(lex, c);

		c = lex_read(lex);
		while (isdigit(c))
			c = lex_read(lex);
	}

	if (c == 'E' || c == 'e') {
		c = lex_read(lex);
		if (c == '+' || c == '-')
			c = lex_read(lex);

		if (!isdigit(c))
			return;

		c = lex_read(lex);
		while (isdigit(c))
			c = lex_read(lex);
	}

	lex_back(lex);
	token = lex_token(lex);
	errno = 0;
	real = strtod(token, &end);
	if (errno == ERANGE && real != 0)
		return;

	lex->result = TOKEN_REAL;
	lex->value.real = real;
}

static void lex_scan(lex_t *lex)
{
	char c;

	token_init(&lex->token);

	if (lex->result == TOKEN_STRING && lex->value.string != NULL) {
		free(lex->value.string);
		lex->value.string = NULL;
	}

	c = lex_get(lex);
	while (c == ' ' || c == '\t' || c == '\n' || c == '\r')
		c = lex_get(lex);

	if (c == (char)EOF) {
		lex->result = TOKEN_EOF;
		return;
	}

	lex_save(lex, c);

	if (c == '{' || c == '}' || c == '[' || c == ']' || c == ':' || c == ',') {
		lex->result = c;
	} else if (c == '"') {
		lex_scan_string(lex);
	} else if (isdigit(c) || c == '-') {
		lex_scan_number(lex, c);
	} else if (isalpha(c)) {
		char *token;

		c = lex_read(lex);
		while (isalpha(c))
			c = lex_read(lex);
		lex_back(lex);

		token = lex_token(lex);
		if (strcasecmp(token, "true") == 0)
			lex->result = TOKEN_TRUE;
		else if(strcasecmp(token, "false") == 0)
			lex->result = TOKEN_FALSE;
		else if(strcasecmp(token, "null") == 0)
			lex->result = TOKEN_NULL;
		else
			lex->result = TOKEN_INVALID;
	} else {
		lex->result = TOKEN_INVALID;
	}
}

static json_t *parse_value(lex_t *lex);

static json_t *parse_object(lex_t *lex)
{
	json_t *object = json_object();
	if (!object)
		return NULL;

	lex_scan(lex);
	if (lex->result == '}')
		return object;

	for (;;) {
		char *key;
		json_t *value;

		if (lex->result != TOKEN_STRING)
			goto err;

		key = lex->value.string;
		lex->value.string = NULL;

		lex_scan(lex);
		if (lex->result != ':') {
			free(key);
			goto err;
		}

		lex_scan(lex);
		value = parse_value(lex);
		if (!value) {
			free(key);
			goto err;
		}

		if (!json_object_add(object, key, value)) {
			free(key);
			goto err;
		}

		free(key);

		lex_scan(lex);
		if (lex->result != ',')
			break;

		lex_scan(lex);
	}

	if (lex->result != '}')
		goto err;

	return object;

err:
	json_free(object);
	return NULL;
}

static json_t *parse_array(lex_t *lex)
{
	json_t *array = json_array();
	if (!array)
		return NULL;

	lex_scan(lex);
	if (lex->result == ']')
		return array;

	while (lex->result != TOKEN_EOF && lex->result != TOKEN_INVALID) {
		json_t *elem = parse_value(lex);
		if (!elem)
			goto err;

		json_array_add(array, elem);

		lex_scan(lex);
		if (lex->result != ',')
			break;

		lex_scan(lex);
	}

	if (lex->result != ']')
		goto err;

	return array;

err:
	json_free(array);
	return NULL;
}

static json_t *parse_value(lex_t *lex)
{
	switch (lex->result) {
	case TOKEN_STRING:
		return json_string(lex->value.string);

	case TOKEN_INTEGER:
		return json_integer(lex->value.integer);

	case TOKEN_REAL:
		return json_real(lex->value.real);
			
	case TOKEN_TRUE:
		return json_true();

	case TOKEN_FALSE:
		return json_false();

	case TOKEN_NULL:
		return json_null();

	case '{':
		return parse_object(lex);

	case '[':
		return parse_array(lex);

	default:
		return NULL;
	}

	return NULL;
}

json_t *json_parse(char *string)
{
	lex_t lex;
	json_t *result = NULL;

	lex.token._buf = malloc(TOKENSZ + 1);
	lex.token._len = TOKENSZ;
	lex.token._max_len = TOKENSZ;
	lex.value.integer = 0;
	lex.value.real = 0;
	lex.value.string = NULL;
	
	lex_init(&lex, string);
	lex_scan(&lex);
	if (lex.result != '[' && lex.result != '{')
		goto out;

	result = parse_value(&lex);
	if (!result)
		goto out;

	lex_scan(&lex);
	if (lex.result != TOKEN_EOF) {
		json_free(result);
		result = NULL;
	}

out:
	lex_close(&lex);
	free(lex.token._buf);
	return result;
}

json_t *json_parse_with_len(char *string, int len)
{
	lex_t lex;
	json_t *result = NULL;

	lex.token._buf = NULL;
	lex.token._buf = malloc(len + 1);

	if(NULL == lex.token._buf)
	{
		printf("\n%s %d %s malloc %d fail\n", __FILE__, __LINE__, __FUNCTION__, len);
		return NULL;
	}

	memset(lex.token._buf, 0, len+1);

	lex.token._len = len;
	lex.token._max_len = len;
	lex.value.integer = 0;
	lex.value.real = 0;
	lex.value.string = NULL;

	lex_init(&lex, string);
	lex_scan(&lex);
	if (lex.result != '[' && lex.result != '{')
		goto out;

	result = parse_value(&lex);
	if (!result)
		goto out;

	lex_scan(&lex);
	if (lex.result != TOKEN_EOF) {
		json_free(result);
		result = NULL;
	}

out:
	lex_close(&lex);
	free(lex.token._buf);
	return result;
}


/****************************************/

typedef struct {
	char *data;			/* pointer to write from */
	int   data_avail;	/* bytes available in data buffer */
	char *buff;			/* start of data buffer */
} strbuff_t;

static char *strbuf_alloc(strbuff_t *strbuf, int size)
{
	if (size > strbuf->data_avail) {
		return NULL;
	} else {
		void *ptr = strbuf->data;
		strbuf->data += size;
		strbuf->data_avail -= size;
		return ptr;
	}
}

static void strbuf_init(strbuff_t *strbuf, char *buf, int sz)
{
	strbuf->buff = buf;
	strbuf->data = buf;
	strbuf->data_avail = sz - 1; /* Reserve one for '\0' */
}

static int strbuf_append(strbuff_t *strbuf, char *str, int len)
{
	char *ptr;

	ptr = strbuf_alloc(strbuf, len);
	if (ptr == NULL)
		return 0;

	memcpy(ptr, str, len);
	return 1;
}

static inline void strbuf_finish(strbuff_t *strbuf)
{
	strbuf->data[0] = '\0';
}

static int format_string(strbuff_t *strbuf, char *str)
{
	char *esc;
	char c, *pos, *end;

	if (str == NULL || !strbuf_append(strbuf, "\"", 1))
		return 0;

	end = pos = str;
	for (;;) {
		while ((c = *end) != '\0') {
			if (c == '\\' || c == '"' || c < 0x20)
				break;

			end++;
		}

		if (end == pos)
			break;

		if (!strbuf_append(strbuf, pos, end - pos))
			return 0;

		if (c == '\0')
			break;

		switch (c) {
		case '\\': esc = "\\\\"; break;
		case '\"': esc = "\\\""; break;
		case '\b': esc = "\\b"; break;
		case '\f': esc = "\\f"; break;
		case '\n': esc = "\\n"; break;
		case '\r': esc = "\\r"; break;
		case '\t': esc = "\\t"; break;
		default:
			printf("JSON Invalid : %d\n", c);
			return 0;
		}
		if (!strbuf_append(strbuf, esc, 2))
			return 0;

		end++;
		pos = end;
	}

	return strbuf_append(strbuf, "\"", 1);
}

static int do_format(json_t *json, strbuff_t *strbuf)
{
	switch (json->type) {
	case JSON_NULL:
		return strbuf_append(strbuf, "null", 4);

	case JSON_TRUE:
		return strbuf_append(strbuf, "true", 4);
	
	case JSON_FALSE:
		return strbuf_append(strbuf, "false", 5);

	case JSON_STRING:
		return format_string(strbuf, json_string_value(json));

	case JSON_INTEGER:
	{
		int  size;
		char data[128];

		size = snprintf_s_ll(data, sizeof(data), "%lld", json_integer_value(json));
		if (size >= sizeof(data))
			return 0;

		return strbuf_append(strbuf, data, size);
	}
				
	case JSON_REAL:
	{
		int  size;
		char data[128];

		size = snprintf_s_g(data, sizeof(data), "%.16g", json_real_value(json));
		if (size >= sizeof(data) || size < 0)
			return 0;

		if (strchr(data, '.') == NULL && strchr(data, 'e') == NULL) {
			if (size + 2 >= sizeof(data))
				return 0;

			data[size]     = '.';
			data[size + 1] = '0';
			size += 2;
		}

		return strbuf_append(strbuf, data, size);
	}

	case JSON_OBJECT:
	{
		json_pair_t *pair;
		json_object_t *obj = json_to_object(json);

		if (!strbuf_append(strbuf, "{", 1))
			return 0;

		for (pair = obj->next; pair != NULL; pair = pair->next) {
			if (!format_string(strbuf, pair->name) ||
			    !strbuf_append(strbuf, ": ", 2))
				return 0;

			if (!do_format(pair->value, strbuf))
				return 0;

			if (pair->next != NULL && !strbuf_append(strbuf, ",", 1))
				return 0;
		}

		return strbuf_append(strbuf, "}", 1);
	}

	case JSON_ARRAY:
	{
		json_t *elem;
		json_array_t *array = json_to_array(json);

		if (!strbuf_append(strbuf, "[", 1))
			return 0;

		for (elem = array->next; elem != NULL; elem = elem->next) {
			if (!do_format(elem, strbuf))
				return 0;

			if (elem->next != NULL && !strbuf_append(strbuf, ",", 1))
				return 0;
		}

		return strbuf_append(strbuf, "]", 1);
	}

	default:
		break;
	}

	return 0;
}

int json_format(json_t *json, char *output, int sz)
{
	strbuff_t strbuf;

	/* '{}' '[]' */
	if ((sz < 3) || (json->type != JSON_OBJECT && json->type != JSON_ARRAY))
		return 0;

	strbuf_init(&strbuf, output, sz);

	if (!do_format(json, &strbuf))
		return 0;

	strbuf_finish(&strbuf);

	return strbuf.data - strbuf.buff;
}

void json_print(json_t *json)
{
	switch (json->type) {
	case JSON_NULL:
		printf("null");
		return;

	case JSON_TRUE:
		printf("true");
		return;

	case JSON_FALSE:
		printf("false");
		return;

	case JSON_INTEGER:
		printf("%lld", json_integer_value(json));
		return;

	case JSON_REAL:
		printf("%.16g", json_real_value(json));
		return;

	case JSON_STRING: 
	{
		char *tmp = json_string_value(json);
		if (tmp)
			printf("\"%s\"", tmp);
		return;
	}
	
	case JSON_OBJECT:
	{
		json_pair_t *pair;
		json_object_t *obj = json_to_object(json);

		printf("{\n");
		for (pair = obj->next; pair != NULL; pair = pair->next) {
			printf("\"%s\": ", pair->name);
			json_print(pair->value);

			if (pair->next != NULL)
				printf(",\n");
		}
		printf("}\n");

		return;
	}

	case JSON_ARRAY:
	{
		json_t *elem;
		json_array_t *array = json_to_array(json);

		printf("[\n");
		for (elem = array->next; elem != NULL; elem = elem->next) {
			json_print(elem);

			if (elem->next != NULL)
				printf(",\n");
		}
		printf("]\n");

		return;
	}

	default:
		break;
	}
}

/*****************************************************************************/
/*****************************************************************************/

static inline void json_init(json_t *json, json_type type)
{
	json->type = type;
	json->next = NULL;
}

void json_free(json_t *json)
{
	if (NULL == json)
	{
		printf("\n%s:%d:%s return NULL\n", __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	if (json->type == JSON_OBJECT) {
		json_pair_t   *pair;
		json_object_t *object = json_to_object(json);

		while ((pair = object->next) != NULL) {
			object->next = pair->next;

			json_free(pair->value);
			free(pair->name);
			free(pair);
		}
		free(object);
	} else if (json->type == JSON_ARRAY) {
		json_t *item;
		json_array_t *array = json_to_array(json);

		while ((item = array->next) != NULL) {
			array->next = item->next;

			json_free(item);
		}
		free(array);
	} else if (json->type == JSON_STRING) {
		json_string_t *string = json_to_string(json);
		free(string->value);
		free(string);
	}
	else {
		free(json);
	}
}


json_t *json_object(void)
{
	json_object_t *object = malloc(sizeof(json_object_t));
	if (!object)
		return NULL;
	json_init(&object->json, JSON_OBJECT);

	object->next = NULL;
	object->tail = NULL;

	return &object->json;
}

int json_object_remove(json_t *json, char *key)
{
	json_pair_t   *pair;
	json_pair_t   *pre;
	json_pair_t   *rm_pair = NULL;
	json_object_t *object = NULL;

	if (NULL == json)
	{
		printf("\n%s:%d:%s return NULL\n", __FILE__, __LINE__, __FUNCTION__);
		return 0;
	}

	if (json->type != JSON_OBJECT)
		return 0;

	object = json_to_object(json);
	for (pair = object->next, pre = NULL; pair != NULL; pre = pair, pair = pair->next) {
		if (strcmp(pair->name, key) == 0) {
			rm_pair = pair;
			break;
		}
	}
	if (rm_pair == NULL)
		return 0;

	if (pre != NULL)
		pre->next = rm_pair->next;
	if(object->tail == rm_pair)
		object->tail = pre;
	if(object->next == rm_pair)
		object->next = rm_pair->next;
	
	json_free(rm_pair->value);
	free(rm_pair->name);
	free(rm_pair);

	return 1;

}

json_t *json_object_get(const json_t *json, const char *key)
{
	json_pair_t   *pair;
	json_object_t *object;

	if (NULL == json)
	{
		printf("\n%s:%d:%s return NULL\n", __FILE__, __LINE__, __FUNCTION__);
		return NULL;
	}

	if (json->type != JSON_OBJECT)
		return NULL;

	object = json_to_object(json);
	for (pair = object->next; pair != NULL; pair = pair->next) {
		if (strcmp(pair->name, key) == 0)
			return pair->value;
	}

	return NULL;
}

int json_object_add(json_t *json, const char *key, json_t *value)
{
	json_pair_t   *pair;
	json_object_t *object;

	if (NULL == json)
	{
		printf("\n%s:%d:%s return NULL\n", __FILE__, __LINE__, __FUNCTION__);
		return 0;
	}

	if (json->type != JSON_OBJECT || value == NULL)
		return 0;

	object = json_to_object(json);
	for (pair = object->next; pair != NULL; pair = pair->next) {
		if (strcmp(pair->name, key) == 0)
			return 0;
	}

	if ((pair = malloc(sizeof(*pair))) == NULL)
		return 0;

	if ((pair->name = strdup(key)) == NULL) {
		free(pair);
		return 0;
	}

	pair->value = value;
	pair->next = NULL;

	if (object->tail == NULL) {
		object->next = pair;
		object->tail = pair;
	} else {
		object->tail->next = pair;
		object->tail = pair;
	}

	return 1;
}

json_t *json_array(void)
{
	json_array_t *array = malloc(sizeof(json_array_t));
	if (!array)
		return NULL;
	json_init(&array->json, JSON_ARRAY);

	array->size = 0;
	array->next = NULL;
	array->tail = NULL;

	return &array->json;
}

int json_array_size(json_t *json)
{
	if (NULL == json)
	{
		printf("\n%s:%d:%s return NULL\n", __FILE__, __LINE__, __FUNCTION__);
		return 0;
	}

	if (json->type != JSON_ARRAY)
		return 0;

	return json_to_array(json)->size;
}

json_t *json_array_get(json_t *json, int index)
{
	int i;
	json_t *elem;
	json_array_t *array;

	if (NULL == json)
	{
		printf("\n%s:%d:%s return NULL\n", __FILE__, __LINE__, __FUNCTION__);
		return NULL;
	}

	if (json->type != JSON_ARRAY)
		return NULL;

	array = json_to_array(json);
	for (i = 0, elem = array->next; i < array->size; i++, elem = elem->next) {
		if (i == index)
			return elem;
	}

	return NULL;
}

int json_array_add(json_t *json, json_t *value)
{
	json_array_t *array;

	if (NULL == json)
	{
		printf("\n%s:%d:%s return NULL\n", __FILE__, __LINE__, __FUNCTION__);
		return 0;
	}

	if (json->type != JSON_ARRAY || value == NULL)
		return 0;

	array = json_to_array(json);
	array->size++;
	if (array->tail == NULL) {
		array->next = value;
		array->tail = value;
	} else {
		array->tail->next = value;
		array->tail = value;
	}

	return 1;
}

json_t *json_string(const char *value)
{
	json_string_t *string = malloc(sizeof(json_string_t));
	if (string == NULL)
		return NULL;
	json_init(&string->json, JSON_STRING);

	string->value = strdup(value);
	if (!string->value) {
		free(string);
		return NULL;
	}

	return &string->json;
}

char *json_string_value(json_t *json)
{
	if (NULL == json)
	{
		printf("\n%s:%d:%s return NULL\n", __FILE__, __LINE__, __FUNCTION__);
		return NULL;
	}

	if (json->type != JSON_STRING)
		return NULL;

	return json_to_string(json)->value;
}

json_t *json_integer(int64 value)
{
	json_integer_t *integer = malloc(sizeof(json_integer_t));
	if (!integer)
		return NULL;
	json_init(&integer->json, JSON_INTEGER);

	integer->value = value;
	return &integer->json;
}

int64 json_integer_value(json_t *json)
{
	if (NULL == json)
	{
		printf("\n%s:%d:%s return NULL\n", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	
	if (json->type != JSON_INTEGER)
		return -1;

	return json_to_integer(json)->value;
}

int json_integer_set(json_t *json, int64 value)
{
	if (NULL == json)
	{
		printf("\n%s:%d:%s return NULL\n", __FILE__, __LINE__, __FUNCTION__);
		return 0;
	}

	if (json->type != JSON_INTEGER)
		return 0;

	json_to_integer(json)->value = value;

	return 1;
}

json_t *json_real(double value)
{
	json_real_t *real = malloc(sizeof(json_real_t));
	if (!real)
		return NULL;
	json_init(&real->json, JSON_REAL);

	real->value = value;
	return &real->json;
}

double json_real_value(json_t *json)
{
	if (NULL == json)
	{
		printf("\n%s:%d:%s return NULL\n", __FILE__, __LINE__, __FUNCTION__);
		return 0;
	}

	if (json->type != JSON_REAL)
		return 0;

	return json_to_real(json)->value;
}

int json_real_set(json_t *json, double value)
{
	if (json->type != JSON_REAL)
		return 0;

	json_to_real(json)->value = value;

	return 1;
}

double json_number_value(json_t *json)
{
	if (NULL == json)
	{
		printf("\n%s:%d:%s return NULL\n", __FILE__, __LINE__, __FUNCTION__);
		return 0;
	}

	if (json->type == JSON_INTEGER)
		return (double)json_to_integer(json)->value;
	else if(json->type == JSON_REAL)
		return json_to_real(json)->value;
	else
		return 0.0;
}

json_t *json_true(void)
{
	json_t *json = malloc(sizeof(json_t));
	if (!json)
		return NULL;
	json_init(json, JSON_TRUE);

	return json;
}

json_t *json_false(void)
{
	json_t *json = malloc(sizeof(json_t));
	if (!json)
		return NULL;
	json_init(json, JSON_FALSE);

	return json;
}

json_t *json_null(void)
{
	json_t *json = malloc(sizeof(json_t));
	if (!json)
		return NULL;
	json_init(json, JSON_NULL);

	return json;
}

int add_json_bool(json_t *result, const char *name, int input)
{
	json_t *rs_json;
	if (input == 1)
		rs_json = json_true();
	else 
		rs_json = json_false();
	json_object_add(result, name, rs_json);
	return 0;

}

int add_json_integer(json_t *result, const char *name, int input)
{
	json_t *rs_json = NULL;

	rs_json = json_integer(input);
	if (rs_json == NULL) {
		json_free(result);
		return -1;
	}
	json_object_add(result, name, rs_json);
	return 0;
}

int add_json_string(json_t *result, const char *name, const char *input)
{
	json_t *rs_json = NULL;

	rs_json = json_string(input);
	if (rs_json == NULL) {
		json_free(result);
		return -1;
	}
	json_object_add(result, name, rs_json);
	return 0;
}


