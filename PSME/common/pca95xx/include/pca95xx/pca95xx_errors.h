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
 * @file pca95xx/pca95xx_errors.h
 *
 * @brief PCA95XX errors interface
 * */

#ifndef PCA95XX_ERRORS_H
#define PCA95XX_ERRORS_H

#include "pca95xx/pca95xx.h"

/*! Common helpful tag error string example for loggers */
#define PCA95XX_ERROR_TAG_STRING            "[Error][PPCA95XX]"

/*!
 * @brief Get error information string from error code number
 *
 * @param[in]   error_code Error code
 * @return      Error string
 * */
const char* pca95xx_get_error_string(int error_code);

#endif /* PCA95XX_ERRORS_H */
