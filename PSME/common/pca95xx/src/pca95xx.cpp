/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @file pca95xx.cpp
 *
 * @brief PCA95XX C++ implementation
 * */

#include "pca95xx/pca95xx.hpp"

extern "C" {
#include "pca95xx/pca95xx_errors.h"
}

#include <stdexcept>

using namespace pca95xx_cpp;

Pca95xx::Pca95xx(enum pca95xx_model model) : m_pca95xx {} {
    int status = pca95xx_init(&m_pca95xx, model);
    if (PCA95XX_SUCCESS != status) {
        throw std::runtime_error(pca95xx_get_error_string(status));
    }
}

void Pca95xx::set_model(enum pca95xx_model model) {
    int status = pca95xx_set_model(&m_pca95xx, model);
    if (PCA95XX_SUCCESS != status) {
        throw std::runtime_error(pca95xx_get_error_string(status));
    }
}

enum pca95xx_model Pca95xx::get_model(const char* name) {
    return pca95xx_get_model_by_name(name);
}

enum pca95xx_model Pca95xx::get_model(const std::string& name) {
    return pca95xx_get_model_by_name(name.c_str());
}

void Pca95xx::set_i2c_slave_address(unsigned int slave_address) {
    pca95xx_set_i2c_slave_address(&m_pca95xx, slave_address);
}

void Pca95xx::set_i2c_bus_number(unsigned int bus_number) {
    pca95xx_set_i2c_bus_number(&m_pca95xx, bus_number);
}

std::string Pca95xx::get_name() {
    const char* name = pca95xx_get_name(&m_pca95xx);
    if (nullptr == name) {
        throw std::runtime_error(pca95xx_get_error_string(PCA95XX_ERROR));
    }
    return std::string(name);
}

void Pca95xx::set_output(unsigned int bank, unsigned int value) {
    int status = pca95xx_set_output(&m_pca95xx, bank, value);
    if (PCA95XX_SUCCESS != status) {
        throw std::runtime_error(pca95xx_get_error_string(status));
    }
}

unsigned int Pca95xx::get_input(unsigned int bank) {
    int value = pca95xx_get_input(&m_pca95xx, bank);
    if (0 > value) {
        throw std::runtime_error(pca95xx_get_error_string(value));
    }
    return static_cast<unsigned int>(value);
}

void Pca95xx::set_config(unsigned int bank, unsigned int value) {
    int status = pca95xx_set_config(&m_pca95xx, bank, value);
    if (PCA95XX_SUCCESS != status) {
        throw std::runtime_error(pca95xx_get_error_string(status));
    }
}

unsigned int Pca95xx::get_config(unsigned int bank) {
    int value = pca95xx_get_config(&m_pca95xx, bank);
    if (0 > value) {
        throw std::runtime_error(pca95xx_get_error_string(value));
    }
    return static_cast<unsigned int>(value);
}

void Pca95xx::set_polarity_inv(unsigned int bank, unsigned int value) {
    int status = pca95xx_set_polarity_inv(&m_pca95xx, bank, value);
    if (PCA95XX_SUCCESS != status) {
        throw std::runtime_error(pca95xx_get_error_string(status));
    }
}

unsigned int Pca95xx::get_polarity_inv(unsigned int bank) {
    int value = pca95xx_get_config(&m_pca95xx, bank);
    if (0 > value) {
        throw std::runtime_error(pca95xx_get_error_string(value));
    }
    return static_cast<unsigned int>(value);
}
