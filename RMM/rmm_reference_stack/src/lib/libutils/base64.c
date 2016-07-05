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


#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#include "libutils/base64.h"

static char b64_encode_table[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_encode(char *dest, void *src, int len)
{
	int i;
	char *out = dest;
	unsigned char *in = src;

	for (i = 0; i < len/3; i++, in += 3) {
		*out++ = b64_encode_table[in[0] >> 2];
		*out++ = b64_encode_table[((in[0] & 0x03) << 4) | ((in[1] & 0xF0) >> 4)];
		*out++ = b64_encode_table[((in[1] & 0x0F) << 2) | ((in[2] & 0xC0) >> 6)];
		*out++ = b64_encode_table[in[2] & 0x3F];
	}

	switch (len%3) {
	case 2:
		*out++ = b64_encode_table[in[0] >> 2];
		*out++ = b64_encode_table[((in[0] & 0x03) << 4) | ((in[1] & 0xF0) >> 4)];
		*out++ = b64_encode_table[((in[1] & 0x0F) << 2)];
		*out++ = '=';
		break;

	case 1:
		*out++ = b64_encode_table[in[0] >> 2];
		*out++ = b64_encode_table[((in[0] & 0x03) << 4)];
		*out++ = '=';
		*out++ = '=';
		break;

	default:
		break;
	}
	*out = '\0';

	return out - dest;
}

char *base64_decode(char *input, int len, int *out_len)
{
	char *output = NULL;
	BIO *base64 = NULL;
	BIO *base64_mem = NULL;
	int rc = -1;

	base64 = BIO_new(BIO_f_base64());
	if (base64 == NULL) {
		printf("base64 alloc error\n");
		return NULL;
	}

	BIO_set_flags(base64, BIO_FLAGS_BASE64_NO_NL);

	output = (char *)malloc(len);
	if (output == NULL) {
		printf("output malloc error, len is %d.\n", len);
		return NULL;
	}
	memset(output, 0, len);

	base64_mem = BIO_new_mem_buf(input, len);
	base64_mem = BIO_push(base64, base64_mem);
	rc = BIO_read(base64_mem, output, len);

	*out_len = rc;

	BIO_free_all(base64_mem);

	return output;
}
