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


#ifndef __ASSETD_HANDELER_H__
#define __ASSETD_HANDELER_H__

#include <stdio.h>
#include <libjsonrpc/jsonrpc.h>

/**
 * @brief process the requsest which send by asset module.
 *
 * @param  func_id		func id
 * @param  req			the req inforation by asset module.
 * @param  resp			response after call the function.
 *
 */
int process_req(int func_id, jrpc_req_pkg_t *req, json_t *resp);

#endif

