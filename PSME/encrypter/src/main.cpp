/*!
 * @brief BMC username and password encrypt utility (uses AES256 CBC).
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file main.cpp
 */

#include <safe-string/safe_lib.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <string.h>

#include "utils/crypt_utils.hpp"
#include "utils/conversion.hpp"

static constexpr const int STATUS_OK = 0;
static constexpr const int STATUS_ERROR_IO = 3;

static constexpr const int CFG_ARG_MIN = 2;
static constexpr const size_t CFG_PASS_LENGHT_MAX = 256;

static constexpr const char CFG_KEY_FILE_PATH[] = "/etc/psme/psme-key";

namespace encrypter {

enum class EncryptOperation {
    ENCRYPT_AES256 = 0,
    DECRYPT_AES256 = 1,
    PBKDF2_SHA512  = 2
};

struct Options {
    std::string password{};
    std::string keyfile{CFG_KEY_FILE_PATH};
    EncryptOperation operation = EncryptOperation::ENCRYPT_AES256;
};

/*!
 * @brief Prints tool's usage help and examples.
 * @param program_name name equals argv[0].
 */
void print_usage(std::string program_name);

/*!
 * @brief Parses and validates arguments given in command line.
 * @param argc argument count.
 * @param argv argument vector.
 * @return options structure initialized from command line parameters.
 */
Options parse_options(int argc, char** argv);

/*!
 * @brief Checks if file exist.
 * @param path string with path to file.
 * @return true if file exists, otherwise false.
 */
bool file_exists(const std::string& path);

/*!
 * @brief Performs encryption with AES256 algorithm.
 * @param options structure containing password to cipher and optionally file name with encryption key.
 */
void encrypt_operation(encrypter::Options& options);

/*!
 * @brief Performs decryption with AES256 algorithm.
 * @param options structure containing ciphered password to decrypt and file name with encryption key.
 */
void decrypt_operation(encrypter::Options& options);

/*!
 * @brief Performs hashing with PBKDF2 algorithm (SHA512 based with salt).
 * @param options structure containing password to hash.
 */
void hash_operation(encrypter::Options& options);

void print_usage(std::string program_name) {
    std::cerr << "Encrypt - tool to encrypt/decrypt given password with AES-256 or hash with PBKDF2 (SHA-512 based with salt).\n"
              << "Usage: " << program_name << " <password | encrypted_password> [[--hash] | [--encrypt | --decrypt] <key_file_path>] | [--help]]"
              << "\nExample:\n"
              << "\t1. Encrypt: encrypt TEXT_TO_ENCRYPT (creates default key file)\n"
              << "\t2. Encrypt: encrypt TEXT_TO_ENCRYPT --encrypt KEY_FILE (creates/reads user key file)\n"
              << "\t3. Decrypt: encrypt CIPHER_TO_DECRYPT --decrypt KEY_FILE (ciphertext in hex form)\n"
              << "\t4. Hash   : encrypt TEXT_TO_HASH --hash\n";
}

Options parse_options(int argc, char** argv) {
    Options options;

    if (argc < CFG_ARG_MIN || (strcmp(argv[1], "--help") == 0)) {
        print_usage(argv[0]);
        throw std::invalid_argument("Too few arguments!");
    }

    // Get password to encrypt
    auto cmd_password_length = strlen(argv[1]);
    if (strlen(argv[1]) > CFG_PASS_LENGHT_MAX - 1) {
        std::cout << "Password length: " << cmd_password_length
                  << "\nPassword max length: " << CFG_PASS_LENGHT_MAX - 1 << std::endl;
        throw std::invalid_argument("Password too long!");
    }

    char input_password[CFG_PASS_LENGHT_MAX] = {0};
    errno_t result = strncpy_s(input_password, CFG_PASS_LENGHT_MAX, argv[1], cmd_password_length);
    if (EOK != result) {
        std::cout << "Unable to encrypt password." << std::endl;
        throw std::invalid_argument("Password copy error!");
    }
    input_password[CFG_PASS_LENGHT_MAX - 1] = 0;
    options.password = std::string(input_password);

    // Set required operation (and key) if passed as an argument
    if (argc > CFG_ARG_MIN) {
        if (strcmp(argv[2], "--encrypt") == 0) {
            options.operation = EncryptOperation::ENCRYPT_AES256;
            options.keyfile = std::string(argv[3]);
        }
        else if (strcmp(argv[2], "--decrypt") == 0) {
            options.operation = EncryptOperation::DECRYPT_AES256;
            options.keyfile = std::string(argv[3]);
        }
        else if (strcmp(argv[2], "--hash") == 0) {
            options.operation = EncryptOperation::PBKDF2_SHA512;
        }
        else {
            // For backward compatibility: encrypt <password> [key_file_path]
            options.keyfile = std::string(argv[2]);
        }
    }

    return options;
}

bool file_exists(const std::string& path) {
    std::ifstream ifile(path);
    return ifile.good();
}

void encrypt_operation(encrypter::Options& options) {
    std::string key;
    std::string init_vector;

    if (encrypter::file_exists(options.keyfile)) {
        auto initializers = utils::load_crypt_initializers(options.keyfile);
        key = initializers.first;
        init_vector = initializers.second;
    }
    else {
        std::cout << "Key file not found - creating: " << options.keyfile << std::endl;
        key = utils::generate_key();
        init_vector = utils::generate_init_vector();
        utils::save_crypt_initializers(options.keyfile, key, init_vector);
    }

    std::string encrypted_password = utils::encrypt(options.password, key, init_vector);
    std::cout << utils::string_to_hex_string(encrypted_password) << std::endl;
}

void decrypt_operation(encrypter::Options& options) {
    std::string key;
    std::string init_vector;

    std::string encrypted_value = utils::hex_string_to_string(options.password);

    if (encrypter::file_exists(options.keyfile)) {
        auto initializers = utils::load_crypt_initializers(options.keyfile);
        key = initializers.first;
        init_vector = initializers.second;
    }

    std::cout << utils::decrypt(encrypted_value, key, init_vector) << std::endl;
}

void hash_operation(encrypter::Options& options) {
    std::string salt = utils::generate_salt();
    std::string hashed_password = utils::salted_hash(options.password, salt);

    // Hashed password as a hex string to be pasted to 'authentication' property in configuration file, in the form of:
    // Concatenated salted hash string [80B] = salt [16B] + hash [64B]
    std::cout << utils::string_to_hex_string(salt) + utils::string_to_hex_string(hashed_password) << std::endl;
}

} // namespace encrypter

int main(int argc, char** argv) {
    try {
        auto options = encrypter::parse_options(argc, argv);

        switch (options.operation) {
            case encrypter::EncryptOperation::ENCRYPT_AES256:
                encrypter::encrypt_operation(options);
                break;
            case encrypter::EncryptOperation::DECRYPT_AES256:
                encrypter::decrypt_operation(options);
                break;
            case encrypter::EncryptOperation::PBKDF2_SHA512:
                encrypter::hash_operation(options);
                break;
            default:
                encrypter::encrypt_operation(options);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return STATUS_ERROR_IO;
    }

    return STATUS_OK;
}