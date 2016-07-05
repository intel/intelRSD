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
#include "libencrypter/encrypter.hpp"
#include "libsecurec/safe_lib.h"


int main(int argc, char** argv) {
	char plaintext[CIPHER_LENGTH_MAX] = {0};
	char keyfile[CIPHER_LENGTH_MAX] = {0};

	// Get plaintext to encrypt.
	if (argc < 3) {
		printf("error: Too few arguments.\n");
		printf("usage: %s plaintext key_file_path\n", argv[0]);;
		return -1;
	}

	int plaintext_length = strnlen_s(argv[1], RSIZE_MAX_STR);
	if (plaintext_length > 256-1) {
		printf("error: plaintext too long\n");
		printf("plaintext length: %d plaintext max length: %d\n",plaintext_length, 256);
		return -1;
	}
	strncpy_s(plaintext, sizeof(plaintext), argv[1], sizeof(plaintext));

	strncpy_s(keyfile, sizeof(plaintext), argv[2], sizeof(keyfile));

	char ciphertext[CIPHER_LENGTH_MAX] = {};
	rmm_encrypt(plaintext, keyfile, ciphertext);
	printf("ciphertext is:%s\n", ciphertext);

    return 0;
}


