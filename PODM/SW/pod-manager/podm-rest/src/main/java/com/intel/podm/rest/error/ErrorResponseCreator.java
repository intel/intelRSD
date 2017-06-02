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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.redfish.RedfishErrorResponse;
import com.intel.podm.common.types.redfish.RedfishErrorResponse.Error.ExtendedInfo;
import com.intel.podm.rest.representation.json.errors.ErrorType;
import com.intel.podm.rest.representation.json.templates.RedfishErrorResponseJson;
import com.intel.podm.rest.representation.json.templates.RedfishErrorResponseJson.ExtendedInfoJson;

import javax.ws.rs.WebApplicationException;
import javax.ws.rs.core.Response;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.utils.Contracts.requires;
import static java.util.Optional.ofNullable;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.status;
import static org.apache.commons.lang.StringUtils.trimToNull;

public final class ErrorResponseCreator {
    private ErrorType errorType;
    private String message;
    private RedfishErrorResponse errorResponse;
    private String[] details;

    private ErrorResponseCreator(ErrorType errorType) {
        this.errorType = errorType;
    }

    public static ErrorResponseCreator from(ErrorType responseType) {
        return new ErrorResponseCreator(responseType);
    }

    public ErrorResponseCreator withMessage(String message) {
        this.message = message;
        return this;
    }

    public ErrorResponseCreator withDetails(String... details) {
        this.details = details;
        return this;
    }

    public ErrorResponseCreator withErrorResponse(RedfishErrorResponse errorResponse) {
        this.errorResponse = errorResponse;
        return this;
    }

    public Response create() {
        String message = ofNullable(this.message).orElse(errorType.getMessage());
        requires(message != null, "Could not create error response with null message");
        ErrorResponseJson entity = new ErrorResponseJson(errorType.getErrorType(), message, errorResponse, details);
        return
                status(errorType.getErrorCode())
                .type(APPLICATION_JSON)
                .entity(entity)
                .build();
    }

    public WebApplicationException toWebApplicationException() {
        return new WebApplicationException(create());
    }

    static class ErrorResponseJson {
        @JsonProperty("error")
        RedfishErrorResponseJson error;

        ErrorResponseJson(String errorType, String errorMessage, RedfishErrorResponse errorResponse, String... details) {
            List<ExtendedInfoJson> detailedErrors = new ArrayList<>();
            if (details != null) {
                processDetails(detailedErrors, details);
            }
            if (errorResponse != null && errorResponse.getError() != null) {
                processRedfishError(detailedErrors, errorResponse.getError());
            }
            this.error = new RedfishErrorResponseJson(errorType, errorMessage, detailedErrors);
        }

        private void processRedfishError(List<ExtendedInfoJson> detailedErrors, RedfishErrorResponse.Error error) {
            ExtendedInfoJson detailJson = new ExtendedInfoJson(error.getCode(), error.getMessage());
            detailedErrors.add(detailJson);

            for (ExtendedInfo info : error.getExtendedInfo()) {
                ExtendedInfoJson extendedInfoJson = new ExtendedInfoJson(info.getMessageId(), info.getMessage());
                detailedErrors.add(extendedInfoJson);
            }
        }

        private void processDetails(List<ExtendedInfoJson> detailedErrors, String[] details) {
            for (String detail : details) {
                if (trimToNull(detail) != null) {
                    ExtendedInfoJson detailJson = new ExtendedInfoJson(null, detail);
                    detailedErrors.add(detailJson);
                }
            }
        }
    }
}
