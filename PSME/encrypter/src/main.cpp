/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
*/
#include <safe-string/safe_lib.hpp>
#include <random>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <vector>
#include <iomanip>
#include <fstream>
#include <crypt.h>
#include <stdexcept>
#include <stdlib.h>

using namespace std;

static constexpr const int STATUS_OK = 0;
static constexpr const int STATUS_ERROR_ARG = 1;
static constexpr const int STATUS_ERROR_PASS_LENGTH = 2;
static constexpr const int STATUS_ERROR_IO = 3;

static constexpr const size_t CFG_OCTET = 8;
static constexpr const size_t CFG_KEY_LENGHT_MAX = CFG_OCTET+1;
static constexpr const int    CFG_ARG_MIN = 2;
static constexpr const size_t CFG_PASS_LENGHT_MAX = 256;
static constexpr const size_t CFG_BITS_BUFFER_SIZE = 64;

static constexpr const char CFG_KEY_FILE_PATH[] = "/etc/psme/psme-key";

/*!
 * @brief Checks does file exist.
 * @param path string with path to file.
 * @return true if file exists, otherwise false.
 */
bool file_exists(const string&path);

/*!
 * @brief Loads key from key file.
 * @param path string with path to keyfile/
 * @param key buffer with key.
 */
void load_key(const string&path, char* key);

/*!
 * @brief Saves key to the file. File will be created.
 * @param path path to the file where key will be stored.
 * @param key pointer to buffer with key.
 */
void save_key(const string&path, const char* key);

/*!
 * @brief Generates random 8-byte length key with alphanumeric and special
 * characters.
 * @param key pointer to buffer where key should be stored.
 * @param len length of the key buffer.
 */
void generate_key(char* key, size_t len);

/*!
 * @brief Encrypt password with key.
 * @param password pointer to the password.
 * @param key pointer to the key.
 * @param encrypted reference to the vector where encrypted password will
 * be stored.
 */
void encrypt_password(const char* password,
                      const char* key,
                      vector<uint8_t>& encrypted);

/*!
 * @brief Prints encrypted password in hex form.
 * @param password reference to vector with encrypted password bytes.
 */
void print_password(const vector<uint8_t>& password);

int main(int argc, char** argv) {
    string keyfile = CFG_KEY_FILE_PATH;
    vector<uint8_t> encrypted_password{};
    // Buffers for unistd crypt API.
    char key     [CFG_KEY_LENGHT_MAX ] = {0};
    char password[CFG_PASS_LENGHT_MAX] = {0};

    // Get password to encrypt.
    if (argc < CFG_ARG_MIN) {
        cerr << "error: Too few arguments."  << endl;
        cout << "usage: " << argv[0] << "password [key_file_path]" << endl;
        return STATUS_ERROR_ARG;
    }

    auto cmd_password_length = strlen(argv[1]);
    if (cmd_password_length > CFG_PASS_LENGHT_MAX - 1) {
        cerr << "error: Password too long"  << endl;
        cout << "Password length: " << cmd_password_length
             << "\nPassword max length: " << CFG_PASS_LENGHT_MAX - 1 << endl;
        return STATUS_ERROR_PASS_LENGTH;
    }
    errno_t result = strncpy_s(password, CFG_PASS_LENGHT_MAX, argv[1], cmd_password_length);
    if (EOK != result) {
        cerr << "error: copy error: "<< result << endl;
        cout << "Unable to encrypt password." << endl;
        return result;
    }

    password[CFG_PASS_LENGHT_MAX - 1] = 0;
    // If path to keyfile is passed as an argument.
    if (argc > CFG_ARG_MIN) {
        keyfile = argv[2];
    }

    try {
        if (file_exists(keyfile)) {
            load_key(keyfile, key);
        } else {
            cout << "Key file not found: " << keyfile << endl;
            cout << "Creating key file: "  << keyfile << endl;
            generate_key(key, CFG_OCTET);
            save_key(keyfile, key);
        }
    }
    catch(const runtime_error& ex) {
        cerr << "Error: " << ex.what() << endl;
        return STATUS_ERROR_IO;
    }

    encrypt_password(password, key, encrypted_password);
    print_password(encrypted_password);

    return STATUS_OK;
}

bool file_exists(const string&path) {
    ifstream ifile(path);
    return ifile.good();
}

void save_key(const string&path, const char* key) {
    ofstream ofile(path);

    if (ofile.good()) {
        ofile.write(key, CFG_OCTET);
    } else {
        throw runtime_error("Error with opening keyfile for write.");
    }
}

void load_key(const string&path, char* key) {
    ifstream ifile(path);
    if (ifile.good()) {
        ifile.getline(key, CFG_OCTET+1);
        ifile.close();
    } else {
        throw runtime_error("Error with opening keyfile for read.");
    }
}

void generate_key(char* key, size_t len) {
    const string chars =
        "1234567890"
        "!@#$%^&*()-=_+,./?<>;'\\][{}|:"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(0, int(chars.size()-1));

    for (uint32_t index = 0 ; index < len; index++) {
        key[index] = chars[size_t((distribution(generator)))];
    }
}

void print_password(const vector<uint8_t>& password) {
    for (const auto cc: password) {
        cout << setfill('0')
             << setw(2)
             << uppercase
             << hex
             << unsigned(cc);

    }
    cout << endl;
}
void encrypt_password(const char* password,
                      const char* key,
                      vector<uint8_t>& encrypted) {
    char key_bits[CFG_BITS_BUFFER_SIZE] = {0};

    for (uint32_t i = 0; i < CFG_OCTET; i++) {
		for (uint32_t j = 0; j < CFG_OCTET; j++) {
			key_bits[i * CFG_OCTET + j] = key[i] >> j & 1;
		}
	}

    setkey(key_bits);
    auto password_length = strlen(password);
    auto blocks_number = password_length / CFG_OCTET;

    if (0 != (password_length % CFG_OCTET)) {
		blocks_number++;
	}

    char password_bits[CFG_BITS_BUFFER_SIZE] = {0};
    for (uint32_t i = 0; i < blocks_number; i++) {
        memset(password_bits, 0, sizeof(password_bits));
        size_t cond = i * CFG_OCTET;
		for (uint32_t j = 0; j < CFG_OCTET && cond + j < password_length; j++) {
			for (uint32_t k = 0; k < CFG_OCTET; k++) {
				password_bits[j * CFG_OCTET + k]
                        = ((password[i * CFG_OCTET + j] >> k) & 1);
			}
		}

		encrypt(password_bits, 0);
		for (uint32_t j = 0; j < CFG_OCTET; j++) {
			uint8_t c = 0;
			for (uint32_t k = 0; k < CFG_OCTET; k++) {
                            c = uint8_t(c | (password_bits[j * CFG_OCTET + k] << k));
			}
            encrypted.push_back(c);
		}
	}
}
