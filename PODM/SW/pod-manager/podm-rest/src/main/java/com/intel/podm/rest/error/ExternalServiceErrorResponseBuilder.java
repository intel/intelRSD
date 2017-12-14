/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.net.HttpStatusCode;
import com.intel.podm.common.types.redfish.ExternalServiceError;
import com.intel.podm.rest.representation.json.exceptionmappers.LogMessageBuilder;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.ws.rs.core.Response;
import java.util.Optional;

import static com.intel.podm.rest.error.ErrorResponseBuilder.newErrorResponseBuilder;
import static com.intel.podm.rest.error.ExternalServiceErrorMessageBuilder.newExternalServiceErrorMessageBuilder;
import static com.intel.podm.rest.representation.json.exceptionmappers.LogMessageBuilder.newLogMessageBuilder;
import static com.intel.podm.common.enterprise.utils.exceptions.RootCauseInvestigator.tryGetExternalServiceErrorInExceptionStack;
import static java.util.Collections.singletonList;
import static java.util.Optional.empty;

@Dependent
public class ExternalServiceErrorResponseBuilder {
    @Inject
    private Logger logger;

    public Optional<Response> getExternalServiceErrorResponse(BusinessApiException exception) {
        Optional<ExternalServiceError> externalServiceErrorResponse = tryGetExternalServiceErrorInExceptionStack(exception);
        if (!externalServiceErrorResponse.isPresent()) {
            return empty();
        }
        HttpStatusCode httpStatusCode = externalServiceErrorResponse.get().getResponse().getHttpStatusCode();
        return getErrorResponseByHttpStatusCode(httpStatusCode, exception);
    }

    private Optional<Response> getErrorResponseByHttpStatusCode(HttpStatusCode httpStatusCode, BusinessApiException exception) {
        if (httpStatusCode.isClientError()) {
            return Optional.of(buildErrorResponse(ErrorType.INVALID_PAYLOAD, exception));
        }
        if (httpStatusCode.equals(HttpStatusCode.NOT_IMPLEMENTED)) {
            return Optional.of(buildErrorResponse(ErrorType.NOT_IMPLEMENTED, exception));
        }
        return empty();
    }

    private Response buildErrorResponse(ErrorType errorType, BusinessApiException businessApiException) {
        LogMessageBuilder logMessageBuilder = newLogMessageBuilder(errorType.getErrorType()).withMessage(errorType.getMessage());
        ErrorResponseBuilder errorResponseBuilder = newErrorResponseBuilder(errorType).withDetails(singletonList(businessApiException.getMessage()));
        tryGetExternalServiceErrorInExceptionStack(businessApiException).ifPresent(externalServiceError -> {
            logMessageBuilder.withExternalServiceError(externalServiceError);
            errorResponseBuilder.withErrorResponse(newExternalServiceErrorMessageBuilder(externalServiceError).build());
        });
        logger.e(logMessageBuilder.build());
        return errorResponseBuilder.build();
    }
}
