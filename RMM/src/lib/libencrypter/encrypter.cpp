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

#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <iomanip>
#include <fstream>
#include <crypt.h>
#include <stdexcept>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "libencrypter/encrypter.hpp"




typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

static const size_t PASS_LENGHT_MAX = CIPHER_LENGTH_MAX;
static const size_t BITS_BUFFER_SIZE = 64;
static const size_t OCTET = 8;
static const size_t KEY_LENGTH = OCTET + 1;

using namespace std;

static bool file_exists(const string&path);
static void save_key(const string&path, const char* key);
static void load_key(const string&path, char* key);
static void generate_key(char* key, size_t len);
static string format_cipher_string(const vector<uint8_t>& encrypted_plaintext);

static int do_decrypt(const string&key, const string&message, char *plaintext);
static void do_encrypt(const char* plaintext, const char* key, vector<uint8_t>& encrypted);
static int _rmm_encrypt(const char *plaintext, const char *keyfile, char *ciphertext);
static int _rmm_decrypt(const char *ciphertext, const char *keyfile, char *plaintext);

#ifdef __cplusplus
extern "C" {


#endif
#include "libsecurec/safe_lib.h"

int rmm_encrypt(const char *plaintext, const char *keyfile, char *ciphertext)
{
    return _rmm_encrypt(plaintext, keyfile, ciphertext);
}

int rmm_decrypt(const char *ciphertext, const char *keyfile, char *plaintext)
{
    return _rmm_decrypt(ciphertext, keyfile, plaintext);
}

#ifdef __cplusplus
}
#endif

static int _rmm_encrypt(const char *plaintext, const char *keyfile, char *ciphertext) {
    vector<uint8_t> encrypted_plaintext;

    // Buffers for unistd crypt API.
    char key     [KEY_LENGTH] = {0};

    // Get plaintext to encrypt.
    if (!plaintext || !keyfile) {
        cerr << "error: plaintext or keyfile is null."  << endl;
        return -1;
    }

    unsigned int plaintext_length = strnlen_s(plaintext, RSIZE_MAX_STR);
    if (plaintext_length > PASS_LENGHT_MAX-1) {
        cerr << "error: plaintext too long"  << endl;
        cout << "plaintext length: " << plaintext_length
             << "\nplaintext max length: " << PASS_LENGHT_MAX << endl;
        return -1;
    }

    try {
        if (file_exists(keyfile)) {
            load_key(keyfile, key);
        } else {
            cout << "Key file not found: " << keyfile << endl;
            cout << "Creating key file: "  << keyfile << endl;
            generate_key(key, OCTET);
            save_key(keyfile, key);
        }
    }
    catch(const runtime_error& ex) {
        cerr << "Error: " << ex.what() << endl;
        return -1;
    }

    do_encrypt(plaintext, key, encrypted_plaintext);

	string str = format_cipher_string(encrypted_plaintext);

	str.copy(ciphertext, str.length()+1, 0);

    return 0;
}

static int _rmm_decrypt(const char *ciphertext, const char *keyfile, char *plaintext) {
    // Buffers for unistd crypt API.
    char key     [KEY_LENGTH] = {0};

    // Get ciphertext to encrypt.
    if (ciphertext == NULL || keyfile == NULL) {
        cerr << "error: ciphertext or keyfile is null."  << endl;
        return -1;
    }

    try {
        if (file_exists(keyfile)) {
            load_key(keyfile, key);
        } else {
            cout << "Key file not found: " << keyfile << endl;
            return -1;
        }
    }
    catch(const runtime_error& ex) {
        cerr << "Error: " << ex.what() << endl;
        return -1;
    }

	do_decrypt(key, ciphertext, plaintext);

    return 0;
}

static bool file_exists(const string&path) {
    ifstream ifile(path.c_str());
    return ifile;
}

static void save_key(const string&path, const char* key) {
    ofstream ofile(path.c_str());

    if (ofile.good()) {
        ofile.write(key, OCTET);
    } else {
        throw runtime_error("Error with opening keyfile for write.");
    }
}

static void load_key(const string&path, char* key) {
    ifstream ifile(path.c_str());
    if (ifile.good()) {
        ifile.getline(key, OCTET+1);
        ifile.close();
    } else {
        throw runtime_error("Error with opening keyfile for read.");
    }
}

static void generate_key(char* key, size_t len) {
    const string chars =
        "1234567890"
        "!@#$%^&*()-=_+,./?<>;'\\][{}|:"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    srand((unsigned)time(NULL));

    for (uint32_t index = 0 ; index < len; index++) {
        key[index] = chars[rand()%sizeof(chars)];
    }
}

static string format_cipher_string(const vector<uint8_t>& encrypted_plaintext) {
	ostringstream ostr;

	for(unsigned int i = 0; i < encrypted_plaintext.size(); i++) {
        ostr << setfill('0')
             << setw(2)
             << uppercase
             << hex
             << unsigned(encrypted_plaintext[i]);
    }

	return ostr.str();
}

static int do_decrypt(const std::string&key, const std::string&message, char *plaintext)
{

    if (key.empty() || message.empty()) {
        return -1;
    }

    std::string decrypted_message = "";

    char key_bits[64];
    char msg_bits[64];
    char byte_string[3];

    if (0 != (message.size() % 16)) {
        return -1;
    }

    struct crypt_data crypt_data = {};
    crypt_data.initialized = 0;

    size_t key_len = key.size();
    if (key_len > 8) {
        key_len = 8;
    }

    memset(key_bits, 0, sizeof(key_bits));
    memset(msg_bits, 0, sizeof(msg_bits));

    for (size_t i = 0; i < key_len; i++) {
        for (size_t j = 0; j < 8; j++) {
            key_bits[i * 8 + j] = ((key[i] >> j) & 1);
        }
    }
    setkey_r(key_bits, &crypt_data);

    size_t blocks = message.size() / 2 / 8;

    for (size_t i = 0; i < blocks; i++) {
        memset(msg_bits, 0, sizeof(msg_bits));
        for (size_t j = 0; j < 8; j++) {
            byte_string[0] = message[(i * 8 + j) * 2];
            byte_string[1] = message[(i * 8 + j) * 2 + 1];
            byte_string[2] = 0;
            unsigned char byte = strtoul(byte_string, NULL, 16) & 0xff;
            for (size_t k = 0; k < 8; k++) {
                msg_bits[j * 8 + k] = ((byte >> k) & 1);
            }
        }
        encrypt_r(msg_bits, 1, &crypt_data);

        for (size_t j = 0; j < 8; j++) {
            uint8_t c = 0;
            for (size_t k = 0; k < 8; k++) {
                uint8_t bit =
                    static_cast<uint8_t>(msg_bits[j * 8 + k] << k);
                c |= bit;
            }
            if (0 != c) {
                decrypted_message += static_cast<char>(c);
            }
        }
    }

	decrypted_message.copy(plaintext, decrypted_message.length()+1, 0);

    return 0;
}

static void do_encrypt(const char* plaintext,
                      const char* key,
                      vector<uint8_t>& encrypted) {
    char key_bits[BITS_BUFFER_SIZE] = {0};

    for (uint32_t i = 0; i < OCTET; i++) {
		for (uint32_t j = 0; j < OCTET; j++) {
			key_bits[i * OCTET + j] = key[i] >> j & 1;
		}
	}

    setkey(key_bits);
    unsigned int plaintext_length = strnlen_s(plaintext, RSIZE_MAX_STR);
    int blocks_number = plaintext_length / OCTET;

    if (0 != (plaintext_length % OCTET)) {
		blocks_number++;
	}

    char plaintext_bits[BITS_BUFFER_SIZE] = {0};
    for (int i = 0; i < blocks_number; i++) {
        memset(plaintext_bits, 0, sizeof(plaintext_bits));
        size_t cond = i * OCTET;
		for (uint32_t j = 0; j < OCTET && cond + j < plaintext_length; j++) {
			for (uint32_t k = 0; k < OCTET; k++) {
				if ((j * OCTET + k) < BITS_BUFFER_SIZE && (i * OCTET + j) < BITS_BUFFER_SIZE)
					plaintext_bits[j * OCTET + k]
                        = ((plaintext[i * OCTET + j] >> k) & 1);
			}
		}

		encrypt(plaintext_bits, 0);
		for (uint32_t j = 0; j < OCTET; j++) {
			uint8_t c = 0;
			for (uint32_t k = 0; k < OCTET; k++) {
				if ((j * OCTET + k) < BITS_BUFFER_SIZE)
					c |= uint8_t(plaintext_bits[j * OCTET + k] << k);
			}
            encrypted.push_back(c);
		}
	}
}
