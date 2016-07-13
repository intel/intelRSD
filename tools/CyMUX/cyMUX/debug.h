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
 * \file debug.h
 *
 * \brief General debug pattern/routines used
 */

#ifndef debug_H
#define debug_H


#include "IPMB.h"
#include <stdint.h>
#include <sys/types.h>
#include <syslog.h>
#include <sys/cdefs.h>


extern void message_dump(const char* __restrict fmt, Message_t const* const m, ...)
                __attribute__((__format__(__printf__, 1, 3))) __nonnull((1, 2));
extern void buffer_dump(const char * __restrict fmt, uint8_t const* const b, ssize_t l, ...)
                __attribute__((__format__(__printf__, 1, 4))) __nonnull((1, 2));
extern void debug(int verbosity, const char * __restrict fmt, ...)
                __attribute__((__format__(__printf__, 2, 3))) __nonnull((2));
extern void init_diag(void);
extern void cleanup_diag(void);


#ifndef LOG_DEBUG2
#define LOG_DEBUG2 (LOG_DEBUG+1)
#endif


extern int gVerbosity;              // Verbosity of info/debug to dump


#endif /* debug_H */
