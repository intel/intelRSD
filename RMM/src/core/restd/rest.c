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

#include "config.h"
#include "rest.h"
#include "libjson/json.h"
#include "handler/handler.h"
#include "libcfg/cfg.h"
#include "libutils/string.h"
#include "libsecurec/safe_lib.h"


struct http_response_status {
	int32 status;
	int8 *title;
	int8 report_error;
};

static struct rest_uri_node root_uri_node = {
	.list    = LIST_HEAD_INIT(root_uri_node.list),
	.subnode = LIST_HEAD_INIT(root_uri_node.subnode),

	.isvar   = 0,
	.rest_handler  = NULL,
};

static const struct http_response_status http_resp[] = {
    /* "no error" conditions */
	{HTTP_OK, "OK", false},
	{HTTP_CREATED, "Created", false},
	{HTTP_ACCEPTED, "Accepted", false},
	{HTTP_NO_CONTENT, "No Content", false},
	{HTTP_MOVED_PERMANENTLY, "Moved Permanently", false},
	{HTTP_FOUND, "Found", false},
	{HTTP_NOT_MODIFIED, "Not Modified", false},

    /* report error with rest-comilant json body */
	{HTTP_BAD_REQUEST, "Bad Request", true},
	{HTTP_FORBIDDEN, "Forbidden", true},
	{HTTP_UNAUTHORIZED, "Unauthorized", true},
	{HTTP_RESOURCE_NOT_FOUND, "Resource not found", true},
	{HTTP_METHOD_NOT_ALLOWED, "Method not allowed", true},
	{HTTP_NOT_ACCEPTABLE, "Not acceptable", true},
	{HTTP_CONFLICT, "Conflict", true},
	{HTTP_GONE, "Gone", true},
	{HTTP_LENGTH_REQUIRED, "Length Required", true},
	{HTTP_PRECONDITION_FAILED, "Precondition Failed", true},
	{HTTP_UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type", true},
	{HTTP_INTERNAL_SERVER_ERROR, "Application Error", true},
	{HTTP_NOT_IMPLEMENTED, "Not Implemented", true},
	{HTTP_SERVICE_UNAVAILABLE, "Service Unavailable", true}
};


static void compile_rest_uri(int32 num, int8 *ids[], const struct rest_handler *handler);
static int32 split_path(int8 *path, int8 *ids[]);
static int32 parse_query_params(int8 *query, struct rest_param_key *params);
static struct rest_uri_node *create_rest_node(int8 *name,
											  struct list_head *parent);
static void send_json_reply(int32 fd, int32 status, const int8 *title, const int8 *json, int32 jsonlen);
static void send_http_reply_no_body(int32 fd, int32 status, const int8 *title, struct rest_uri_param *param);
static struct rest_handler *lookup_rest_handle(struct rest_uri_param *param);
static json_t *handle_rest_req(struct rest_uri_param *param, const struct rest_handler *handler);


void rest_register_handlers(void)
{
	register_rack_handler();
	register_redfish_handler();
	register_manager_handler();
	register_mbp_handler();
	register_pzone_handler();
	register_psu_handler();
	register_drawer_handler();
	register_tzone_handler();
	register_fan_handler();
}

void register_handler(const int8 *url, const struct rest_handler *handler)
{
	int32 num;
	int8 *ids[MAX_URI_NODES];
	int8 path[MAX_HEADER_LEN];
	int8 prefix[PREFIX_LEN] = {0};
	int8 new_link[MAX_URL] = {0};
	printf("url is %s\n", url);
	rmm_cfg_get_rest_prefix(prefix, PREFIX_LEN);
	insert_str(new_link, url, 0, prefix);/// "http://%s:%d/rack/" --->http://%s:%d/prefix/rack/

	strncpy_safe(path, new_link, MAX_HEADER_LEN, MAX_HEADER_LEN - 1);
	num = split_path(path, ids);

	compile_rest_uri(num, ids, handler);
}

int8 *rest_path_value(const struct rest_uri_param *param, const int8 *name)
{
	int32 i;

	for (i = 0; i < param->num_path_keys; i++) {
		if (strcasecmp(param->path_keys[i].name, name) == 0)
			return param->path_keys[i].value;
	}

	return NULL;
}

int8 *rest_query_value(struct rest_uri_param *param, int8 *name)
{
	int32 i;

	for (i = 0; i < param->num_query_keys; i++) {
		if (strcasecmp(param->query_keys[i].name, name) == 0)
			return param->query_keys[i].value;
	}

	return NULL;
}

static int format_url(char *url, int len)
{
	int i = 0;
	int8  *find = NULL;

	if (NULL == url)
		return -1;

	/*remove tail '/'*/
	if (url[len - 1] == '/')
		url[len - 1] = 0;

	/*ignore first '/v1'*/
	for (i = 11; i < len; i++) {
		find = strchr(&url[i], '/');
		if (find == NULL)
			break;
		if (*(find + 1) > 'a' && *(find + 1) < 'z')
			*(find + 1) -= 32;
		i = find - url;
	}
	return 0;
}

#ifdef REST_HTTPS
static void split(char **arr, char *str, const char *del) {
	char *s = strtok(str, del);

	while(s != NULL) {
		*arr++ = s;
		s = strtok(NULL, del);
	}
}
#endif

void rest_process(struct http_request *req)
{
	json_t *result = NULL;
	struct rest_handler *handle;
	struct rest_uri_param param;
	int32 i = 0;

	param.httpmethod     = req->method;
	param.content_length = req->content_length;
	param.fd             = req->fd;
	format_url(req->path, strnlen_s(req->path, RSIZE_MAX_STR));
	strncpy_safe(param.url, req->path, HREF_URL_LEN, strnlen_s(req->path, RSIZE_MAX_STR));
	param.num_nodes      = split_path(req->path, param.nodes);
	param.num_query_keys = parse_query_params(req->query, param.query_keys);
	param.status         = HTTP_OK;
	param.host           = req->host;

	param.json_data_sz = req->json_end - req->json_start + 1;
	param.json_data = req->buff + req->json_start;

	/*
	 * Except for /v1, other URLs can only be accessed by https protocol.
	 * Redirect to corresponding https URL.
	 */
#ifdef REST_HTTPS
	if (param.host && (strncmp("127.0.0.1", param.host, strnlen_s("127.0.0.1", RSIZE_MAX_STR)) != 0) &&
			str_end_with(param.host, RACK_HTTPD_PORT_STR) &&
			param.url && !str_end_with(param.url, "/redfish/v1")) {
		char redirected_url[256];
		char *arr[2];
		int https_port;

		get_stunnel_ports(&https_port);
		if (https_port == -1)
			https_port = RACK_HTTPSD_PORT;

		bzero(redirected_url, sizeof(redirected_url));
		split(arr, param.host, ":");
		snprintf_s_sis(redirected_url, sizeof(redirected_url), "https://%s:%d%s", arr[0], https_port, param.url);
		snprintf_s_s(param.https_url, sizeof(param.https_url), "%s", redirected_url);
		send_json_reply_redirect(req->fd, redirected_url);
		return;
	}
#endif

	handle = lookup_rest_handle(&param);
	if (handle != NULL) {
		result = handle_rest_req(&param, handle);
	} else {
		result = NULL;
		param.status = HTTP_RESOURCE_NOT_FOUND;
	}

	{
		int32 sz;
		int8 body[100*1024];
		int resp = -1;

		for (i = 0; i < sizeof(http_resp)/sizeof(struct http_response_status); i++) {
			if (param.status == http_resp[i].status) {
				resp = i;
				break;
			}
		}
		/* if wrong response was returned, it should be bad_request then! */
		if (resp < 0) {
			// This will secure resp variable from -1 value. If somehow HTTP_BAD_REQUEST will not be found in table,
			// we return Internal Server Error. 18 is inde in htt_resp table.
			resp = 18;
			for (i = 0; i < sizeof(http_resp)/sizeof(struct http_response_status); i++) {
				if (HTTP_BAD_REQUEST == http_resp[i].status) {
					resp = i;
					break;
				}
			}
		}

		if (http_resp[resp].report_error) {
			/* unconditionally send redfish compilant json error message */
			sz = sprintf(body,
				"{"
				"\"error\": {"
				"\"code\": \"Base.1.0.0.GeneralError\","
				"\"message\": \"%s\","
				"\"@Message.ExtendedInfo\": []"
				"}"
				"}", http_resp[resp].title);
		} else if (result != NULL) {
			sz = json_format(result, body, sizeof(body));
		} else {
			sz = 0;
		}
		if (0 != sz) {
			send_json_reply(req->fd, http_resp[resp].status,
							http_resp[resp].title, body, sz);
		} else {
			send_http_reply_no_body(req->fd, http_resp[resp].status,
									http_resp[resp].title, &param);
		}
	}

	if (NULL != result)
		json_free(result);
}


static int32 split_path(int8 *path, int8 *ids[])
{
	int32 i = 0;
	int8 *cp = path;

	if (*cp != '/')
		return 0;

	for (cp++; i < MAX_URI_NODES;) {
		ids[i] = cp;
		while (*cp != '\0' && *cp != '/')
			cp++;

		i++;

		if (*cp == '\0')	/* end of path */
			break;
		else {		/* *cp is '/' */
			*cp = '\0';	/* replace '/' */
			cp++;

			if (*(cp) == '\0')	/* end of path */
				break;
		}
	}

	return i;
}

static int8 *decode_node(int8 *node, int32 *isvar)
{
	*isvar = 0;

	if (*node == '{') {
		int8 *tmp;

		*isvar = 1;

		tmp = strchr(++node, '}');
		if (tmp != NULL)
			*tmp = '\0';
	}

	return node;
}

static struct rest_uri_node *alloc_rest_uri_node(const int8 *id, int32 isvar)
{
	int32 idlen;
	struct rest_uri_node *node;

	idlen = strnlen_s(id, RSIZE_MAX_STR);
	node = malloc(sizeof(*node) + idlen + 1);

	if (node == NULL)
		FATAL("No memory for restd REST, exiting ...\n");

	INIT_LIST_HEAD(&node->list);
	INIT_LIST_HEAD(&node->subnode);

	node->isvar  = isvar;
	node->rest_handler = NULL;

	memcpy_s(node->name, idlen, id, idlen);
	node->name[idlen] = '\0';

	return node;
}

static struct rest_uri_node *create_rest_node(int8 *name,
											  struct list_head *parent)
{
	int32 isvar;
	int8 *id;
	struct rest_uri_node *n;

	id = decode_node(name, &isvar);
	list_for_each_entry(n, parent, list) {
		if (strcasecmp(n->name, id) == 0)
			return n;
	}

	n = alloc_rest_uri_node(id, isvar);
	list_add_tail(&n->list, parent);

	return n;
}

static void compile_rest_uri(int32 num, int8 *ids[], const struct rest_handler *handler)
{
	int32 i;
	struct rest_uri_node *leaf, *parent;

	leaf   = NULL;
	parent = &root_uri_node;
	for (i = 0; i < num; i++) {
		leaf = create_rest_node(ids[i], &parent->subnode);
		parent = leaf;
	}

	if (leaf == NULL || leaf->rest_handler != NULL)
		FATAL("No leaf or Duplicate REST URI path!\n");

	leaf->rest_handler = (struct rest_handler *)handler;
}


static int32 parse_query_params(int8 *query, struct rest_param_key *params)
{
	int32 i;
	int8 *to, *cp = query;

	if (cp == NULL)
		return 0;

	for (i = 0; i < MAX_KEYS_NUM; i++, cp++) {
		params[i].name = cp;
		to = cp;
		while (*cp != '\0' && *cp != '=') {
			if (cp[0] == '%' && isxdigit(cp[1]) && isxdigit(cp[2])) {
				*to++ = (hexit(cp[1]) << 4) | hexit(cp[2]);
				cp += 3;
			} else {
				*to++ = *cp++;
			}
		}
		if (*cp == '\0')
			break;
		*to = '\0';

		params[i].value = ++cp;
		to = cp;
		while (*cp != '\0' && *cp != '&') {
			if (cp[0] == '%' && isxdigit(cp[1]) && isxdigit(cp[2])) {
				*to++ = (hexit(cp[1]) << 4) | hexit(cp[2]);
				cp += 3;
			} else {
				*to++ = *cp++;
			}
		}
		if (*cp == '\0') {
			*to = '\0';
			i++;
			break;
		}
		*to = '\0';

	}

	return i;
}

static void send_json_reply(int32 fd, int32 status, const int8 *title, const int8 *json, int32 jsonlen)
{
	int32 len;
	int8 header[1024];

	len = snprintf_s_si(header, sizeof(header), "%s %d", HTTPD_PROTOCOL, status);
	if (len < 0)
		return;

	len += snprintf_s_ss(header+len, sizeof(header)-len,
			" %s\r\n"
			"Server: %s\r\n"
			"Access-Control-Allow-Origin: *\r\n"
			"Content-Type: application/json\r\n"
			"Access-Control-Allow-Methods: GET,PUT,POST,DELETE\r\n"
			"Access-Control-Allow-Headers: Content-Type\r\n"
			"Proxy-Connection: Keep-Alive\r\n"
			"Connection: close\r\n"
			"\r\n",
			(char *)title, HTTPD_SERVER_NAME);
	if (len < 0)
		return;

	http_write(fd, header, len);

	if ((json != NULL) && (jsonlen > 0))
		http_write(fd, json, jsonlen);
	else
		http_write(fd, "{}", 2);
}

void send_json_reply_redirect(int32 fd, const char *redirected_url)
{
	int32 len;
	int8 header[1024];

	len = snprintf_s_si(header, sizeof(header),
			"%s %d",
			HTTPD_PROTOCOL, HTTP_FOUND);
	if (len < 0)
		return;

	len += snprintf_s_sss(header+len, sizeof(header)-len,
			" %s\r\n"
			"Server: %s\r\n"
			"Location: %s\r\n"
			"\r\n",
			"Redirect", HTTPD_SERVER_NAME, (char *)redirected_url);
	if (len < 0)
		return;

	http_write(fd, header, len);
}

static void send_http_reply_no_body(int32 fd, int32 status, const int8 *title, struct rest_uri_param *param)
{
	int32 len;
	int8 header[256];

	len = snprintf_s_si(header, sizeof(header),
			"%s %d",
			HTTPD_PROTOCOL, status);
	if (len < 0)
		return;

	len += snprintf_s_s(header+len, sizeof(header)-len,
			" %s\r\n",
			(char *)title);
	if (len < 0)
		return;

	if (status == HTTP_CREATED) {
		len += snprintf_s_s(header+len, sizeof(header)-len,
		"Location:%s\r\n", param->rsp);
	}

	len += snprintf_s_s(header+len, sizeof(header)-len, "%s", "\r\n");
	if (len < 0)
		return;

	http_write(fd, header, len);
}


static inline struct rest_uri_node *find_rest_node(const int8 *name, struct list_head *parent)
{
	struct rest_uri_node *n, *var = NULL;

	list_for_each_entry(n, parent, list) {
		if (strcasecmp(n->name, name) == 0)
			return n;
		else if (n->isvar == 1)
			var = n;
	}

	return var;
}

static struct rest_handler *lookup_rest_handle(struct rest_uri_param *param)
{
	int32 i;
	int32 num_keys = 0;
	struct rest_uri_node *leaf, *parent;

	leaf   = NULL;
	parent = &root_uri_node;
	for (i = 0; i < param->num_nodes; i++) {
		if ((leaf = find_rest_node(param->nodes[i], &parent->subnode)) == NULL)
			break;

		parent = leaf;
		if (leaf->isvar) {
			param->path_keys[num_keys].name = leaf->name;
			param->path_keys[num_keys].value = param->nodes[i];
			num_keys++;
		}
	}
	param->num_path_keys = num_keys;

	return leaf != NULL ? leaf->rest_handler : NULL;
}

static json_t *handle_rest_req(struct rest_uri_param *param, const struct rest_handler *handler)
{
	json_t *result = NULL;

	switch (param->httpmethod) {
	case M_GET:
		if (handler->get != NULL)
			result = handler->get((struct rest_uri_param *)param);
		else
			param->status = HTTP_METHOD_NOT_ALLOWED;
		break;
	case M_PUT:
		if (handler->put != NULL)
			result = handler->put((struct rest_uri_param *)param);
		else
			param->status = HTTP_METHOD_NOT_ALLOWED;
		break;
	case M_PATCH:
		if (handler->patch != NULL)
			result = handler->patch((struct rest_uri_param *)param);
		else
			param->status = HTTP_METHOD_NOT_ALLOWED;
		break;
	case M_POST:
		if (handler->post != NULL)
			result = handler->post((struct rest_uri_param *)param);
		else
			param->status = HTTP_METHOD_NOT_ALLOWED;
		break;
	case M_DELETE:
		if (handler->del != NULL)
			result = handler->del((struct rest_uri_param *)param);
		else
			param->status = HTTP_METHOD_NOT_ALLOWED;
		break;
	default:
		break;
	}
	return result;
}

