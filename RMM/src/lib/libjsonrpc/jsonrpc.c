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
#include "libjsonrpc/jsonrpc.h"
#include "libsecurec/safe_lib.h"



json_t * jrpc_init_pkg()
{
	json_t * ver = NULL;
	json_t *req = json_object();

	if (NULL != req) {
		ver = json_string("2.0");
		if (NULL != ver) {
			if (json_object_add(req, "jsonrpc", ver) != JSON_SUCCESS)	
				return NULL;
			else 
				return req;
		}
		else
			json_free(req);
	}

	return NULL;
}

json_t * jrpc_init_req()
{
	return jrpc_init_pkg();
}

json_t * jrpc_init_rsp()
{
	return jrpc_init_pkg();
}

int jrpc_set_method(json_t * req, char * value)
{
	json_t * method = NULL;
	
	if (NULL == value || *value == '\0' || NULL == req ||
		NULL == (method = json_string(value)) ||
		json_object_add(req, "method", method) != JSON_SUCCESS)
		goto error;

	return JSONRPC_SUCCESS;

error:
	json_free(method);
	return JSONRPC_FAILED;
}

int jrpc_set_id(json_t * pkg, int64 value, jrpc_id_type_t type)
{
	json_t * id = NULL;

	if(type < JSONRPC_ID_TYPE_NORMAL || type > JSONRPC_ID_TYPE_NULL)
		goto error;
		
	if (JSONRPC_ID_TYPE_EMPTY == type)
		return JSONRPC_SUCCESS;
		
	if (NULL == pkg || 
		(type == JSONRPC_ID_TYPE_NULL && NULL == (id = json_null())) ||
		(type == JSONRPC_ID_TYPE_NORMAL && NULL == (id = json_integer(value)))||
		json_object_add(pkg, "id", id) != JSON_SUCCESS)
		goto error;

	return JSONRPC_SUCCESS;

error:
	json_free(id);
	return JSONRPC_FAILED;
}

#if 0
int jrpc_set_lockid(json_t * req, lock_id_t value)
{
	if (NULL == req)
		return JSONRPC_FAILED;

	json_t * lock_id = json_integer(value);
	if (NULL != lock_id) {
		if (json_object_add(req, "lock_id", lock_id) != JSON_SUCCESS)
			goto error;
	}

	return JSONRPC_SUCCESS;

error:
	json_free(lock_id);
	return JSONRPC_FAILED;
}
#endif

int jrpc_add_params(json_t * req, json_t * params)
{
	if (NULL == req || NULL == params || json_object_add(req, "params", params) != JSON_SUCCESS)
		return JSONRPC_FAILED;
	
	return JSONRPC_SUCCESS;
}

int jrpc_add_named_param(json_t * req, jrpc_param_t * param_t)
{
	if (NULL == param_t->name || *param_t->name == '\0' || NULL == req ||
		(param_t->value_type != JSON_TRUE && param_t->value_type != JSON_FALSE && param_t->value_type != JSON_NULL && NULL == param_t->value) ||
		(param_t->value_type == JSON_STRING && *(char *)param_t->value == '\0'))
		return JSONRPC_FAILED;
	
	json_t * param = NULL;
	json_t * params = json_object_get(req, "params");
	if (NULL == params) {
		params = json_object();
		if (NULL == params || json_object_add(req, "params", params) != JSON_SUCCESS) {
			json_free(params);
			return JSONRPC_FAILED;
		}
	}

	switch (param_t->value_type) {
	case JSON_STRING:
		param = json_string((char *)param_t->value);
		break;
	case JSON_INTEGER:
		param = json_integer(*(int64 *)param_t->value);
		break;
	case JSON_REAL:
		param = json_real(*(double *)param_t->value);
		break;
	case JSON_TRUE:
		param = json_true();
		break;
	case JSON_FALSE:
		param = json_false();
		break;

	case JSON_OBJECT:
	case JSON_ARRAY:
		param = (json_t *)param_t->value;
		break;
		
	case JSON_NULL:
		param = json_null();;
		break;
	
    default:
        return JSONRPC_FAILED;        /* wrong type */
	}

	if (NULL == param || json_object_add(params, param_t->name, param) != JSON_SUCCESS) {
		json_free(param);
		return JSONRPC_FAILED;
	}

	return JSONRPC_SUCCESS;
}


int jrpc_set_result(json_t * rsp, void * value, json_type value_type)
{
	if (NULL == rsp ||
		(value_type != JSON_TRUE && value_type != JSON_FALSE && value_type != JSON_NULL && NULL == value) ||
		(value_type == JSON_STRING && *(char *)value == '\0'))
		return JSONRPC_FAILED;
	
	json_t * result = NULL;

	switch (value_type) {
	case JSON_STRING:
		result = json_string((char *)value);
		break;
	case JSON_INTEGER:
		result = json_integer(*(int64 *)value);
		break;
	case JSON_REAL:
		result = json_real(*(double *)value);
		break;
	case JSON_TRUE:
		result = json_true();
		break;
	case JSON_FALSE:
		result = json_false();
		break;
	case JSON_OBJECT:
	case JSON_ARRAY:
		result = (json_t *)value;
		break;
		
	case JSON_NULL:
		result = json_null();
		break;
		
	default:
		return JSONRPC_FAILED;		 /* wrong type */
	}

	if (NULL == result || json_object_add(rsp, "result", result) != JSON_SUCCESS) {
		json_free(result);
		return JSONRPC_FAILED;
	}

	return JSONRPC_SUCCESS;
}


int jrpc_set_error(json_t * rsp, int64 code, char * message)
{
	if (NULL == rsp || NULL == message || *message == '\0')
		return JSONRPC_FAILED;

	json_t * err = json_object();
	json_t * err_code = json_integer(code);
	json_t * err_msg = json_string(message);
	
	if (NULL != err && NULL != err_code && NULL != err_msg) {
		if(json_object_add(rsp, "error", err) != JSON_SUCCESS || 
			json_object_add(err, "code", err_code) != JSON_SUCCESS || 
			json_object_add(err, "message", err_msg) != JSON_SUCCESS) {
			json_free(err);
			json_free(err_code);
			json_free(err_msg);
			return JSONRPC_FAILED;
		}
	}
	
	return JSONRPC_SUCCESS;
}

int jrpc_verify_string(char * string)
{
	unsigned int i = 0;
	unsigned int first_matched = 0;
	unsigned char starter = 0;
	signed int checker_brace = 0;   /* checker of '{' '}'*/
	signed int checker_bracket = 0;     /* checker of '['  ']' */
	signed int checker_quote = 0;   /* checked of '"' */

	/* check string format */
	if (NULL == string || *string != '{')
		return JSONRPC_FAILED;

	/* check string validity */
	for (i = 0; i < strnlen_s(string, JSONRPC_MAX_STRING_LEN); i++) {
		switch (string[i]) {
		case '{':
			checker_brace++;
			break;
		case '}':
			checker_brace--;
			break;
		case '[':
			checker_bracket++;
			break;
		case ']':
			checker_bracket--;
			break;
		case '"':
			checker_quote++;
			break;

		default:
			break;
		}
	}

	if (checker_brace || checker_bracket || (checker_quote%2 != 0)) {
		printf("str invalid. brace[%d] bracket[%d] quote[%d]\n",
			checker_brace, checker_bracket, checker_quote);
		return JSONRPC_FAILED;
	}

	return JSONRPC_SUCCESS;
}

int jrpc_verify_version(json_t * object)
{
	char * ver_str = NULL;
	json_t * ver = json_object_get(object, "jsonrpc");

	if (NULL == ver)
		return JSONRPC_FAILED;

	ver_str = json_string_value(ver);
	if(NULL == ver_str)
		return JSONRPC_FAILED;

	if (0 != strcmp(ver_str, JSONRPC_VER_STRING))
		return JSONRPC_FAILED;

	return JSONRPC_SUCCESS;
}

json_t * jrpc_parse_string(char * string)
{
	json_t * obj = NULL;

	if(jrpc_verify_string(string) ||
		NULL == (obj = json_parse(string)) ||
		jrpc_verify_version(obj)) {
		if (obj) {
			json_free(obj);
		}
		return NULL;
	}

	return obj;
}

int jrpc_parse_req(char * string, json_t ** req, jrpc_req_type_t * type)
{
	json_t * method = NULL;
	json_t * value = jrpc_parse_string(string);

	/* method should always exist */
	if (NULL == value || 
		NULL == (method = json_object_get(value, "method")))
		return JSONRPC_FAILED;

	if (NULL == json_object_get(value, "id"))
		*type = JSONRPC_REQ_NOTFICATION;
	else
		*type = JSONRPC_REQ_NORMAL;
	
	*req = value;
	return JSONRPC_SUCCESS;
}




#if 0
json_t * jrpc_do_create_req_with_param_object(jrpc_req_type_t type, int64 id, char * method, json_t * params, lock_id_t lock_id)
{
	int rc = 1;
	json_t * req = NULL;
	if (NULL == (req = jrpc_init_req()) ||
		((type == JSONRPC_REQ_NORMAL) && (rc = jrpc_set_id(req, id, JSONRPC_ID_TYPE_NORMAL))) ||
		(rc = jrpc_set_method(req, method)) ||
		(rc = jrpc_add_params(req, params)) ||
		(rc = jrpc_set_lockid(req, lock_id))) {
		json_free(req);
		return NULL;
	}

	return req;
}

json_t * jrpc_create_req_with_param_object(int64 id, char * method, json_t * params, lock_id_t lock_id)
{
	return jrpc_do_create_req_with_param_object(JSONRPC_REQ_NORMAL, id, method, params, lock_id);
}

json_t * jrpc_create_notify_with_param_object(char * method, json_t * params, lock_id_t lock_id)
{
	return jrpc_do_create_req_with_param_object(JSONRPC_REQ_NOTFICATION, 0, method, params, lock_id);
}
#endif

json_t * jrpc_do_create_req(jrpc_req_type_t type, int64 id, char * method, uint8 num_of_params, jrpc_param_t * param_t)
{
	int i = 0;
	int rc = 1;
	json_t * req = NULL;

	if (num_of_params > JSONRPC_MAX_PARAMS ||
		NULL == (req = jrpc_init_req()) ||
		((type == JSONRPC_REQ_NORMAL) && (rc = jrpc_set_id(req, id, JSONRPC_ID_TYPE_NORMAL))) ||
		(rc = jrpc_set_method(req, method)))
		goto error;

	/*  parameters for the rpc call MUST be provided as a Structured value. Either by-position through an Array or by-name through an Object. */
	/* parameter structure: By-position */
	if (num_of_params == 1 && param_t->name == NULL && (param_t->value_type == JSON_ARRAY || param_t->value_type == JSON_OBJECT)) {
		if (jrpc_add_params(req, (json_t *)param_t->value))
			goto error;
		else
			return req;
	}

	/* parameter structure: By-name */
	for (i=0; i<num_of_params; i++, param_t++) {
		if(jrpc_add_named_param(req, param_t))
			goto error;
	}

	return req;

error:
	if (req)
		json_free(req);
	return NULL;
}

char * jrpc_do_create_req_string(jrpc_req_type_t type, int64 id, char * method, uint8 num_of_params, jrpc_param_t * param_t)
{
	json_t * req = NULL;
	char * string = NULL;
	if (NULL != (req = jrpc_do_create_req(type, id, method, num_of_params, param_t))) {
		if (NULL != (string = malloc(JSONRPC_MAX_STRING_LEN))) {
			memset(string, 0, JSONRPC_MAX_STRING_LEN);
			if (JSONRPC_SUCCESS != jrpc_format_string(req, string, JSONRPC_MAX_STRING_LEN)) {
				free(string);
				string = NULL;
			}
		}
		json_free(req);
	}

	return string;
}


json_t * jrpc_create_req(int64 id, char * method, uint8 num_of_params, jrpc_param_t * param_t)
{
	return jrpc_do_create_req(JSONRPC_REQ_NORMAL, id, method, num_of_params, param_t);
}

json_t * jrpc_create_notify(char * method, uint8 num_of_params, jrpc_param_t * param_t)
{
	return jrpc_do_create_req(JSONRPC_REQ_NOTFICATION, 0, method, num_of_params, param_t);
}

char * jrpc_create_req_string(int64 id, char * method, uint8 num_of_params, jrpc_param_t * param_t)
{
	return jrpc_do_create_req_string(JSONRPC_REQ_NORMAL, id, method, num_of_params, param_t);
}

char * jrpc_create_notify_string(char * method, uint8 num_of_params, jrpc_param_t * param_t)
{
	return jrpc_do_create_req_string(JSONRPC_REQ_NOTFICATION, 0, method, num_of_params, param_t);
}

json_t * jrpc_create_result_rsp(int64 id, jrpc_id_type_t id_type, void * value, json_type value_type)
{
	json_t * rsp = NULL;

 	if (id_type == JSONRPC_ID_TYPE_EMPTY ||		/* id is required */
		NULL == (rsp = jrpc_init_rsp()) || 
		jrpc_set_id(rsp, id, id_type) ||
		jrpc_set_result(rsp, value, value_type)) {
		json_free(rsp);
		return NULL;
 	}

	return rsp;
}

json_t * jrpc_create_error_rsp(int64 id, jrpc_id_type_t id_type, int64 code, char * err_msg)
{
	json_t * rsp = NULL;

	if (id_type == JSONRPC_ID_TYPE_EMPTY ||		/* id is required */
		NULL == (rsp = jrpc_init_rsp()) || 
		jrpc_set_id(rsp, id, id_type) ||
		jrpc_set_error(rsp, code, err_msg)) {
		json_free(rsp);
		return NULL;
	}

	return rsp;
}

int jrpc_create_result_rsp_string(int64 id, jrpc_id_type_t id_type, void * value, json_type value_type, char *str_rsp)
{
	json_t * rsp = NULL;
	char * string = NULL;
	if (NULL == (rsp = jrpc_create_result_rsp(id, id_type, value, value_type)) ||
		jrpc_format_string(rsp, str_rsp, JSONRPC_MAX_STRING_LEN))
		return -1;

	json_free(rsp);
	return 0;
}

int jrpc_create_error_rsp_string(int64 id, jrpc_id_type_t id_type, int64 code, char * err_msg, char *str_rsp)
{
	json_t * rsp = NULL;
	char * string = NULL;
	if (NULL == (rsp = jrpc_create_error_rsp(id, id_type, code, err_msg)) ||
		jrpc_format_string(rsp, str_rsp, JSONRPC_MAX_STRING_LEN))
		return -1;

	json_free(rsp);
	return 0;
}
	
void jrpc_free_string(char * string)
{
	if (string)
		free(string);
	
	return;
}

int jrpc_format_string(json_t * json, char * string, int size)
{
	int str_len = 0;
	
	if (NULL == json || string == NULL || size > JSONRPC_MAX_STRING_LEN || size < 0 ||
		JSON_FAILED == (str_len = json_format(json, string, size)) ||
		str_len >= JSONRPC_MAX_STRING_LEN) {
		return JSONRPC_FAILED;
 	}

	return JSONRPC_SUCCESS;
}

#if 0	/* seems useless */
int jrpc_create_rsp_from_struct(jrpc_rsp_pkg_t * pkg)
{
	if (NULL == pkg || 
		(pkg->rsp_type != JSONRPC_RSP_RESULT && pkg->rsp_type != JSONRPC_RSP_ERROR))
		return JSONRPC_FAILED;

	if ((pkg->rsp_type == JSONRPC_RSP_RESULT &&
		NULL != (pkg->rsp = jrpc_create_result_rsp(pkg->id, pkg->id_type, pkg->data.result.value, pkg->data.result.value_type))) ||
		(pkg->rsp_type == JSONRPC_RSP_ERROR &&
		NULL != (pkg->rsp = jrpc_create_error_rsp(pkg->id, pkg->id_type, pkg->data.error.code, pkg->data.error.message))))
		return JSONRPC_SUCCESS;

	return JSONRPC_FAILED;
}
#endif

int jrpc_get_method(json_t * req, char ** value)
{
	json_t * method = NULL;

	if (NULL == req || 
		NULL == (method = json_object_get(req, "method")) ||
		NULL == (*value = json_string_value(method)) ||
		**value == '\0')
		return JSONRPC_FAILED; 

	return JSONRPC_SUCCESS;
}

int jrpc_get_json_value(json_t * json, void *value, json_type type)
{
	if (NULL == json || NULL == value || type < JSON_OBJECT || type > JSON_NULL)
		return JSONRPC_FAILED;

	switch (type) {
	case JSON_STRING:
		if (NULL == (*(char **)value = json_string_value(json)))
			goto error;
		break;
	case JSON_INTEGER:
		if ( -1 == (*(jrpc_data_integer *)value = json_integer_value(json)))	/* BUG: If integer is -1 ? */
			goto error;
		break;
	case JSON_REAL:
		if ( 0 == (*(jrpc_data_real *)value = json_real_value(json))) /* BUG: If real is 0? */
			goto error;
		break;
		
	case JSON_TRUE:
	case JSON_FALSE:
	case JSON_OBJECT:
	case JSON_ARRAY:	
	case JSON_NULL:
		*(json_t **)value = json;
		break;
		
	default:
		goto error;		 /* wrong type */
	}

	return JSONRPC_SUCCESS;

error:
	return JSONRPC_FAILED;
}

/* parameter structure: By-position */
int jrpc_get_array_param_value(json_t * req, uint8 index, json_type type, void * value)
{
	json_t * params = NULL;
	json_t * element = NULL;

	if (NULL == req || index > JSONRPC_MAX_PARAMS ||
		NULL == (params = json_object_get(req, "params")) ||
		NULL == (element = json_array_get(params, index)) ||
		jrpc_get_json_value(element, value, type))
		return JSONRPC_FAILED; 

	return JSONRPC_SUCCESS;
}

/* parameter structure: By-name */
int jrpc_get_named_param_value(json_t * req, char * name, json_type type, void * value)
{
	json_t * params = NULL;
	json_t * element = NULL;

	if (NULL == req || NULL == value || name == NULL || type < JSON_OBJECT || type > JSON_NULL ||
		NULL == (params = json_object_get(req, "params")) ||
		NULL == (element = json_object_get(params, name)) ||
		jrpc_get_json_value(element, value, type))
		return JSONRPC_FAILED;
	
	return JSONRPC_SUCCESS;
}

int jrpc_get_id(json_t * pkg, jrpc_id_type_t * type, int64 * value)
{
	int64 rc;
	json_t * id = NULL;

	if (NULL == pkg || 
		NULL == (id = json_object_get(pkg, "id")))
		goto error;

	if (id->type == JSON_NULL) {
		*type = JSONRPC_ID_TYPE_NULL;
		*value = 0;
	}
	else if (id->type == JSON_INTEGER) {
		*type = JSONRPC_ID_TYPE_NORMAL;
		*value = json_integer_value(id);	/* BUG: json_integer_value may return -1 as internal error */
	}
	else
		goto error;
		
	return JSONRPC_SUCCESS;

error:
	*value = 0;
	*type = JSONRPC_ID_TYPE_INVALID;
	return JSONRPC_FAILED;
}

#if 0
int jrpc_get_lock_id(json_t * req, lock_id_t * value)
{
	int64 rc;
	json_t * lock_id = NULL;

	if (NULL == req || 
		NULL == (lock_id = json_object_get(req, "lock_id")) ||
		-1 == (rc = json_integer_value(lock_id)))
		return JSONRPC_FAILED; 

	*value = rc;
	return JSONRPC_SUCCESS;
}
#endif

json_t * jrpc_get_result_object(json_t * rsp)
{
	json_t * result = NULL;

	if (NULL == rsp || 
		NULL == (result = json_object_get(rsp, "result")))
		return NULL; 
	
	return result;
}

int jrpc_get_result_value(json_t * rsp, json_type type, void * value)
{
	json_t * result = NULL;

	if (NULL == rsp || NULL == value || type < JSON_OBJECT || type > JSON_NULL ||
		NULL == (result = jrpc_get_result_object(rsp)) ||
		jrpc_get_json_value(result, value, type)) {
		return JSONRPC_FAILED; 
	}

	rsp = result;
	return JSONRPC_SUCCESS;
}

/* parameter structure: By-name */
int jrpc_get_named_result_value(json_t * rsp, char * name, json_type type, void * value)
{
	json_t * result = NULL;
	json_t * element = NULL;

	if (NULL == rsp || NULL == value || name == NULL || type < JSON_OBJECT || type > JSON_NULL ||
		NULL == (result = json_object_get(rsp, "result")) ||
		NULL == (element = json_object_get(result, name)) ||
		jrpc_get_json_value(element, value, type))
		return JSONRPC_FAILED;
	
	return JSONRPC_SUCCESS;
}


int jrpc_get_error(json_t * rsp, int64 * code, char ** message)
{
	json_t * error = NULL;
	json_t * error_code = NULL;
	json_t * error_msg = NULL;

	if (NULL == rsp || 
		NULL == (error = json_object_get(rsp, "error")) ||
		NULL == (error_code = json_object_get(error, "code")) ||
		NULL == (error_msg = json_object_get(error, "message")) ||
		NULL == (*message = json_string_value(error_msg)))
		return JSONRPC_FAILED; 

	*code = json_integer_value(error_code);	/* BUG: json_integer_value may return -1 for internal error */
	
	
	return JSONRPC_SUCCESS;
}

int jrpc_parse_rsp(char * string, json_t ** rsp, jrpc_rsp_type_t *type)
{
	json_t * value = jrpc_parse_string(string);

	if (NULL == value)
		return JSONRPC_FAILED;

	json_t * result = json_object_get(value, "result");
	json_t * error = json_object_get(value, "error");
	json_t * id = json_object_get(value, "id");

	/*
	1. id should exist
	2. result and error should not both exist
	3. either result or error should exist
	*/
	if ( !id || ( result && error) || (!result && !error)) {
		json_free(value);
		return JSONRPC_FAILED;
	}

	if (result)
		*type = JSONRPC_RSP_RESULT;
	else
		*type = JSONRPC_RSP_ERROR;

	*rsp = value;
	return JSONRPC_SUCCESS;
}

int jrpc_parse_rsp_to_struct(char *string, jrpc_rsp_pkg_t *pkg)
{
	if (NULL == string || pkg == NULL ||
		jrpc_parse_rsp(string, &pkg->json, &pkg->rsp_type) ||
		jrpc_get_id(pkg->json, &pkg->id_type, &pkg->id))
		return JSONRPC_FAILED;

	switch (pkg->rsp_type) {
	case JSONRPC_RSP_RESULT:
		if (NULL == (pkg->data.result.value_obj = jrpc_get_result_object(pkg->json)))
			return JSONRPC_FAILED;
		break;
	case JSONRPC_RSP_ERROR:
		if (jrpc_get_error(pkg->json, &pkg->data.error.code, &pkg->data.error.message))
			return JSONRPC_FAILED;
		break;
		
	default:
		return JSONRPC_FAILED;		 /* wrong type */
	}

	return JSONRPC_SUCCESS;
}

void jrpc_rsp_pkg_free(jrpc_rsp_pkg_t *pkg)
{
	if (pkg ==NULL)
		return;
	if(pkg->json) {
		json_free(pkg->json);
		pkg->json = NULL;
	}
}

void jrpc_req_pkg_free(jrpc_req_pkg_t *pkg)
{
	if (pkg ==NULL)
		return;
	if(pkg->json) {
		json_free(pkg->json);
		pkg->json = NULL;
	}
}


