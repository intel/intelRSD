/*
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.podm.rest.error;

import javax.ws.rs.core.Response.Status;

import static javax.ws.rs.core.Response.Status.BAD_REQUEST;
import static javax.ws.rs.core.Response.Status.CONFLICT;
import static javax.ws.rs.core.Response.Status.INTERNAL_SERVER_ERROR;
import static javax.ws.rs.core.Response.Status.METHOD_NOT_ALLOWED;
import static javax.ws.rs.core.Response.Status.NOT_ACCEPTABLE;
import static javax.ws.rs.core.Response.Status.REQUEST_URI_TOO_LONG;

@SuppressWarnings({"LineLength"})
public enum ErrorType {
    UNKNOWN_EXCEPTION(INTERNAL_SERVER_ERROR, "UnknownException", "The server encountered an unexpected condition that prevented it from fulfilling the request"),
    NOT_FOUND(Status.NOT_FOUND, "InvalidEndpoint", "Invalid endpoint"),
    RESOURCES_STATE_MISMATCH(CONFLICT, "ResourcesStateMismatch"),
    INVALID_HTTP_METHOD(METHOD_NOT_ALLOWED, "InvalidHttpMethod", "Invalid HTTP request method"),
    MALFORMED_URI(BAD_REQUEST, "MalformedUri", "Malformed URI"),
    BAD_ACCEPT_HEADER(NOT_ACCEPTABLE, "BadAcceptHeader", "Bad Accept field in request header"),
    URI_TOO_LONG(REQUEST_URI_TOO_LONG, "RequestUriTooLong", "Malformed URI"),
    INVALID_PAYLOAD(BAD_REQUEST, "InvalidPayload", "Request payload is invalid or missing"),
    SERVICE_UNAVAILABLE(Status.SERVICE_UNAVAILABLE, "ServiceUnavailable", "Server is currently unable to handle this request."),
    UNSUPPORTED_CREATION_REQUEST(BAD_REQUEST, "UnsupportedCreationRequest", "Unable to create machine due to error in machine template or bad creation request"),
    UNAUTHORIZED(Status.UNAUTHORIZED, "Unauthorized", "Authorization is required to access this resource."),
    FORBIDDEN(Status.FORBIDDEN, "Forbidden", "Access denied."),
    NOT_IMPLEMENTED(Status.NOT_IMPLEMENTED, "NotImplemented", "Not implemented");

    private static final String ERROR_CODE_PREFIX = "Base.1.0.";

    private final int errorCode;
    private final Status status;
    private final String errorType;
    private final String message;

    ErrorType(Status status, String errorType) {
        this.status = status;
        this.errorType = errorType;
        this.errorCode = status.getStatusCode();
        this.message = null;
    }

    ErrorType(Status status, String errorType, String message) {
        this.status = status;
        this.errorType = errorType;
        this.errorCode = status.getStatusCode();
        this.message = message;
    }

    public Status getStatus() {
        return status;
    }

    public String getErrorType() {
        return ERROR_CODE_PREFIX + errorType;
    }

    public int getErrorCode() {
        return errorCode;
    }

    public String getMessage() {
        return message;
    }
}
