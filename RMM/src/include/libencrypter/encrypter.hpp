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

#ifndef __LIBENCRYPTER_ENCRYPT_H__
#define __LIBENCRYPTER_ENCRYPT_H__

#define CIPHER_LENGTH_MAX	(256)
#define PLAIN_LENGTH_MAX	(256)

#ifdef __cplusplus
extern "C" {
#endif

extern int rmm_encrypt(const char *plaintext, const char *keyfile, char *ciphertext);

extern int rmm_decrypt(const char *ciphertext, const char *keyfile, char *plaintext);

#ifdef __cplusplus
}
#endif


#endif
