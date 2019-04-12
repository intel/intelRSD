/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 *
 * @file lvm.hpp
 * @brief LVM library header
 * */

#pragma once


extern "C" {

/* Define to ignore LVM2APP deprecation warning */
#ifndef _BUILDING_LVM
#define _BUILDING_LVM
#endif

#include <lvm2app.h>

#ifdef _BUILDING_LVM
#undef _BUILDING_LVM
#endif

}