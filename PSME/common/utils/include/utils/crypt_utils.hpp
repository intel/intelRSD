/*!
 * @brief Cryptographic utils.

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
 * @file utils/crypt_utils.hpp
 */

#pragma once

#include <string>

namespace utils {

/*!
 * @brief Encrypts the given data (by default uses AES256 CBC).
 * @param plain_text string with the plain text to be encrypted.
 * @param key string with the encryption key.
 * @param init_vector string with the initialization vector.
 * @return decrypted data.
 * */
std::string encrypt(std::string& plain_text, const std::string& key, std::string& init_vector);

/*!
 * @brief Decrypts the given data (by default uses AES256 CBC).
 * @param cipher_text string with the encrypted text to be decrypted.
 * @param key string with the encryption key.
 * @param init_vector string with the initialization vector.
 * @return decrypted data.
 * */
std::string decrypt(const std::string& cipher_text, const std::string& key, const std::string& init_vector);

/*!
 * @brief Calculates message digest (by default uses SHA512).
 * @param plain_text string with the plain text to be hashed.
 * @return message digest (hash).
 * */
std::string hash(const std::string& plain_text);

/*!
 * @brief Generates key/hash with PBKDF2 key derivation function (SHA512 based).
 * @param password string with the plain text password to be hashed.
 * @param salt string with random data
 * @return password hash with salt added.
 * */
std::string salted_hash(const std::string& password, const std::string& salt);

/*!
 * @brief Gets key size for a configured cryptographic algorithm (32-bytes for AES256).
 * @return cipher key size in Bytes.
 * */
size_t get_key_size();

/*!
 * @brief Gets block size for a configured cryptographic algorithm (16-bytes for AES256).
 * @return cipher block size in Bytes.
 * */
size_t get_block_size();

/*!
 * @brief Gets digest size for a configured hash algorithm.
 * @return message digest size in Bytes.
 * */
size_t get_hash_size();

/*!
 * @brief Gets derived key/hash size for a configured PBKDF2 function.
 * @return key/hash size in Bytes.
 * */
size_t get_kdf_key_size();

/*!
 * @brief Gets salt size for a configured PBKDF2 function.
 * @return salt size in Bytes.
 * */
size_t get_salt_size();

/*!
 * @brief Loads encryption key and initialization vector from the file.
 * @param path string with path to keyfile.
 * @return pair containing key and initialization vector.
 * */
std::pair<std::string, std::string> load_crypt_initializers(const std::string& path);

/*!
 * @brief Saves encryption key and initialization vector to the file. File will be created.
 * @param path path to the file where key/iv will be stored.
 * @param key string with key.
 * @param init_vector string with initialization vector.
 * */
void save_crypt_initializers(const std::string& path, const std::string& key, const std::string& init_vector);

/*!
 * @brief Generates random 32-byte key with alphanumeric and special characters.
 * @return encryption key (to be used for AES256 encryption by default).
 * */
std::string generate_key();

/*!
 * @brief Generates random 16-byte initialization vector.
 * @return initialization vector (to be used for AES256 encryption by default).
 * */
std::string generate_init_vector();

/*!
 * @brief Generates random 16-byte salt.
 * @return cryptographic salt (to be used for PBKDF2 by default).
 * */
std::string generate_salt();

}