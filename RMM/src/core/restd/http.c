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


#define _GNU_SOURCE
#include <string.h>
#include "rest.h"
#include "http.h"
#include "websocket.h"
#include "libcfg/cfg.h"
#include <pthread.h>



static int32 add_headers(int8 *buf, int32 status, const int8 *title, const int8 *extra_header,
			const int8 *mime_type);
static int8 *file_mime_type(const int8 *name);
static void decode_url(struct http_request *req, int8 *url);
static int32 http_method(const int8 *method);
static int8 *parse_reqline(int32 fd, int8 *str);
static int8 *get_request_line(struct http_request *req);
static uint32 get_json_pointer(struct http_request *req);
static int32 re_alloc_buff(struct http_request *req);



int32 hexit(int8 c)
{
	if (c >= '0' && c <= '9')
		return c - '0';

	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;

	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;

	return 0;
}


int32 http_read(int32 fd, int8 *buff, int32 len)
{
	int32 rc;

	for (;;) {
		rc = read(fd, buff, len);
		if (rc < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		break;
	}

	return rc;
}

int32 http_write(int32 fd, const int8 *data, int32 len)
{
	int32 rc;
	int32 offset = 0;

	while (offset < len) {
		rc = write(fd, data + offset, len - offset);
		if (rc < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		offset += rc;
	}

	return offset;
}


void send_error(int32 fd, int32 status, const int8 *title, const int8 *extra_header, const int8 *text)
{
	int32 len = 0;
	int32 str_len = 0;
	int8 buf[BUFFSIZ];

	len += add_headers(buf, status, title, extra_header, "text/html");
	str_len = len;
	len += snprintf_s_isis(&buf[len], (BUFFSIZ - str_len), 
				"<HTML>"
				"<HEAD><TITLE>%d %s</TITLE></HEAD>\n"
				"<BODY BGCOLOR=\"#cc9999\">"
				"<H4>%d %s</H4>\n",
				status, (char *)title, status, (char *)title);
	str_len = len;
	len += snprintf_s_s(&buf[len], (BUFFSIZ - str_len), "%s\n", (char *)text);
	str_len = len;
	len += snprintf(&buf[len], (BUFFSIZ - str_len), "</BODY></HTML>\n");

	http_write(fd, buf, len);
}


void *http_process(void *args)
{
	int32		fd;
	struct	http_request *req;
	int8 *cp = NULL;
	int8 *line = NULL;
	int8 *method = NULL;
	int8 *url = NULL;
	int8 *protocol = NULL;
	struct timeval timo;
	int32 optval = 1;
	int32 rc, rc_r, rc_s;
	int32 r = 0;

	if (NULL == args)
		return NULL;

	req = (struct http_request *)args;
	fd = req->fd;

	req->buff = malloc(BUFFSIZ);
	req->buff_size = BUFFSIZ;
	if (NULL == req->buff) {
		HTTPD_ERR("get_request_line malloc %d fail\n", BUFFSIZ);
		free(args);
		return NULL;
	}
	memset(req->buff, 0, BUFFSIZ);

	rc = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
	if (rc < 0)
		HTTPD_ERR("setsockopt error rc=%d errno=%d %s\n", rc, errno, strerror(errno));

	timo.tv_sec  = HTTPD_TIMO;
	timo.tv_usec = 0;

	rc_r = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timo, sizeof(timo));
	rc_s = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timo, sizeof(timo));
	if (rc_r < 0 || rc_s < 0)
		HTTPD_ERR("setsockopt error fd=%d rc=%d rc_r=%d rc_s=%d errno=%d %s\n", fd, rc, rc_r, rc_s, errno, strerror(errno));

	/* GET / HTTP/1.1 */
	method = get_request_line(req);
	if (method == NULL)
		goto close_task;

	url = parse_reqline(fd, method);
	if (url == NULL)
		goto close_task;

	protocol = parse_reqline(fd, url);
	if (protocol == NULL)
		goto close_task;

	req->method = http_method(method);

	if (req->method == M_OPTIONS) {
		send_error(fd, 204, "No Content", NULL, "Method OPTIONS.");
		goto close_task;
	}

	if (req->method == M_UNKNOWN) {
		send_error(fd, 400, "Bad Request", NULL, "Method Not Support.");
		goto close_task;
	}

	decode_url(req, url);

	HTTPD_INFO("method[%s] protocol[%s] url[%s] path[%s] query[%s]!\n", method, protocol, url, req->path, req->query ? : "");

	while ((line = get_request_line(req)) != NULL) {
		if (line[0] == '\0')
			break;

		if (strncasecmp(line, "Upgrade:", 8) == 0) {
			cp = &line[8];
			cp += strspn(cp, " \t");

			if (strncasecmp(cp, "websocket", 9) == 0)
				req->is_websocket =  1;
		} else if (strncasecmp(line, "Connection:", 11) == 0) {
			cp = &line[11];
			cp += strspn(cp, " \t");

			/* Firefox: [Connection: keep-alive, Upgrade] */
			if (strcasestr(cp, "Upgrade") != NULL)
				req->is_connection_upgrade = 1;
		} else if (strncasecmp(line, "Sec-WebSocket-Key:", 18) == 0) {
			cp = &line[18];
			cp += strspn(cp, " \t");
			req->sec_websocket_key = cp;
		} else if (strncasecmp(line, "Origin:", 7) == 0) {
			cp = &line[7];
			cp += strspn(cp, " \t");
			req->origin = cp;
		} else if (strncasecmp(line, "Sec-WebSocket-Version:", 22) == 0) {
			cp = &line[22];
			cp += strspn(cp, " \t");
			req->sec_websocket_version = atoi(cp);
		} else if (strncasecmp(line, "Content-Length:", 15) == 0) {
			cp = &line[15];
			cp += strspn(cp, " \t");
			req->content_length = atol(cp);

			// If 0 the conversion failed.
			if (0 == req->content_length) {
				goto close_task;
			}

			if ((BUFFSIZ < req->content_length + MAX_HEADER_LEN) &&
				(req->buff_size != req->content_length + MAX_HEADER_LEN) + 1) {
				re_alloc_buff(req);
				memset(req->buff + BUFFSIZ, 0, req->buff_size - BUFFSIZ);
			}
		} else if (strncasecmp(line, "Content-Type:", 13) == 0) {
			cp = &line[13];
			cp += strspn(cp, " \t");
			req->content_type = cp;
		} else if (strncasecmp(line, "Host:", 5) == 0) {
			cp = &line[5];
			cp += strspn(cp, " \t");
			req->host = cp;
		}
	}

	HTTPD_INFO("Left %d bytes HTTP data with Content-Length: %ld req->sz[%d] req->pos[%d]\n",
			req->sz - req->pos, req->content_length, req->sz, req->pos);

	if (line == NULL || req->path[0] != '/')
		send_error(fd, 400, "Bad Request", NULL, "Bad Request!");

	if ((0 != req->content_length) && (req->sz != req->hd_sz + req->content_length)) {
		while ((r = http_read(req->fd, req->buff + req->sz, req->buff_size - req->sz)) > 0) {
			req->sz += r;
			if (req->sz == req->hd_sz + req->content_length)
				break;
		}
	}

	if (req->is_websocket)
		ws_process(req);
	else {
		uint32 i = 0;
		int8 *file = NULL;
		int8 *path = req->path;
		int8 *pTmp = path;
		int8 prefix[PREFIX_LEN] = {0};
		int8 new_link[MAX_URL + 8] = {0};

		rmm_cfg_get_rest_prefix(prefix, PREFIX_LEN);
		/*snprintf(new_link, (MAX_URL + 8), "%s%s", prefix, "/rack");*/
		snprintf_s_s(new_link, (MAX_URL + 8), "%s", prefix);

		/*TODO: move to correct place */
		get_json_pointer(req);

		/* remove redundant '/' */
		for (; *(path+i) != '\0'; i++) {
			if ((*(path+i) == '/') && (*(path+i+1) == '/'))
				continue;
			*pTmp++ = *(path+i);
		}
		*pTmp = '\0';
		if ((path[1] == '\0')
			|| (strncasecmp(path, new_link, strnlen_s(new_link, sizeof(new_link)-1)) == 0))
			file = NULL;
		else
			file = path;

		if (file != NULL)
			send_file(req->fd, file);
		else
			rest_process(req);
	}

close_task:
	close(fd);
	if (NULL != req->buff) {
		free(req->buff);
		req->buff = NULL;
	}
	HTTPD_INFO("\nrest_req_thread[%lld] exit, fd[%d], errno:%d %s\n", (uint64)(req->tid_rest_req), fd, errno, strerror(errno));
	free(args);
	pthread_detach(pthread_self());
	return NULL;
}

/* @name starts from "/", such as "/index.html" */
void send_file(int32 sockfd, const int8 *name)
{
	int32 fd;
	int32 r, len;
	int64 filesize;
	struct stat st;
	int8 path[1024], contentlen[128], buf[BUFFSIZ];

	snprintf_s_ss(path, sizeof(path), "%s%s", HTTPD_WORK_DIR, (char *)name);

	if (stat(path, &st) < 0 || (fd = open(path, O_RDONLY)) < 0) {
		send_error(sockfd, 404, "Not Found", NULL, "File not Found.");
		close(sockfd);
		HTTPD_ERR("Not found file %s\n", path);
		return;
	}

	filesize = st.st_size;
	snprintf_s_ll(contentlen, sizeof(contentlen), "Content-Length: %lld", filesize);

	len = add_headers(buf, 200, "OK", contentlen, file_mime_type(name));
	http_write(sockfd, buf, len);

	while (filesize > 0) {
		r = read(fd, buf, sizeof(buf));
		if (r < 0 && (errno == EINTR || errno == EAGAIN))
			continue;

		if (r <= 0 || http_write(sockfd, buf, r) != r)
			break;

		filesize -= r;
	}

	close(fd);
}


struct mime_map {
	int8 *pattern;
	int8 *mime_type;
};

static const struct mime_map mime_tables[] = {
	{ "htm",  "text/html" },
	{ "html", "text/html" },
	{ "css",  "text/css" },
	{ "js",   "application/javascript" },
	{ "gif",  "image/gif" },
	{ "jpg",  "image/jpeg" },
	{ "jpeg", "image/jpeg" },
	{ "png",  "image/png" },

	{ NULL, NULL }
};

static int8 *file_mime_type(const int8 *name)
{
	int8 *suffix;
	const struct mime_map *map = &mime_tables[0];

	suffix = strrchr(name, '.');
	if (suffix == NULL)
		return "text/plain";

	for (suffix++; map->pattern != NULL; map++) {
		if (strcmp(map->pattern, suffix) == 0)
			return map->mime_type;
	}

	return "text/plain";
}


static int32 add_headers(int8 *buf, int32 status, const int8 *title, const int8 *extra_header,
			const int8 *mime_type)
{
	int32 len = 0;
	time_t now;
	int8 timebuf[128];
	struct tm *gmt = NULL;
	len += snprintf_s_sis(&buf[len], BUFFSIZ, "%s %d %s\r\n", HTTPD_PROTOCOL, status, (char *)title);
	len += snprintf_s_s(&buf[len], (BUFFSIZ - len), "Server: %s\r\n", HTTPD_SERVER_NAME);

	now = time(NULL);
	gmt = gmtime(&now);
	if (gmt == NULL)
		return 0;
	strftime(timebuf, sizeof(timebuf), HTTPD_RFC1123FMT, gmt);
	len += snprintf_s_s(&buf[len], (BUFFSIZ - len), "Date: %s\r\n", timebuf);

	if (extra_header != NULL)
		len += snprintf_s_s(&buf[len], (BUFFSIZ - len), "%s\r\n", (char *)extra_header);
	if (mime_type != NULL)
		len += snprintf_s_s(&buf[len], (BUFFSIZ - len), "Content-Type: %s\r\n", (char *)mime_type);

	len += snprintf(&buf[len], (BUFFSIZ - len), "Access-Control-Allow-Origin: *\r\n");        /* Allow swagger json access */
	len += snprintf(&buf[len], (BUFFSIZ - len), "Access-Control-Allow-Headers:Content-Type, api_key, Authorization\r\n");    /* Allow swagger to allow method POSe */
	len += snprintf(&buf[len], (BUFFSIZ - len), "Access-Control-Allow-Methods: GET,PUT,POST,DELETE\r\n");
	len += snprintf(&buf[len], (BUFFSIZ - len), "Allow:OPTIONS,POST,PUT\r\n");
	len += snprintf(&buf[len], (BUFFSIZ - len), "Proxy-Connection: Keep-Alive\r\n");
	len += snprintf(&buf[len], (BUFFSIZ - len), "Connection: close\r\n");
	len += snprintf(&buf[len], (BUFFSIZ - len), "\r\n");

	return len;
}


static void decode_url(struct http_request *req, int8 *url)
{
	int8 *cp = url;
	int8 *to = url;

	req->path = url;

	for (; *cp != '\0'; ++to, ++cp) {
		if (cp[0] == '%' && isxdigit(cp[1]) && isxdigit(cp[2])) {
			*to = (hexit(cp[1]) << 4) | hexit(cp[2]);
			cp += 2;
		} else if (cp[0] == '?') {
			*to = '\0';
			req->query = cp + 1;
			break;
		} else {
			*to = *cp;
		}
	}

	*to = '\0';
}

static int32 http_method(const int8 *method)
{
	if (strcasecmp(method, "GET") == 0)
		return M_GET;

	if (strcasecmp(method, "PUT") == 0)
		return M_PUT;

	if (strcasecmp(method, "PATCH") == 0)
		return M_PATCH;

	if (strcasecmp(method, "POST") == 0)
		return M_POST;

	if (strcasecmp(method, "DELETE") == 0)
		return M_DELETE;

	if (strcasecmp(method, "OPTIONS") == 0)
		return M_OPTIONS;

	if (strcasecmp(method, "EVENT") == 0)
		return M_EVENT;

	return M_UNKNOWN;
}

static int8 *parse_reqline(int32 fd, int8 *str)
{
	int8 *cp;

	cp = strpbrk(str, " \t");
	if (cp == NULL) {
		send_error(fd, 400, "Bad Request", NULL, "Can't parse HTTP Request.");
	} else {
		*cp++ = '\0';
		cp += strspn(cp, " \t");
	}
	return cp;
}

static int32 get_http_head_sz(struct http_request *req)
{
	int32 i = 0;
	int8 *buf = NULL;

	if (NULL == req)
		return 0;

	buf = req->buff;

	for (i = 0; i < req->sz; i++) {
		if ((i+3 < req->sz) &&
			(0 == memcmp(&buf[i], "\r\n\r\n", 4))) {
			req->hd_sz = i+3+1;
			break;
		}
	}
	return req->hd_sz;
}

static int8 *get_request_line(struct http_request *req)
{
	if (NULL == req)
		return NULL;

	int32 r = 0;
	int32 i = req->pos;
	int8 *buf = NULL;
	int8 ch, *line = NULL;

	buf = req->buff;


again:
	for (; req->pos < req->sz; req->pos++) {
		ch = buf[req->pos];

		if (ch == '\r') {
			buf[req->pos] = '\0';
		} else if (ch == '\n') {
			buf[req->pos++] = '\0';
			line = &buf[i];
			break;
		}
	}

	if ((line == NULL)
		&& (r = http_read(req->fd, buf + req->sz, req->buff_size - req->sz)) > 0) {
		req->sz += r;
		get_http_head_sz(req);
		goto again;
	}

	return line;
}


static uint32 get_json_pointer(struct http_request *req)
{
	uint32 i = 0;
	uint32 first_matched = 0;
	uint8 starter = 0;
	int32 checker_brace = 0;   /* checker of '{' '}'*/
	int32 checker_bracket = 0; /* checker of '['  ']' */
	int32 checker_quote = 0;   /* checked of '"' */

	for (i = 0; i < req->sz; i++) {
		switch (req->buff[i]) {
		case '{':
			checker_brace++;
			if ((0 == first_matched)) {
				req->json_start = i;
				starter = '{';
				first_matched = 1;
			}
			break;
		case '}':
			checker_brace--;
			if (starter == '{')
				req->json_end = i;
			break;
		case '[':
			checker_bracket++;
			if ((0 == first_matched)) {
				req->json_start = i;
				starter = '[';
				first_matched = 1;
			}
			break;
		case ']':
			checker_bracket--;
			if (starter == '[')
				req->json_end = i;
			break;
		case '"':
			checker_quote++;
			break;

		default:
			break;
		}
	}

	if (checker_brace || checker_bracket || (checker_quote%2 != 0) ||
		(0 == req->json_start) || (0 == req->json_end) || (req->json_start > req->json_end)) {
		//printf("brace[%d] bracket[%d] quote[%d] content_length[%lld] hd_sz[%d] sz[%d] json_start[%d] json_end[%d]\n",
			//checker_brace, checker_bracket, checker_quote, req->content_length, req->hd_sz, req->sz, req->json_start, req->json_end);
		return 0;
	}
	HTTPD_DEBUG("content_length[%ld] hd_sz[%d] sz[%d] json_start[%d] json_end[%d]\n",
		req->content_length, req->hd_sz, req->sz, req->json_start, req->json_end);
	return (req->json_end-req->json_start + 1);

}

static int32 re_alloc_buff(struct http_request *req)
{
	int8 *orig = req->buff;

	if (NULL != req->sec_websocket_key)
		req->sec_websocket_key_offset = (int32)(req->sec_websocket_key - req->buff);
	if (NULL != req->origin)
		req->origin_offset = (int32)(req->origin - req->buff);
	if (NULL != req->content_type)
		req->content_type_offset = (int32)(req->content_type - req->buff);
	if (NULL != req->host)
		req->host_offset = (int32)(req->host - req->buff);
	if (NULL != req->path)
		req->path_offset = (int32)(req->path - req->buff);
	if (NULL != req->query)
		req->query_offset = (int32)(req->query - req->buff);

	req->buff_size = req->content_length + MAX_HEADER_LEN + 1;
	req->buff = (int8 *)realloc(req->buff, req->content_length + MAX_HEADER_LEN + 1);
	if (NULL == req->buff)
		return -1;

	if (orig != req->buff)
		HTTPD_DEBUG("\nrealloc buffer size from %d to %ld\n", BUFFSIZ, req->content_length + MAX_HEADER_LEN);

	if (NULL != req->sec_websocket_key)
		req->sec_websocket_key = req->buff + req->sec_websocket_key_offset;
	if (NULL != req->origin)
		req->origin = req->buff + req->origin_offset;
	if (NULL != req->content_type)
		req->content_type = req->buff + req->content_type_offset;
	if (NULL != req->host)
		req->host = req->buff + req->host_offset;
	if (NULL != req->path)
		req->path = req->buff + req->path_offset;
	if (NULL != req->query)
		req->query = req->buff + req->query_offset;
	return 0;
}

