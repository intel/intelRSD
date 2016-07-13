/*!
 * \copyright
 * Copyright (c) 2015, 2016 Intel Corporation. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \file IPMB.h
 *
 * \brief IPMB basic structure definitions
 *
 */


#ifndef SHIM_H
#define SHIM_H

#include <stdio.h>

extern ssize_t sys_send (int __fd, const void *__buf, size_t __n, int __flags);

#endif /* SHIM_H */
