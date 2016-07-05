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


#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "libjson/json.h"
#include "libjsonrpc/jsonrpc.h"


#define JSONRPC_TEST_MAX_STRING_LEN (2048)

int test_rsp()
{
	int i = 0;
	char * tmp_str = malloc(JSONRPC_MAX_STRING_LEN);
	json_t * rsp[10] = {0};
	jrpc_rsp_pkg_t pkg = {0};

	json_t * object = json_object();

	if (tmp_str == NULL) {
		printf("%s: malloc failed\n", __func__);
		return -1;
	}

	json_object_add(object, "ob1", json_integer(-21));
	json_object_add(object, "ob2", json_integer(839));
	json_object_add(object, "ob3", json_integer(563278));
	json_object_add(object, "ob4",json_string("rsp jsonrpc"));
	rsp[0] = jrpc_create_result_rsp(333, JSONRPC_ID_TYPE_NORMAL, object, JSON_OBJECT);


	jrpc_data_string string = "jsonrsp result";
	rsp[1] = jrpc_create_result_rsp(444, JSONRPC_ID_TYPE_NORMAL, (void *)string, JSON_STRING);

	jrpc_create_result_rsp_string(444, JSONRPC_ID_TYPE_NORMAL, (void *)string, JSON_STRING, tmp_str);
	printf("out put of jrpc_create_result_rsp_string:\n%s\n", tmp_str);

	jrpc_data_integer data = 212321;
	rsp[2] = jrpc_create_result_rsp(555, JSONRPC_ID_TYPE_NORMAL, (void *)&data, JSON_INTEGER);


	json_t * array = json_array();
	json_array_add(array, json_integer(12));
	json_array_add(array, json_integer(4343));
	json_array_add(array, json_integer(-123));
	rsp[3] = jrpc_create_result_rsp(666, JSONRPC_ID_TYPE_NORMAL, (void *)array, JSON_ARRAY);

	rsp[4] = jrpc_create_result_rsp(777, JSONRPC_ID_TYPE_NORMAL, NULL, JSON_TRUE);

	rsp[5] = jrpc_create_result_rsp(888, JSONRPC_ID_TYPE_NORMAL, NULL, JSON_FALSE);

	rsp[6] = jrpc_create_result_rsp(999, JSONRPC_ID_TYPE_NORMAL, NULL, JSON_NULL);

	jrpc_data_real data1 = -212321.312312;
	rsp[7] = jrpc_create_result_rsp(000, JSONRPC_ID_TYPE_NORMAL, (void *)&data1, JSON_REAL);

	rsp[8] = jrpc_create_error_rsp(123, JSONRPC_ID_TYPE_NORMAL, 8989, "error message 1");

	rsp[9] = jrpc_create_error_rsp(1233, JSONRPC_ID_TYPE_NULL, 8989434, "error message 2");

	jrpc_create_error_rsp_string(1233, JSONRPC_ID_TYPE_NULL, 8989434, "error message 2", tmp_str);
	printf("out put of jrpc_create_result_rsp_string:\n%s\n", tmp_str);
	jrpc_free_string(tmp_str);

	printf("\nrsp %d\n", rsp[0] == NULL);

	char buffer[JSONRPC_TEST_MAX_STRING_LEN] = {0};

	json_t * parsed_rsp= NULL;
	jrpc_rsp_type_t rsp_type = JSONRPC_RSP_INVALID;
	int64 id = 0;
	jrpc_id_type_t id_type;
	int64 p_integer = 0;
	double p_real = 0;
	char * p_string = NULL;
	json_t * p_json_true = NULL;
	json_t * p_json_false = NULL;
	json_t * p_json_array = NULL;
	json_t * p_json_object = NULL;
	json_t * p_json_null = NULL;

	int64 e_code = 0;
	char * e_msg = NULL;
	
	for (i=0; i<sizeof(rsp)/sizeof(rsp[0]); i++) {
		rsp_type = JSONRPC_RSP_INVALID;
		if (NULL != rsp[i]) {
			jrpc_format_string(rsp[i], buffer, sizeof(buffer));
			printf("\nrsp %d is : \n%s\n\n", i, buffer);
		}
		else
			printf("\nrsp %d is NULL.\n", i);

#if 1
		if (jrpc_parse_rsp(buffer, &parsed_rsp, &rsp_type))
			printf("\nparsed_rsp %d is null.\n", i);

		if (jrpc_get_id(parsed_rsp, &id_type, &id))
			printf("\nparsed_rsp id %lld. id type%d.\n", id, id_type);
#else

		if (jrpc_parse_rsp_to_struct(buffer, &pkg))
			printf("\nparsed_rsp %d is null.\n", i);
		
		parsed_rsp = pkg.rsp;
		rsp_type = pkg.rsp_type;
		id = pkg.id;
		id_type = pkg.id_type;
#endif
		
		printf("\nparsed_rsp id %lld. id type%d.\n", id, id_type);
		
		switch (i) {
		case 0:
			if (rsp_type != JSONRPC_RSP_RESULT || jrpc_get_result_value(parsed_rsp, JSON_OBJECT, (void *)&p_json_object))
				printf("jrpc_get_result_value error\n");
			break;
		case 1:
			if (rsp_type != JSONRPC_RSP_RESULT || jrpc_get_result_value(parsed_rsp, JSON_STRING, (void *)&p_string))
				printf("jrpc_get_result_value error\n");
			break;
		case 2:
			if (rsp_type != JSONRPC_RSP_RESULT || jrpc_get_result_value(parsed_rsp, JSON_INTEGER, (void *)&p_integer))
				printf("jrpc_get_result_value error\n");
			break;
		case 3:
			if (rsp_type != JSONRPC_RSP_RESULT || jrpc_get_result_value(parsed_rsp, JSON_ARRAY, (void *)&p_json_array))
				printf("jrpc_get_result_value error\n");
			break;
		case 4:
			if (rsp_type != JSONRPC_RSP_RESULT || jrpc_get_result_value(parsed_rsp, JSON_TRUE, (void *)&p_json_true))
				printf("jrpc_get_result_value error\n");
			break;
		case 5:
			if (rsp_type != JSONRPC_RSP_RESULT || jrpc_get_result_value(parsed_rsp, JSON_FALSE, (void *)&p_json_false))
			printf("jrpc_get_result_value error\n");
			break;
		case 6:	
			if (rsp_type != JSONRPC_RSP_RESULT || jrpc_get_result_value(parsed_rsp, JSON_NULL, (void *)&p_json_null))
				printf("jrpc_get_result_value error\n");
			break;
		case 7:
			if (rsp_type != JSONRPC_RSP_RESULT || jrpc_get_result_value(parsed_rsp, JSON_REAL, (void *)&p_real))
				printf("jrpc_get_result_value error\n");
			break;
		case 8:
			if (rsp_type != JSONRPC_RSP_ERROR || jrpc_get_error(parsed_rsp, &e_code, &e_msg))
				printf("jrpc_get_error error\n");
			if (e_msg != NULL)
				printf("case 8, err code:%lld, err msg:%s.\n", e_code, e_msg);
			break;
		case 9:
			if (rsp_type != JSONRPC_RSP_ERROR || jrpc_get_error(parsed_rsp, &e_code, &e_msg))
				printf("jrpc_get_error error\n");
			if (e_msg != NULL)
				printf("case 9, err code:%lld, err msg:%s.\n", e_code, e_msg);
			break;
			
		default:
			printf("should not be here! i = %d\n", i);		 /* wrong type */
		}
	}

	if (p_string != NULL)
		printf("rsp test, integer %lld, real %f, string:\"%s\"\n", p_integer, p_real, p_string);
	if (p_json_true != NULL && p_json_array != NULL &&  p_json_object != NULL && p_json_null != NULL)
		printf("rep test, true %d, array %d, object %d, null %d\n\n",
			p_json_true->type == JSON_TRUE,
			p_json_array->type == JSON_ARRAY,
			p_json_object->type == JSON_OBJECT,
			p_json_null->type == JSON_NULL);

	for (i=0; i<sizeof(rsp)/sizeof(rsp[0]); i++) {
		json_free(rsp[i]);	
	}
	json_free(parsed_rsp);

	return 0;
}


int test_req()
{
	jrpc_data_integer data0 = 12331;
	jrpc_data_real data1 = -31312.3211;
	jrpc_data_string string = "value of json string";
	
	jrpc_param_t param_t[7] = {{0}};

	param_t[0].name = "param0";
	param_t[0].value = (void *)&data0;
	param_t[0].value_type = JSON_INTEGER;

	param_t[1].name = "param1";
	param_t[1].value = (void *)&data1;
	param_t[1].value_type = JSON_REAL;

	param_t[2].name = "param2";
	param_t[2].value = NULL;
	param_t[2].value_type = JSON_TRUE;

	json_t * array = json_array();
	json_array_add(array, json_integer(-112));
	json_array_add(array, json_real(3213.323));
	json_array_add(array, json_string("param string"));
	param_t[3].name = "param3";
	param_t[3].value = (void *)array;
	param_t[3].value_type = JSON_ARRAY;
	jrpc_param_t param_array = {0};
	param_array.name = NULL;
	param_array.value = (void *)array;
	param_array.value_type = JSON_ARRAY;

	json_t * object = json_object();
	json_object_add(object, "ob1", json_integer(-11));
	json_object_add(object, "ob2", json_integer(89));
	json_object_add(object, "ob3", json_integer(5678));
	json_object_add(object, "ob4",json_string("hello jsonrpc"));
	param_t[4].name = "param4";
	param_t[4].value = (void *)object;
	param_t[4].value_type = JSON_OBJECT;

	
	param_t[5].name = "param5";
	param_t[5].value = (void *)string;
	param_t[5].value_type = JSON_STRING;
	
	param_t[6].name = "param6";
	param_t[6].value = NULL;
	param_t[6].value_type = JSON_NULL;


	#if 0
	/* test jrpc_create_req_string for normal req */
	char * tmp_str = NULL;
	tmp_str = jrpc_create_req_string(99, "req_with_list", sizeof(param_t)/sizeof(jrpc_param_t), param_t, 101);	/* named params */
	printf("out put of jrpc_create_req_string:\n%s\n", tmp_str);
	jrpc_free_string(tmp_str);
	return 0;
	#endif

	#if 0
	/* test jrpc_create_req_string for notification */
	char * tmp_str = NULL;
	tmp_str = jrpc_create_notify_string("notification_with_list", sizeof(param_t)/sizeof(jrpc_param_t), param_t, 1323);	/* named params */
	printf("out put of jrpc_create_req_string:\n%s\n", tmp_str);
	jrpc_free_string(tmp_str);
	return 0;
	#endif

	int i = 0;
	json_t * req[4];
	
	req[0] = jrpc_create_req(99, "req_with_list", sizeof(param_t)/sizeof(jrpc_param_t), param_t);	/* named params */
	req[1] = jrpc_create_req(100,"req_with_array", 1, &param_array);	/* param array */
	req[2] = jrpc_create_notify("notification_with_list", sizeof(param_t)/sizeof(jrpc_param_t), param_t);
	req[3] = jrpc_create_notify("notification_with_object",1, &param_array);


	char buffer[JSONRPC_TEST_MAX_STRING_LEN] = {0};
	jrpc_req_type_t req_type = JSONRPC_REQ_INVALID;
	jrpc_id_type_t id_type = JSONRPC_ID_TYPE_INVALID;
	json_t * parsed_req = NULL;
	char * method = NULL;
	int64 id = 0;
	int64 p_integer = 0;
	double p_real = 0;
	char * p_string = NULL;
	json_t * p_json_true = NULL;
	json_t * p_json_array = NULL;
	json_t * p_json_object = NULL;
	json_t * p_json_null = NULL;
	
	for (i=0; i<sizeof(req)/sizeof(req[0]); i++) {
		if (NULL != req[i]) {
			jrpc_format_string(req[i], buffer, sizeof(buffer));
			printf("\nreq %d is : \n%s\n\n", i, buffer);
		}
		else
			printf("\nreq %d is NULL.\n", i);

		if (jrpc_parse_req(buffer, &parsed_req, &req_type))
			printf("\nparsed_req %d is null.\n", i);
		else if (i==0 || i==2) {
			if (jrpc_get_method(parsed_req, &method))
				printf("jrpc_get_method error\n");
			if (jrpc_get_id(parsed_req, &id_type, &id))
				printf("jrpc_get_id error at %d\n", i);
			if (jrpc_get_named_param_value(parsed_req, "param0", JSON_INTEGER, (void *)&p_integer))
				printf("jrpc_get_named_param_value error\n");
			if (jrpc_get_named_param_value(parsed_req, "param1", JSON_REAL, (void *)&p_real))
				printf("jrpc_get_named_param_value error\n");
			if (jrpc_get_named_param_value(parsed_req, "param2", JSON_TRUE, (void *)&p_json_true))
				printf("jrpc_get_named_param_value error\n");
			if (jrpc_get_named_param_value(parsed_req, "param3", JSON_ARRAY, (void *)&p_json_array))
				printf("jrpc_get_named_param_value error\n");
			if (jrpc_get_named_param_value(parsed_req, "param4", JSON_OBJECT, (void *)&p_json_object))
				printf("jrpc_get_named_param_value error\n");
			if (jrpc_get_named_param_value(parsed_req, "param5", JSON_STRING, (void *)&p_string))
				printf("jrpc_get_named_param_value error\n");
			if (jrpc_get_named_param_value(parsed_req, "param6", JSON_NULL, (void *)&p_json_null))
				printf("jrpc_get_named_param_value error\n");
			if (method != NULL)
				printf("req%d, req type %d, method:\"%s\", id type %d, id %lld\n",i, req_type, method, id_type, id);
			if (p_string != NULL)
				printf("req%d, integer %lld, real %f, string:\"%s\"\n", i, p_integer, p_real, p_string);
			if (p_json_true != NULL && p_json_array != NULL &&	p_json_object != NULL && p_json_null != NULL)
				printf("req%d, true %d, array %d, object %d, null %d\n", i,
					p_json_true->type == JSON_TRUE,
					p_json_array->type == JSON_ARRAY,
					p_json_object->type == JSON_OBJECT,
					p_json_null->type == JSON_NULL);

			json_free(parsed_req);
		}
		else if (i==1) {
			if (jrpc_get_method(parsed_req, &method))
				printf("jrpc_get_method error\n");
			if (jrpc_get_id(parsed_req, &id_type, &id))
				printf("jrpc_get_id error\n");

			if (jrpc_get_array_param_value(parsed_req, 0, JSON_INTEGER, (void *)&p_integer))
				printf("jrpc_get_array_param_value error\n");
			if (jrpc_get_array_param_value(parsed_req, 1, JSON_REAL, (void *)&p_real))
				printf("jrpc_get_array_param_value error\n");
			if (jrpc_get_array_param_value(parsed_req, 2, JSON_STRING, (void *)&p_string))
				printf("jrpc_get_array_param_value error\n");
			
			if (method != NULL)
				printf("req1, req type %d, method:\"%s\", id type %d, id %lld\n", req_type, method, id_type, id);
			if (p_string != NULL)
				printf("req1, element 0:%lld, element 1:%f, element2:\"%s\"\n", p_integer, p_real, p_string);
		}
	}
	
	json_free(req[0]);
	
	
	return 0;
}
int main()
{
	test_req();
	test_rsp();

	return 0;
}

