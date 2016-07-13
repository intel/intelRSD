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
 * \file ll.h
 *
 * \brief Simple linked list implementation to manage messages requests waiting
 *        for responses and which clients they belong to
 *
 * \note Should migrate to boost libraries when moved to C++.  The binary tree
 *       was overkill and was reduced to a linked list in the first
 *       implementation.  It turns out that for our purposes, doubly linked list
 *       is sufficient.
 *
 */



#ifndef ll_H
#define ll_H


#include "IPMB.h"
#include <stdbool.h>


int ll_init(void);
int ll_destroy(void);
int ll_get(int* const r, Message_t const* const m, bool i2c);
int ll_post(Message_t const* const m);
int ll_ready(Message_t* const m);
int ll_flush(int resource);


#endif /* ll_H */
