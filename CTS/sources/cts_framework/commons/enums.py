"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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
"""


class RequestStatus:
    SUCCESS = True
    FAILED = False


class HttpMethods:
    GET = "Get"
    PATCH = "Patch"
    POST = "Post"
    DELETE = "Delete"


class ReturnCodes:
    OK = 200
    CREATED = 201
    ACCEPTED = 202
    NO_CONTENT = 204
    INVALID_FORWARDING = 301
    BAD_REQUEST = 400
    INVALID_CERTS = 401
    NOT_FOUND = 404
    METHOD_NOT_ALLOWED = 405
    TIMEOUT = 408
    CONFLICT = 409
    GONE = 410
    UNPROCESSABLE_ENTITY = 422
    INTERNAL_SERVER_ERROR = 500
    NOT_IMPLEMENTED = 501
    SERVICE_UNAVAILABLE = 503


class ErrorReturnCodes:
    ErrorCodes = {
        301: "Invalid forwarding",
        400: "Bad request from endpoint",
        401: "Invalid certification files",
        404: "No endpoint present at the provided ApiEndpoint",
        408: "Timeout reached",
        500: "Internal server error",
        503: "Service unavailable"
    }

class ResourceTypes:
    SERVICE_ROOT = "ServiceRoot"


class LoggingLevel:
    ERROR = "ERROR"
    WARNING = "WARNING"
    MESSAGE = "MESSAGE"
    DEBUG = "DEBUG"
    CONTROL = "CONTROL"
    LEVEL = (set([ERROR, WARNING]),
             set([ERROR, WARNING, MESSAGE]),
             set([ERROR, WARNING, MESSAGE, CONTROL]),
             set([ERROR, WARNING, MESSAGE, DEBUG, CONTROL])
             )

class HTTPServerErrors:
    ERROR = {500: "Internal Server Error",
             501: "Not Implemented",
             502: "Bad Gateway",
             503: "Service Unavailable",
             504: "Gateway Timeout",
             505: "HTTP Version Not Supported",
             506: "Variant Also Negotiates",
             507: "Insufficient Storage",
             508: "Loop Detected",
             509: "Bandwidth Limit Exceeded",
             510: "Not Extended",
             511: "Network Authentication Required"}