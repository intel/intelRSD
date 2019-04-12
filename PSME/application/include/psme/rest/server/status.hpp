/*
 * Copyright (C) 2014 MediaSift Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * */

#pragma once
#include <string>

namespace psme {
namespace rest {
namespace server {

namespace status_1XX {
	// 1XX - Informational
	const std::uint32_t CONTINUE            = 100;
	const std::uint32_t SWITCHING_PROTOCOLS = 101;
	const std::uint32_t PROCESSING          = 102;
}

namespace status_2XX {
	// 2XX - Success
	const std::uint32_t OK                   = 200;
	const std::uint32_t CREATED              = 201;
	const std::uint32_t ACCEPTED             = 202;
	const std::uint32_t NON_AUTHORITIVE_INFO = 203;
	const std::uint32_t NO_CONTENT           = 204;
	const std::uint32_t RESET_CONTENT        = 205;
	const std::uint32_t PARTIAL_CONTENT      = 206;
	const std::uint32_t MULTI_STATUS         = 207;
	const std::uint32_t ALREADY_REPORTED     = 208;
	const std::uint32_t IM_USED              = 226;
}

namespace status_3XX {
	// 3XX - Redirectional
	const std::uint32_t MULTI_CHOICES     = 300;
	const std::uint32_t MOVED_PERMANENTLY = 301;
	const std::uint32_t FOUND             = 302;
	const std::uint32_t SEE_OTHER         = 303;
	const std::uint32_t NOT_MODIFIED      = 304;
	const std::uint32_t USE_PROXY         = 305;
	const std::uint32_t SWITCH_PROXY      = 306;
	const std::uint32_t TEMP_REDIRECT     = 307;
	const std::uint32_t PERM_REDIRECT     = 308;
}

namespace status_4XX {
	// 4XX - Client error
	const std::uint32_t BAD_REQUEST                 = 400;
	const std::uint32_t UNAUTHORIZED                = 401;
	const std::uint32_t PAYMENT_REQUIRED            = 402;
	const std::uint32_t FORBIDDEN                   = 403;
	const std::uint32_t NOT_FOUND                   = 404;
	const std::uint32_t METHOD_NOT_ALLOWED          = 405;
	const std::uint32_t NOT_ACCEPTABLE              = 406;
	const std::uint32_t PROXY_AUTH_REQUIRED         = 407;
	const std::uint32_t REQUEST_TIMEOUT             = 408;
	const std::uint32_t CONFLICT                    = 409;
	const std::uint32_t GONE                        = 410;
	const std::uint32_t LENGTH_REQUIRED             = 411;
	const std::uint32_t PRECONDITION_FAILED         = 412;
	const std::uint32_t REQ_ENTITY_TOO_LARGE        = 413;
	const std::uint32_t REQ_URI_TOO_LONG            = 414;
	const std::uint32_t UNSUPPORTED_MEDIA_TYPE      = 415;
	const std::uint32_t REQ_RANGE_NOT_SATISFYABLE   = 416;
	const std::uint32_t EXPECTATION_FAILED          = 417;
	const std::uint32_t IM_A_TEAPOT                 = 418;
	const std::uint32_t AUTH_TIMEOUT                = 419; // not in RFC 2616
	const std::uint32_t UNPROCESSABLE_ENTITY        = 422;
	const std::uint32_t LOCKED                      = 423;
	const std::uint32_t FAILED_DEPENDENCY           = 424;
	const std::uint32_t UPGRADE_REQUIRED            = 426;
	const std::uint32_t PRECONDITION_REQUIRED       = 428;
	const std::uint32_t TOO_MANY_REQUESTS           = 429;
	const std::uint32_t REQ_HEADER_FIELDS_TOO_LARGE = 431;
}

namespace status_5XX {
	// 5XX - Server error
	const std::uint32_t INTERNAL_SERVER_ERROR       = 500;
	const std::uint32_t NOT_IMPLEMENTED             = 501;
	const std::uint32_t BAD_GATEWAY                 = 502;
	const std::uint32_t SERVICE_UNAVAILABLE         = 503;
	const std::uint32_t GATEWAY_TIMEOUT             = 504;
	const std::uint32_t HTTP_VERSION_NOT_SUPPORTED  = 505;
	const std::uint32_t VARIANT_ALSO_NEGOTIATES     = 506;
	const std::uint32_t INSUFFICIENT_STORAGE        = 507;
	const std::uint32_t LOOP_DETECTED               = 508;
	const std::uint32_t NOT_EXTENDED                = 510;
	const std::uint32_t NETWORK_AUTH_REQUIRED       = 511;
	const std::uint32_t NETWORK_READ_TIMEOUT_ERR    = 598;
	const std::uint32_t NETWORK_CONNECT_TIMEOUT_ERR = 599;
}

namespace status {

/*
 * @brief Converts an HTTP status code std::uint32_to a known reason string.
 *
 * The reason string is a small line of text that gives a std::uint32_t as to the underlying meaning of the
 * status code for debugging purposes.
 * */
inline const std::string status_to_reason(std::uint32_t status_code)
{
	switch ( status_code )
	{
	// 1XX - Informational
	case status_1XX::CONTINUE:
		return "CONTINUE";
	case status_1XX::SWITCHING_PROTOCOLS:
		return "SWITCHING PROTOCOLS";
	case status_1XX::PROCESSING:
		return "PROCESSING";
	// 2XX - Success
	case status_2XX::OK:
		return "OK";
	case status_2XX::CREATED:
		return "CREATED";
	case status_2XX::ACCEPTED:
		return "ACCEPTED";
	case status_2XX::NON_AUTHORITIVE_INFO:
		return "NON AUTHORITIVE INFO";
	case status_2XX::NO_CONTENT:
		return "NO CONTENT";
	case status_2XX::RESET_CONTENT:
		return "RESET CONTENT";
	case status_2XX::PARTIAL_CONTENT:
		return "PARTIAL CONTENT";
	case status_2XX::MULTI_STATUS:
		return "MULTI STATUS";
	case status_2XX::ALREADY_REPORTED:
		return "ALREADY REPORTED";
	case status_2XX::IM_USED:
		return "IM USED";
	// 3XX - Redirectional
	case status_3XX::MULTI_CHOICES:
		return "MULTI CHOICES";
	case status_3XX::MOVED_PERMANENTLY:
		return "MOVED PERMANENTLY";
	case status_3XX::FOUND:
		return "FOUND";
	case status_3XX::SEE_OTHER:
		return "SEE OTHER";
	case status_3XX::NOT_MODIFIED:
		return "NOT MODIFIED";
	case status_3XX::USE_PROXY:
		return "USE PROXY";
	case status_3XX::SWITCH_PROXY:
		return "SWITCH PROXY";
	case status_3XX::TEMP_REDIRECT:
		return "TEMP REDIRECT";
	case status_3XX::PERM_REDIRECT:
		return "PERM REDIRECT";
	// 4XX - Client error
	case status_4XX::BAD_REQUEST:
		return "BAD REQUEST";
	case status_4XX::UNAUTHORIZED:
		return "UNAUTHORIZED";
	case status_4XX::PAYMENT_REQUIRED:
		return "PAYMENT REQUIRED";
	case status_4XX::FORBIDDEN:
		return "FORBIDDEN";
	case status_4XX::NOT_FOUND:
		return "NOT FOUND";
	case status_4XX::METHOD_NOT_ALLOWED:
		return "METHOD NOT ALLOWED";
	case status_4XX::NOT_ACCEPTABLE:
		return "NOT ACCEPTABLE";
	case status_4XX::PROXY_AUTH_REQUIRED:
		return "PROXY AUTH REQUIRED";
	case status_4XX::REQUEST_TIMEOUT:
		return "REQUEST TIMEOUT";
	case status_4XX::CONFLICT:
		return "CONFLICT";
	case status_4XX::GONE:
		return "GONE";
	case status_4XX::LENGTH_REQUIRED:
		return "LENGTH REQUIRED";
	case status_4XX::PRECONDITION_FAILED:
		return "PRECONDITION FAILED";
	case status_4XX::REQ_ENTITY_TOO_LARGE:
		return "REQ ENTITY TOO LARGE";
	case status_4XX::REQ_URI_TOO_LONG:
		return "REQ URI TOO LONG";
	case status_4XX::UNSUPPORTED_MEDIA_TYPE:
		return "UNSUPPORTED MEDIA TYPE";
	case status_4XX::REQ_RANGE_NOT_SATISFYABLE:
		return "REQ RANGE NOT SATISFYABLE";
	case status_4XX::EXPECTATION_FAILED:
		return "EXPECTATION FAILED";
	case status_4XX::IM_A_TEAPOT:
		return "IM A TEAPOT";
	case status_4XX::AUTH_TIMEOUT: // not in RFC 2616
		return "AUTH TIMEOUT";
	case status_4XX::UNPROCESSABLE_ENTITY:
		return "UNPROCESSABLE ENTITY";
	case status_4XX::LOCKED:
		return "LOCKED";
	case status_4XX::FAILED_DEPENDENCY:
		return "FAILED DEPENDENCY";
	case status_4XX::UPGRADE_REQUIRED:
		return "UPGRADE REQUIRED";
	case status_4XX::PRECONDITION_REQUIRED:
		return "PRECONDITION REQUIRED";
	case status_4XX::TOO_MANY_REQUESTS:
		return "TOO MANY REQUESTS";
	case status_4XX::REQ_HEADER_FIELDS_TOO_LARGE:
		return "REQ HEADER FIELDS TOO LARGE";
	// 5XX - Server error
	case status_5XX::INTERNAL_SERVER_ERROR:
		return "INTERNAL SERVER ERROR";
	case status_5XX::NOT_IMPLEMENTED:
		return "NOT IMPLEMENTED";
	case status_5XX::BAD_GATEWAY:
		return "BAD GATEWAY";
	case status_5XX::SERVICE_UNAVAILABLE:
		return "SERVICE UNAVAILABLE";
	case status_5XX::GATEWAY_TIMEOUT:
		return "GATEWAY TIMEOUT";
	case status_5XX::HTTP_VERSION_NOT_SUPPORTED:
		return "HTTP VERSION NOT SUPPORTED";
	case status_5XX::VARIANT_ALSO_NEGOTIATES:
		return "VARIANT ALSO NEGOTIATES";
	case status_5XX::INSUFFICIENT_STORAGE:
		return "INSUFFICIENT STORAGE";
	case status_5XX::LOOP_DETECTED:
		return "LOOP DETECTED";
	case status_5XX::NOT_EXTENDED:
		return "NOT EXTENDED";
	case status_5XX::NETWORK_AUTH_REQUIRED:
		return "NETWORK AUTH REQUIRED";
	case status_5XX::NETWORK_READ_TIMEOUT_ERR:
		return "NETWORK READ TIMEOUT ERR";
	case status_5XX::NETWORK_CONNECT_TIMEOUT_ERR:
		return "NETWORK CONNECT TIMEOUT ERR";
    default: return "-";
	}
	return "-";
}

} }
}
}

