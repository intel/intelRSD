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


#include <curl/curl.h>
#include "libutils/curl_ref.h"

void curl_init(void)
{
	curl_global_init(CURL_GLOBAL_ALL);
}

int curl_post_msg(char *url, char *data, int b_https)
{
	CURL *easyhandle;
	struct curl_slist *headers=NULL;

	headers = curl_slist_append(headers, "Content-Type: application/json");
	easyhandle = curl_easy_init();
	if (easyhandle) {
		curl_easy_setopt(easyhandle, CURLOPT_POSTFIELDS, data);
		curl_easy_setopt(easyhandle, CURLOPT_URL, url);
		curl_easy_setopt(easyhandle, CURLOPT_HTTPHEADER, headers);
		if (b_https) {
			curl_easy_setopt(easyhandle, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(easyhandle, CURLOPT_SSL_VERIFYHOST, 0L);
		}
		curl_easy_perform(easyhandle);
		curl_easy_cleanup(easyhandle);
	} else {
		return -1;
	}
	return 0;
}

void curl_uninit(void)
{
	curl_global_cleanup();
}

