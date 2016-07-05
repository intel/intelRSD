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


#ifndef __LIBJSONRPC_JSONRPCERR_H__
#define __LIBJSONRPC_JSONRPCERR_H__

enum {

	/*J SON RPC ERROR CODE */
	JSONRPC_PARSE_ERROR = -32700,
	JSONRPC_INVALID_REQ = -32600,
	JSONRPC_METHOD_NOTFOUND = -32601,
	JSONRPC_INVALID_PARAMS = -32602,
	JSONRPC_INTERNAL_ERR = -32603,
	JSONRPC_SERVER_ERR = -32000,

	JSONRPC_OEM_NODE_NOTFOUND = -32001,
	JSONRPC_OEM_ATTR_NOTFOUND = -32002,
	JSONRPC_OEM_HANDLING_ERROR = -32003,
	JSONRPC_OEM_MALLOC_ERROR = -32004,
	JSONRPC_FILL_PARAM_ERR = -32005,
	JSONRPC_OEM_HANDLE_SUCCESS = -32099,

	ERR_JSONRPC_RSP_INVALID = -100
};


#endif

