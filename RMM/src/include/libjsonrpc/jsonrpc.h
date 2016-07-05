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


#ifndef __LIBJSONRPC_JSONRPC_H__
#define __LIBJSONRPC_JSONRPC_H__
#include "libjson/json.h"
#include "libjsonrpc/jsonrpc_err.h"

#define JSONRPC_VER_STRING "2.0"

#define JSONRPC_MAX_PARAMS	64

#define JSONRPC_MAX_STRING_LEN (0x10000)	/* 64kB */


/* json.c return 1 for success */
#define JSON_SUCCESS (1)
#define JSON_FAILED	(0)

#define JSONRPC_SUCCESS (0)
#define JSONRPC_FAILED (-1)

typedef signed long long jrpc_data_integer;
typedef double jrpc_data_real;
typedef char *jrpc_data_string;


typedef struct jrpc_param {
	char *name;
	void *value;
	json_type value_type;
} jrpc_param_t;

typedef enum {
	JSONRPC_REQ_NORMAL,
	JSONRPC_REQ_NOTFICATION,
	JSONRPC_REQ_INVALID
} jrpc_req_type_t;

typedef enum {
	JSONRPC_RSP_RESULT,
	JSONRPC_RSP_ERROR,
	JSONRPC_RSP_INVALID
} jrpc_rsp_type_t;

typedef enum {
	JSONRPC_ID_TYPE_NORMAL,
	JSONRPC_ID_TYPE_EMPTY,
	JSONRPC_ID_TYPE_NULL,
	JSONRPC_ID_TYPE_INVALID
} jrpc_id_type_t;

typedef struct jrpc_rsp_pkg {
	json_t *json;
	jrpc_rsp_type_t rsp_type;
	int64 id;
	jrpc_id_type_t id_type;

	union {
		struct {
			/* void * value;
			json_type value_type; */
			json_t *value_obj;
		} result;
		struct {
			int64 code;
			char *message;
		} error;
	} data;
} jrpc_rsp_pkg_t;

typedef struct jrpc_req_pkg {
	json_t *json;
	jrpc_req_type_t req_type;
	int64 id;
	int64 node_id;
	jrpc_id_type_t id_type;
	char *method;
	uint8 num_of_params;
	jrpc_param_t params[JSONRPC_MAX_PARAMS];
} jrpc_req_pkg_t;

extern json_t *jrpc_init_req();
extern json_t *jrpc_init_rsp();
extern int jrpc_set_method(json_t *req, char *value);
extern int jrpc_set_id(json_t *pkg, int64 value, jrpc_id_type_t type);
extern int jrpc_add_params(json_t *req, json_t *params);
extern int jrpc_add_named_param(json_t *req, jrpc_param_t *param_t);
extern int jrpc_set_result(json_t *rsp, void *value, json_type value_type);
extern int jrpc_set_error(json_t *rsp, int64 code, char *message);
extern int jrpc_get_method(json_t *req, char **value);
extern int jrpc_format_string(json_t *json, char *string, int size);
extern int jrpc_create_rsp_from_struct(jrpc_rsp_pkg_t *pkg);
/* parameter structure: By-position */
extern int jrpc_get_array_param_value(json_t *req, uint8 index, json_type type, void *value);
/* parameter structure: By-name */
extern int jrpc_get_named_param_value(json_t *req, char *name, json_type type, void *value);
extern int jrpc_get_id(json_t *pkg, jrpc_id_type_t *type, int64 *value);
extern int jrpc_parse_req(char *string, json_t **req, jrpc_req_type_t *type);
extern int jrpc_parse_rsp(char *string, json_t **rsp, jrpc_rsp_type_t *type);
extern json_t *jrpc_create_req(int64 id, char *method, uint8 num_of_params, jrpc_param_t *param_t);
extern json_t *jrpc_create_notify(char *method, uint8 num_of_params, jrpc_param_t *param_t);
extern json_t *jrpc_create_result_rsp(int64 id, jrpc_id_type_t id_type, void *value, json_type value_type);
extern json_t *jrpc_create_error_rsp(int64 id, jrpc_id_type_t id_type, int64 code, char *message);
extern char *jrpc_create_req_string(int64 id, char *method, uint8 num_of_params, jrpc_param_t *param_t);
extern char *jrpc_create_notify_string(char *method, uint8 num_of_params, jrpc_param_t *param_t);
extern int jrpc_create_result_rsp_string(int64 id, jrpc_id_type_t id_type, void *value, json_type value_type, char *str_rsp);
extern int  jrpc_create_error_rsp_string(int64 id, jrpc_id_type_t id_type, int64 code, char *err_msg, char *str_rsp);
extern void jrpc_free_string(char *string);
extern json_t *jrpc_get_result(json_t *rsp);
extern int jrpc_get_result_value(json_t *rsp, json_type type, void *value);
extern int jrpc_get_named_result_value(json_t *rsp, char *name, json_type type, void *value);
extern int jrpc_get_error(json_t *rsp, int64 *code, char **message);
extern int jrpc_parse_rsp_to_struct(char *string, jrpc_rsp_pkg_t *pkg);
extern void jrpc_rsp_pkg_free(jrpc_rsp_pkg_t *pkg);
extern void jrpc_req_pkg_free(jrpc_req_pkg_t *pkg);

#endif
