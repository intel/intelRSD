/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.redfish;

import lombok.Getter;
import org.springframework.http.HttpStatus;

import java.util.Map;
import java.util.stream.Stream;

import static com.intel.rsd.redfish.RedfishErrorCode.GatewayTimeout;
import static com.intel.rsd.redfish.RedfishErrorCode.GeneralError;
import static com.intel.rsd.redfish.RedfishErrorCode.InvalidEndpoint;
import static com.intel.rsd.redfish.RedfishErrorCode.NotImplemented;
import static com.intel.rsd.redfish.RedfishErrorCode.ServiceUnavailable;
import static java.util.function.Function.identity;
import static java.util.stream.Collectors.toMap;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.CONFLICT;
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;
import static org.springframework.http.HttpStatus.METHOD_NOT_ALLOWED;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.springframework.http.HttpStatus.URI_TOO_LONG;

@Getter
public enum RedfishErrorsArchetype {

    INVALID_ENDPOINT(NOT_FOUND, InvalidEndpoint, "Invalid endpoint."),
    INVALID_PAYLOAD(BAD_REQUEST, "Request payload is invalid or missing."),
    UNKNOWN_EXCEPTION(INTERNAL_SERVER_ERROR, "The server encountered an unexpected condition that prevented it from fulfilling the request."),
    RESOURCES_STATE_MISMATCH(CONFLICT, "Resources state mismatch."),
    INVALID_HTTP_METHOD(METHOD_NOT_ALLOWED, "Invalid HTTP request method."),
    REQUEST_URI_TOO_LONG(URI_TOO_LONG, "Malformed URI."),
    SERVICE_UNAVAILABLE(HttpStatus.SERVICE_UNAVAILABLE, ServiceUnavailable, "Server is currently unable to handle this request."),
    UNAUTHORIZED(HttpStatus.UNAUTHORIZED, "Authorization is required to access this resource."),
    FORBIDDEN(HttpStatus.FORBIDDEN, "Access denied."),
    NOT_ACCEPTABLE(HttpStatus.NOT_ACCEPTABLE, "Request contain not acceptable values in header."),
    NOT_IMPLEMENTED(HttpStatus.NOT_IMPLEMENTED, NotImplemented, "Not implemented."),
    GATEWAY_TIMEOUT(HttpStatus.GATEWAY_TIMEOUT, GatewayTimeout, "Gateway timeout.");

    private static final Map<HttpStatus, RedfishErrorsArchetype> REDFISH_ERROR_ARCHETYPES;

    private HttpStatus httpStatus;
    private RedfishErrorCode redfishErrorcode = GeneralError;
    private String message;

    RedfishErrorsArchetype(HttpStatus httpStatus, String message) {
        this.httpStatus = httpStatus;
        this.message = message;
    }

    RedfishErrorsArchetype(HttpStatus httpStatus, RedfishErrorCode redfishErrorcode, String message) {
        this.httpStatus = httpStatus;
        this.redfishErrorcode = redfishErrorcode;
        this.message = message;
    }

    static {
        REDFISH_ERROR_ARCHETYPES = Stream.of(values()).collect(toMap(archetype -> archetype.httpStatus, identity()));
    }

    public static RedfishErrorsArchetype findArchetype(HttpStatus statusCode) {
        return REDFISH_ERROR_ARCHETYPES.getOrDefault(statusCode, UNKNOWN_EXCEPTION);
    }
}
