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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "libutils/rmm.h"
#include "libsecurec/safe_lib.h"

#if 0
int fill_string_from_str(char *output, char *input)
{
	if (output == NULL) {
		return -1;
	}

	sprintf(output, "%s", input);
	return 0;
}

int fill_string_from_int(char *output, int input)
{
	if (output == NULL) {
		return -1;
	}

	sprintf(output, "%d", input);
	return 0;
}
#endif

void reg_sigterm_handler(void (*handler)(int s))
{
	struct sigaction action, old_action;

	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	sigaction(SIGTERM, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN) {
		sigaction(SIGTERM, &action, NULL);
	}
}

char *insert_str(char *dest, const char *source, unsigned int first, const char *ins_str)
{
	int len_ins_str = 0;

	if ((dest == NULL) || (source == NULL) || (ins_str == NULL) ||
	(first < 0) || (first > strnlen_s(source, RSIZE_MAX_STR))) {
		return NULL;
	}

	len_ins_str = strnlen_s(ins_str, RSIZE_MAX_STR);
	memset(dest, 0, (strnlen_s(source, RSIZE_MAX_STR) + len_ins_str));

	if (first > 0)
		memcpy_s(dest, RSIZE_MAX_STR, source, first);

	memcpy_s(dest + first, len_ins_str, ins_str, len_ins_str);
	memcpy_s(dest + first + len_ins_str, (strnlen_s(source, RSIZE_MAX_STR) - first), source + first, (strnlen_s(source, RSIZE_MAX_STR) - first));

	return dest;
}


/**
  *  @brief 2 hex char convert to 1 byte
  *         e.g., string "3f" to 1 byte 0x3F
  *
  *  @param[in] p_char hex char
  *  @return 1 byte value
  */
static char hexchar2byte(const char *p_char)
{
	char h, l;

	if (p_char == NULL)
		return 0;

	h = p_char[0];
	l = p_char[1];

	if (l >= '0' && l <= '9')
		l = l - '0';
	else if (l >= 'a' && l <= 'f')
		l = l - 'a' + 0xa;
	else if (l >= 'A' && l <= 'F')
		l = l - 'A' + 0xa;
	else
		return -1;

	if (h >= '0' && h <= '9')
		h = h - '0';
	else if (h >= 'a' && h <= 'f')
		h = h - 'a' + 0xa;
	else if (h >= 'A' && h <= 'F')
		h =  h - 'A' + 0xa;
	else
		return -1;

	return (h*16 + l);
}

/**
  *  @brief 1 byte value convert to 2 hex char
  *         e.g., 1 byte 0x3F to string "3f"
  *
  *  @param[in] i 1 byte char value
  *  @param[out] hex_char output hex string
  *  @return
  */
static void byte2hexchar(char i, char *hex_char)
{
	char h, l;

	if (hex_char == NULL)
		return;

	h = (0xf0 & i) >> 4;
	l = 0x0f & i;

	if (l >= 0 && l <= 9)
		l = l + '0';
	else if (l >= 0x0a && l <= 0x0f)
		l = l + 'a' - 0xa;

	if (h >= 0 && h <= 9)
		h = h + '0';
	else if (h >= 0x0a && h <= 0x0f)
		h =  h + 'a' - 0xa;

	hex_char[0] = h;
	hex_char[1] = l;
}

/**
  *  @brief hex string to buffer
  *         e.g., "001122334455af" to 0x001122334455af
  *
  *  @param[in] hex_str output hex string
  *  @param[in] size buffer size
  *  @param[out] buf
  *  @return
  *  @retval <0 failure
  *  @retval >0 buffer size
  */
int hexstr2buf(const char *hex_str, int size, char *buf)
{
	if ((hex_str == NULL) || (buf == NULL))
		return -1;
	else if ((size <= 0) || (strnlen_s(hex_str, RSIZE_MAX_STR) != size))
		return -2;
	else if ((strnlen_s(hex_str, RSIZE_MAX_STR))%2 != 0)
		return -3;
	else {
		int i = 0;

		for (i = 0; i < size/2; i++)
			buf[i] = hexchar2byte(hex_str + 2 * i);
	}

	return size/2;
}

/**
  *  @brief buffer to hex string
  *         e.g., 0x001122334455af to "001122334455af"
  *
  *  @param[in] buf
  *  @param[in] size buffer size
  *  @param[out] hex_str output hex string
  *  @return
  *  @retval -1 failure
  *  @retval >0 hex_str size
  */
int buf2hexstr(const char *buf, int size, char *hex_str)
{
	int i;

	if ((buf == NULL) || (hex_str == NULL))
		return -1;
	else if (size <= 0)
		return -1;

	for (i = 0; i < size; i++)
		byte2hexchar(buf[i], hex_str + 2 * i);

	return 2*size;
}


