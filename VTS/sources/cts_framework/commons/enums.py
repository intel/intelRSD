"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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
    SUCCESS = "success"
    FAILED = "failed"


class HttpMethods:
    GET = "Get"
    PATCH = "Patch"


class ReturnCodes:
    OK = 200
    CREATED = 201
    NO_CONTENT = 204
    BAD_REQUEST = 400
    UNAUTHORIZED = 401
    FORBIDDEN = 403
    METHOD_NOT_ALLOWED = 405
    UNPROCESSABLE_ENTITY = 422
    NOT_IMPLEMENTED = 501


class ResourceTypes:
    SERVICE_ROOT = "ServiceRoot"


class LoggingLevel:
    ERROR = "ERROR"
    WARNING = "WARNING"
    MESSAGE = "MESSAGE"
    DEBUG = "DEBUG"
    RAW = "RAW"
