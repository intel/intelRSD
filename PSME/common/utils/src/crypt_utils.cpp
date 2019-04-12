/*!
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file utils/crypt_utils.cpp
 */

#include "utils/crypt_utils.hpp"

#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <random>
#include <gcrypt.h>

static constexpr const uint32_t PSME_CIPHER_ALGO = GCRY_CIPHER_AES256;
static constexpr const uint32_t PSME_CIPHER_MODE = GCRY_CIPHER_MODE_CBC;
static constexpr const uint32_t PSME_CIPHER_FLAG = GCRY_CIPHER_CBC_CTS;
static constexpr const uint32_t PSME_HASH_ALGO   = GCRY_MD_SHA512;
static constexpr const uint32_t PSME_HASH_FLAG   = GCRY_MD_FLAG_SECURE;
static constexpr const uint32_t PSME_KDF_ALGO    = GCRY_KDF_PBKDF2;
static constexpr const uint32_t PSME_KDF_SUBALGO = GCRY_MD_SHA512;

static constexpr const size_t AES256_BLOCK_SIZE = 16;
static constexpr const size_t AES256_KEY_SIZE = 32;

static constexpr const size_t KDF_KEY_SIZE = 64;
static constexpr const size_t KDF_SALT_SIZE = 16;
static constexpr const size_t KDF_ITERATIONS = 10000;


namespace utils {

/*!
 * @brief Initializes cipher: selects algorithm (AES256 CBC by default), sets key and initialization vector.
 * @param handle cipher handle.
 * @param key buffer with the encryption key.
 * @param init_vector buffer with the initialization vector.
 * */
void init_cipher(gcry_cipher_hd_t* handle, const char* key, const char* init_vector);

/*!
 * @brief Initializes hash: selects algorithm (SHA512 by default).
 * @param handle hash handle.
 * */
void init_hash(gcry_md_hd_t* handle);

void init_cipher(gcry_cipher_hd_t* handle, const char* key, const char* init_vector) {
    gcry_error_t error;

    error = gcry_cipher_open(handle, PSME_CIPHER_ALGO, PSME_CIPHER_MODE, PSME_CIPHER_FLAG);
    if (error) {
        throw std::runtime_error("Error on cipher initialization: " + std::string(gcry_strerror(error)));
    }

    auto key_length = get_key_size();
    error = gcry_cipher_setkey(*handle, key, key_length);
    if (error) {
        gcry_cipher_close(*handle);
        throw std::runtime_error("Error on setting cipher key: " + std::string(gcry_strerror(error)));
    }

    auto block_length = get_block_size();
    error = gcry_cipher_setiv(*handle, init_vector, block_length);
    if (error) {
        gcry_cipher_close(*handle);
        throw std::runtime_error("Error on setting cipher init vector: " + std::string(gcry_strerror(error)));
    }
}

void init_hash(gcry_md_hd_t* handle) {
    gcry_error_t error;

    error = gcry_md_open(handle, PSME_HASH_ALGO, PSME_HASH_FLAG);
    if (error) {
        throw std::runtime_error("Error on hash initialization: " + std::string(gcry_strerror(error)));
    }
}

std::string encrypt(std::string& plain_text, const std::string& key, std::string& init_vector) {
    gcry_cipher_hd_t handle;
    gcry_error_t error;

    init_cipher(&handle, key.data(), init_vector.data());

    auto plain_text_length = plain_text.size();
    auto blocks_number = plain_text_length / AES256_BLOCK_SIZE;

    if ((plain_text_length % AES256_BLOCK_SIZE) != 0) {
        blocks_number++;
    }

    plain_text.resize(blocks_number * AES256_BLOCK_SIZE);
    std::vector<char> encrypted_text(blocks_number * AES256_BLOCK_SIZE);

    error = gcry_cipher_encrypt(handle,
                                encrypted_text.data(),
                                blocks_number * AES256_BLOCK_SIZE,
                                plain_text.data(),
                                blocks_number * AES256_BLOCK_SIZE);
    if (error) {
        gcry_cipher_close(handle);
        throw std::runtime_error("Error on encryption: " + std::string(gcry_strerror(error)));
    }

    std::string cipher_text(encrypted_text.begin(), encrypted_text.end());
    return cipher_text;
}

std::string decrypt(const std::string& cipher_text, const std::string& key, const std::string& init_vector) {
    gcry_cipher_hd_t handle;
    gcry_error_t error;

    init_cipher(&handle, key.data(), init_vector.data());

    auto cipher_text_length = cipher_text.size();
    auto blocks_number = cipher_text_length / AES256_BLOCK_SIZE;

    if ((cipher_text_length % AES256_BLOCK_SIZE) != 0) {
        blocks_number++;
    }

    std::vector<char> decrypted_text(blocks_number * AES256_BLOCK_SIZE);

    error = gcry_cipher_decrypt(handle,
                                decrypted_text.data(),
                                blocks_number * AES256_BLOCK_SIZE,
                                cipher_text.data(),
                                blocks_number * AES256_BLOCK_SIZE);
    if (error) {
        gcry_cipher_close(handle);
        throw std::runtime_error("Error on decryption " + std::string(gcry_strerror(error)));
    }

    std::string plain_text(decrypted_text.begin(), decrypted_text.end());
    return plain_text;
}

std::string hash(const std::string& plain_text) {
    gcry_md_hd_t handle;

    init_hash(&handle);

    gcry_md_write(handle, plain_text.data(), plain_text.size());
    auto digest_read = gcry_md_read(handle, PSME_HASH_ALGO);

    std::string digest((char*)digest_read, get_hash_size());

    gcry_md_close(handle);

    return digest;
}

std::string salted_hash(const std::string& password, const std::string& salt) {
    gcry_error_t error;

    std::vector<char> derived_key(KDF_KEY_SIZE);

    error = gcry_kdf_derive(password.data(),
                            password.size(),
                            PSME_KDF_ALGO,
                            PSME_KDF_SUBALGO,
                            salt.data(),
                            salt.size(),
                            KDF_ITERATIONS,
                            KDF_KEY_SIZE,
                            derived_key.data());
    if (error) {
        throw std::runtime_error("Error on hashing (KDF): " + std::string(gcry_strerror(error)));
    }

    std::string salted_hash(derived_key.begin(), derived_key.end());
    return salted_hash;
}

size_t get_key_size() {
    return gcry_cipher_get_algo_keylen(PSME_CIPHER_ALGO);
}

size_t get_block_size() {
    return gcry_cipher_get_algo_blklen(PSME_CIPHER_ALGO);
}

size_t get_hash_size() {
    return gcry_md_get_algo_dlen(PSME_HASH_ALGO);
}

size_t get_kdf_key_size() {
    return KDF_KEY_SIZE;
}

size_t get_salt_size() {
    return KDF_SALT_SIZE;
}

void save_crypt_initializers(const std::string& path, const std::string& key, const std::string& init_vector) {
    int fd = ::open(path.c_str(), O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        throw std::runtime_error("Error with opening keyfile for write.");
    }

    auto cipher_initializers = key + init_vector;
    auto cipher_initializers_size = key.size() + init_vector.size();

    ssize_t written = ::write(fd, cipher_initializers.data(), cipher_initializers_size);
    ::close(fd);
    if (written != (long) cipher_initializers_size) {
        throw std::runtime_error("Cannot write keyfile");
    }
}

std::pair<std::string, std::string> load_crypt_initializers(const std::string& path) {
    std::ifstream ifile(path, std::ios::binary);
    char cipher_initializers[AES256_KEY_SIZE + AES256_BLOCK_SIZE];

    if (ifile.good()) {
        ifile.getline(cipher_initializers, AES256_KEY_SIZE + AES256_BLOCK_SIZE + 1);
        ifile.close();
    }
    else {
        throw std::runtime_error("Error with opening keyfile for read.");
    }

    auto key = std::string(cipher_initializers).substr(0, AES256_KEY_SIZE);
    auto init_vector = std::string(cipher_initializers).substr(AES256_KEY_SIZE, AES256_BLOCK_SIZE);

    return std::make_pair(key, init_vector);
}

std::string generate_key() {
    const std::string chars =
        "1234567890"
        "!@#$%^&*()-=_+,./?<>;'\\][{}|:"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    char key[AES256_KEY_SIZE + 1]{0};
    std::random_device rd; // Uses "/dev/urandom" by default or RDRND (CPU instruction) if available
    std::uniform_int_distribution<int> distribution(0, int(chars.size() - 1));

    for (uint32_t index = 0; index < AES256_KEY_SIZE; index++) {
        key[index] = chars[size_t((distribution(rd)))];
    }

    return std::string(key);
}

std::string generate_init_vector() {
    char init_vector[AES256_BLOCK_SIZE + 1]{0};
    gcry_create_nonce(init_vector, AES256_BLOCK_SIZE);

    return std::string(init_vector);
}

std::string generate_salt() {
    char salt[KDF_SALT_SIZE + 1]{0};
    gcry_create_nonce(salt, KDF_SALT_SIZE);

    return std::string(salt);
}

}