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
 * \file test_client_usage.h
 *
 * \brief command line parameter / argument related routines and definitions for
 *        the test client utility
 *
 */



#ifndef test_client_usage_H
#define test_client_usage_H


#include "IPMB.h"
#include <stdbool.h>
#include <sys/types.h>


// Routines exported
void usage(int rc);
void process_commandline(int ac, char *av[]);

// Test/configuration variables:
extern int      gPlanned;
extern bool     gHang_connection;
extern int      gConnection_holdtime;
extern bool     gNo_server;
extern bool     gServer;
extern bool     gClose_prematurely;
extern char     gClient_type[MAX_IPMB_PKT_SIZE*2];
extern size_t   gClient_type_len;


extern int  diag(const char* fmt, ...);
extern bool pass(const char* desc);
extern bool fail(const char* desc);
#define ok(test,desc)   ((test) ? pass(desc) : fail(desc))
extern void plan(int planned);
extern void plan_lazy(void);

#endif /* test_client_usage_H */
