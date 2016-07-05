/**
 * Copyright (c)  2015, Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __ASSETD_PARSER_H__
#define __ASSETD_PARSER_H__

#include <stdio.h>
#include "libjsonrpc/jsonrpc.h"

/**
 * @brief parse the request which send by asset moudle.
 *
 * @param  string		the string recv from asset module.
 * @param  req			the req inforation by asset module.
 * @param  func_id		func id
 *
 */
int parse_req(char *string, jrpc_req_pkg_t *req, int *func_id);
int parse_rsp(char * string, jrpc_rsp_pkg_t *rsp);

#endif

