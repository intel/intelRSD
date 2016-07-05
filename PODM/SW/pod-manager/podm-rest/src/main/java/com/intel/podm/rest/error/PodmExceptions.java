/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.allocation.validation.Violations;
import com.intel.podm.common.types.redfish.RedfishErrorResponse;

import javax.ws.rs.WebApplicationException;

import static com.intel.podm.rest.error.ErrorResponseCreator.from;
import static com.intel.podm.rest.representation.json.errors.ErrorType.INVALID_HTTP_METHOD;
import static com.intel.podm.rest.representation.json.errors.ErrorType.INVALID_PAYLOAD;
import static com.intel.podm.rest.representation.json.errors.ErrorType.NOT_FOUND;
import static com.intel.podm.rest.representation.json.errors.ErrorType.RESOURCES_STATE_MISMATCH;
import static com.intel.podm.rest.representation.json.errors.ErrorType.SERVICE_UNAVAILABLE;
import static com.intel.podm.rest.representation.json.errors.ErrorType.UNKNOWN_EXCEPTION;
import static com.intel.podm.rest.representation.json.errors.ErrorType.UNSUPPORTED_CREATION_REQUEST;

public final class PodmExceptions {
    private PodmExceptions() {
    }

    public static WebApplicationException notFound() {
        return from(NOT_FOUND).toWebApplicationException();
    }

    public static WebApplicationException serviceUnavailable() {
        return from(SERVICE_UNAVAILABLE).toWebApplicationException();
    }

    public static WebApplicationException unsupportedCreationRequest(String detailedMessage) {
        return from(UNSUPPORTED_CREATION_REQUEST)
                .withDetails(detailedMessage)
                .toWebApplicationException();
    }

    public static WebApplicationException unsupportedCreationRequest(Violations violations) {
        return from(UNSUPPORTED_CREATION_REQUEST)
                .withDetails(violations.toStringArray())
                .toWebApplicationException();
    }

    public static WebApplicationException resourcesStateMismatch(String message, String detailedMessage) {
        return from(RESOURCES_STATE_MISMATCH)
                .withMessage(message)
                .withDetails(detailedMessage)
                .toWebApplicationException();
    }

    public static WebApplicationException resourcesStateMismatch(String message, RedfishErrorResponse errorResponse) {
        return from(RESOURCES_STATE_MISMATCH)
                .withMessage(message)
                .withErrorResponse(errorResponse)
                .toWebApplicationException();
    }
    public static WebApplicationException resourcesStateMismatch(String message, Violations violations) {
        return from(RESOURCES_STATE_MISMATCH)
                .withMessage(message)
                .withDetails(violations.toStringArray())
                .toWebApplicationException();
    }

    public static WebApplicationException invalidPayload(String... detailedMessage) {
        return from(INVALID_PAYLOAD)
                .withDetails(detailedMessage)
                .toWebApplicationException();
    }

    public static WebApplicationException invalidPayload(String message, RedfishErrorResponse response) {
        return from(INVALID_PAYLOAD)
                .withDetails(message)
                .withErrorResponse(response)
                .toWebApplicationException();
    }

    public static WebApplicationException invalidHttpMethod(String... detailedMessage) {
        return from(INVALID_HTTP_METHOD)
                .withDetails(detailedMessage)
                .toWebApplicationException();
    }

    public static WebApplicationException internalServerError(String message, String detailedMessage) {
        return from(UNKNOWN_EXCEPTION)
                .withMessage(message)
                .withDetails(detailedMessage)
                .toWebApplicationException();
    }

    public static WebApplicationException internalServerError() {
        return from(UNKNOWN_EXCEPTION)
                .toWebApplicationException();
    }
}
