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


#ifndef _IPMI20_CRYPTO_H_
#define _IPMI20_CRYPTO_H_

#ifdef IPMI20_SUPPORT
int ipmi20_generate_auth_code(unsigned char alg,
							  void *key,
							  int key_len,
							  unsigned char *data,
							  int data_len,
							  unsigned char *buf);

int ipmi20_random(unsigned char *buf, int num);

unsigned char *ipmi20_payload_encrypt(struct ipmi_session *sess,
									  struct rmcp_plus_ipmi_info *ipmi_info,
									  unsigned char *data,
									  int data_len,
									  int *len);


unsigned char *ipmi20_payload_decrypt(struct ipmi_session *sess,
									  unsigned char *key,
									  struct rmcp_plus_ipmi_info *ipmi_info,
									  int *len);
#endif

#endif
