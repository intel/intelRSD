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

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.Violations;
import com.intel.podm.business.ViolationsDisclosingException;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.logger.LoggerFactory;

import javax.ws.rs.WebApplicationException;

import static com.intel.podm.rest.error.ErrorResponseBuilder.newErrorResponseBuilder;
import static com.intel.podm.rest.error.ErrorType.INVALID_HTTP_METHOD;
import static com.intel.podm.rest.error.ErrorType.NOT_FOUND;
import static com.intel.podm.rest.error.ErrorType.RESOURCES_STATE_MISMATCH;
import static com.intel.podm.rest.error.ErrorType.UNKNOWN_EXCEPTION;
import static com.intel.podm.rest.error.ErrorType.UNSUPPORTED_CREATION_REQUEST;
import static java.util.Collections.singletonList;

public final class PodmExceptions {
    private static final Logger LOGGER = LoggerFactory.getLogger(PodmExceptions.class);

    private PodmExceptions() {
    }

    public static WebApplicationException notFound() {
        return newErrorResponseBuilder(NOT_FOUND).toWebApplicationException();
    }

    public static WebApplicationException unsupportedCreationRequest(String detailedMessage) {
        return newErrorResponseBuilder(UNSUPPORTED_CREATION_REQUEST)
            .withDetails(singletonList(detailedMessage))
            .toWebApplicationException();
    }

    public static WebApplicationException unsupportedCreationRequest(Violations violations) {
        return newErrorResponseBuilder(UNSUPPORTED_CREATION_REQUEST)
            .withDetails(violations.asStringList())
            .toWebApplicationException();
    }

    public static WebApplicationException resourcesStateMismatch(String message, BusinessApiException e) {
        ErrorResponseBuilder responseBuilder = newErrorResponseBuilder(RESOURCES_STATE_MISMATCH).withMessage(message);
        Throwable cause = e.getCause();
        if (cause instanceof ViolationsDisclosingException) {
            ViolationsDisclosingException ex = (ViolationsDisclosingException) cause;
            responseBuilder.withDetails(ex.getViolations().asStringList());
        } else {
            responseBuilder.withDetails(singletonList(e.getMessage()));
        }
        return responseBuilder.toWebApplicationException();
    }

    public static WebApplicationException invalidHttpMethod(String detailedMessage) {
        return invalidHttpMethod(detailedMessage, null);
    }

    public static WebApplicationException invalidHttpMethod(String detailedMessage, BusinessApiException exception) {
        ErrorResponseBuilder responseBuilder = newErrorResponseBuilder(INVALID_HTTP_METHOD).withDetails(singletonList(detailedMessage));
        if (exception instanceof ViolationsDisclosingException) {
            ViolationsDisclosingException violationsDisclosingException = (ViolationsDisclosingException) exception;
            responseBuilder.withDetails(violationsDisclosingException.getViolations().asStringList());
        }
        return responseBuilder.toWebApplicationException();
    }

    public static WebApplicationException invalidHttpMethod() {
        return newErrorResponseBuilder(INVALID_HTTP_METHOD).toWebApplicationException();
    }

    public static WebApplicationException internalServerError() {
        return newErrorResponseBuilder(UNKNOWN_EXCEPTION).toWebApplicationException();
    }
}
