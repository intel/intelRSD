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


#include <stdlib.h>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

#include "rmcp.h"
#include "ipmi20_crypto.h"
#include "libsecurec/safe_lib.h"
#include "ipmi_log.h"


#ifdef IPMI20_SUPPORT
/**
  *  @brief This function is used to generate random numbers for RAKP Message 1,
  *         Remote Console Random Number.
  *
  *  @param[out] buf buffer to save the random number
  *  @param[in] num the random length
  *  @return
  *  @retval 0 successful
  *  @retval !0 failure
  */
int ipmi20_random(unsigned char *buf, int num)
{
	return (!RAND_bytes(buf, num));
}

/**
  *  @brief To generate Auth Code for RAKP 3, RAKP-HMAC-SHA1, RAKP-HMAC-MD5.
  *
  *  @param[in] alg algorithm
  *  @param[in] key
  *  @param[in] key_len
  *  @param[in] data the input data before encode
  *  @param[in] data_len the input data length
  *  @param[out] buf the output buffer
  *  @return  the output data length
  */
int ipmi20_generate_auth_code(unsigned char alg,
							  void *key,
							  int key_len,
							  unsigned char *data,
							  int data_len,
							  unsigned char *buf)
{
	unsigned int len = 0;

	switch (alg) {
	case AUTH_ALG_RAKP_NONE: /* INTEGRITY_ALG_NONE has the same value */
		return 0;
	case AUTH_ALG_RAKP_HMAC_SHA1: /* INTEGRITY_ALG_HMAC_SHA1_96 has the same value */
		HMAC(EVP_sha1(), key, key_len, data, data_len, buf, &len);
		break;
	case AUTH_ALG_RAKP_HMAC_MD5: /* INTEGRITY_ALG_HMAC_MD5_128 has the same value */
	case INTEGRITY_ALG_MD5_128:
		HMAC(EVP_md5(), key, key_len, data, data_len, buf, &len);
	default:
		break;
	}

	return len;
}

#define CONFIDENTIALLITY_HEADER_MAX_LEN		(16 + 4)
#define PAYLOAD_DATA_LEN					16
#define CONFIDENTIALLITY_TRAILER_MAX_LEN	4

#define IPMI20_CRYPT_AES_CBC_128_BLOCK_SIZE	16

/**
  *  @brief
  *
  *  @param
  *  @return
  */
void ipmi20_encrypt_aes_cbc_128(unsigned char *init_vec,
								unsigned char *key,
								unsigned char *data,
								int data_len,
								unsigned char *out,
								int *out_len)
{
	EVP_CIPHER_CTX ctx;

	EVP_CIPHER_CTX_init(&ctx);
	EVP_EncryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, key, init_vec);
	EVP_CIPHER_CTX_set_padding(&ctx, 0);

	if (!EVP_EncryptUpdate(&ctx, out, out_len, data, data_len)) {
		*out_len = 0;

		return;
	} else {
		int len_tmp = 0;

		if (!EVP_EncryptFinal_ex(&ctx, out + *out_len, &len_tmp)) {
			*out_len = 0;

			return;
		} else {
			*out_len += len_tmp;
			EVP_CIPHER_CTX_cleanup(&ctx);
		}
	}
}

/**
  *  @brief
  *
  *  @param
  *  @return
  */
void ipmi20_decrypt_aes_cbc_128(unsigned char *init_vec,
								unsigned char *key,
								unsigned char *data,
								int data_len,
								unsigned char *out,
								int *out_len)
{
	EVP_CIPHER_CTX ctx;

	EVP_CIPHER_CTX_init(&ctx);
	EVP_DecryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, key, init_vec);
	EVP_CIPHER_CTX_set_padding(&ctx, 0);

	if (!EVP_DecryptUpdate(&ctx, out, out_len, data, data_len)) {
		*out_len = 0;
		return;
	} else {
		int len_tmp = 0;

		if (!EVP_DecryptFinal_ex(&ctx, out + *out_len, &len_tmp)) {
			*out_len = 0;
			return;
		} else {
			*out_len += len_tmp;
			EVP_CIPHER_CTX_cleanup(&ctx);
		}
	}
}

/**
  *  @brief
  *
  *  @param
  *  @return
  */
unsigned char *ipmi20_payload_encrypt(struct ipmi_session *sess,
									  struct rmcp_plus_ipmi_info *ipmi_info,
									  unsigned char *msg,
									  int msg_len,
									  int *len)
{
	int i = 0;
	int mod = 0;
	unsigned char *buf = NULL;
	unsigned char *pad_input = NULL;
	int pad_len = 0;
	unsigned char confidentiallity_header[CONFIDENTIALLITY_HEADER_MAX_LEN] = {0};

	buf = (unsigned char *)malloc(CONFIDENTIALLITY_HEADER_MAX_LEN
								  + PAYLOAD_DATA_LEN
								  + CONFIDENTIALLITY_TRAILER_MAX_LEN);
	if (NULL == buf) {
		IPMI_LOG_DEBUG("malloc failed\n");
		return NULL;
	};

	switch (sess->crypt_alg) {
	case CRYPT_ALG_NONE:
		buf = msg;
		*len = msg_len;
		break;
	case CRYPT_ALG_AES_CBC_128:
		/* Confidentiallity Header */
		ipmi20_random(confidentiallity_header, 16);
		memcpy_s(buf, 16, confidentiallity_header, 16);

		mod = (msg_len + 1) % IPMI20_CRYPT_AES_CBC_128_BLOCK_SIZE;
		if (mod != 0)
			pad_len = IPMI20_CRYPT_AES_CBC_128_BLOCK_SIZE - mod;

		pad_input = (unsigned char *)malloc(msg_len + pad_len + 1);
		if (NULL == pad_input) {
			free(buf);
			return NULL;
		}

		memcpy_s(pad_input, msg_len, msg, msg_len);

		/* 0x1, 0x2, 0x3 ... */
		for (i = 0; i < pad_len; i++) {
			pad_input[msg_len + i] = i + 1;
		}

		pad_input[msg_len + pad_len] = pad_len;

		ipmi20_encrypt_aes_cbc_128(buf, sess->key_k2, pad_input,
								   msg_len + pad_len + 1,
								   buf + 16, len);

		free(pad_input);

		if (*len == 0)
			return NULL;

		*len += 16;

		break;
	case CRYPT_ALG_xRC4_128:
	case CRYPT_ALG_xRC4_40:
		break;
	default:
		break;
	}

	return buf;
}

/**
  *  @brief
  *
  *  @param
  *  @return
  */
unsigned char *ipmi20_payload_decrypt(struct ipmi_session *sess,
									  unsigned char *key,
									  struct rmcp_plus_ipmi_info *ipmi_info,
									  int *len)
{
	unsigned char *out = NULL;
	int pad_len = 0;
	int payload_len = 0;

	out = (unsigned char *)malloc(CONFIDENTIALLITY_HEADER_MAX_LEN
								  + PAYLOAD_DATA_LEN
								  + CONFIDENTIALLITY_TRAILER_MAX_LEN);
	if (NULL == out) {
		IPMI_LOG_DEBUG("malloc failed\n");
		return NULL;
	};

	switch (sess->crypt_alg) {
	case CRYPT_ALG_NONE:
		*len = ipmi_info->ipmi_data_len;
		memcpy_s(out, CONFIDENTIALLITY_HEADER_MAX_LEN
						+ PAYLOAD_DATA_LEN
						+ CONFIDENTIALLITY_TRAILER_MAX_LEN,
			ipmi_info->ipmi_data, ipmi_info->ipmi_data_len);
		break;
	case CRYPT_ALG_AES_CBC_128:
		ipmi20_decrypt_aes_cbc_128(ipmi_info->ipmi_data, key,
								   ipmi_info->ipmi_data + 16,
								   ipmi_info->ipmi_data_len - 16,
								   out, len);
		if (*len > 0) {
			int i = 0;

			pad_len = out[*len - 1];

			payload_len = *len - pad_len - 1;

			for (i = 0; i < pad_len; i++) {
				if (out[payload_len + i] != (i + 1))
					return NULL;
			}

			*len -= (pad_len + 1);
		} else {
			return NULL;
		}
		break;
	case CRYPT_ALG_xRC4_128:
	case CRYPT_ALG_xRC4_40:
		break;
	default:
		break;
	}

	return out;
}

#endif
