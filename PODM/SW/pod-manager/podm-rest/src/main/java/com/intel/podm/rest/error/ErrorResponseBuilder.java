/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import com.intel.podm.business.errors.ErrorDto;
import com.intel.podm.common.types.redfish.RedfishErrorResponse;
import com.intel.podm.common.types.redfish.RedfishErrorResponse.Error;
import com.intel.podm.common.types.redfish.RedfishErrorResponse.Error.ExtendedInfo;

import javax.ws.rs.WebApplicationException;
import javax.ws.rs.core.Response;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.business.errors.ErrorDto.ErrorBuilder.newErrorBuilder;
import static com.intel.podm.business.errors.ExtendedInfoDto.ExtendedInfoBuilder.newExtendedInfoBuilder;
import static com.intel.podm.common.utils.Contracts.requires;
import static java.util.Collections.emptyList;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static javax.ws.rs.core.Response.status;
import static org.apache.commons.lang3.StringUtils.trimToNull;

public final class ErrorResponseBuilder {
    private final ErrorType errorType;
    private String message;
    private RedfishErrorResponse redfishErrorResponse;
    private List<String> details;

    private ErrorResponseBuilder(ErrorType errorType) {
        this.errorType = errorType;
    }

    public static ErrorResponseBuilder newErrorResponseBuilder(ErrorType responseType) {
        return new ErrorResponseBuilder(responseType);
    }

    public ErrorResponseBuilder withMessage(String message) {
        this.message = message;
        return this;
    }

    public ErrorResponseBuilder withDetails(List<String> details) {
        this.details = details;
        return this;
    }

    public ErrorResponseBuilder withErrorResponse(RedfishErrorResponse errorResponse) {
        this.redfishErrorResponse = errorResponse;
        return this;
    }

    public Response build() {
        String message = ofNullable(this.message).orElse(errorType.getMessage());
        requires(message != null, "Could not create error response with null message");

        ErrorDto.ErrorBuilder errorBuilder = newErrorBuilder(message).withCode(errorType.getErrorType());
        if (details != null) {
            errorBuilder.addExtendedInfoList(extendedInfoListFromDetails(details));
        }

        if (redfishErrorResponse != null && redfishErrorResponse.getError() != null) {
            errorBuilder.addExtendedInfoList(extendedInfoListFromError(redfishErrorResponse.getError()));
        }

        return status(errorType.getErrorCode())
            .type(APPLICATION_JSON)
            .entity(new RedfishErrorResponseWrapper(errorBuilder.build()))
            .build();
    }

    private List<ExtendedInfo> extendedInfoListFromDetails(List<String> details) {
        return details.stream()
            .filter(detail -> trimToNull(detail) != null)
            .map(detail -> newExtendedInfoBuilder(detail).build())
            .collect(toList());
    }

    private List<ExtendedInfo> extendedInfoListFromError(Error error) {
        if (error == null) {
            return emptyList();
        }

        List<ExtendedInfo> errorDetails = new ArrayList<>();
        errorDetails.add(newExtendedInfoBuilder(error.getMessage())
            .withMessageId(error.getCode())
            .build());
        for (ExtendedInfo info : error.getExtendedInfo()) {
            errorDetails.add(newExtendedInfoBuilder(info.getMessage())
                .withMessageId(info.getMessageId())
                .withResolution(info.getResolution())
                .build());
        }
        return errorDetails;
    }

    WebApplicationException toWebApplicationException() {
        return new WebApplicationException(build());
    }
}
