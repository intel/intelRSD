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

package com.intel.rsd.nodecomposer.rest.error;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.types.net.HttpStatusCode;
import com.intel.rsd.nodecomposer.types.redfish.ExternalServiceError;
import com.intel.rsd.nodecomposer.types.redfish.RedfishErrorResponse;
import com.intel.rsd.redfish.RedfishError;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Component;

import java.util.Optional;

import static com.intel.rsd.nodecomposer.utils.exceptions.RootCauseInvestigator.tryGetExternalServiceErrorInExceptionStack;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.ResponseEntity.status;

@Slf4j
@Component
class ExternalServiceErrorResponseBuilder {

    Optional<ResponseEntity<RedfishError>> getExternalServiceErrorResponse(BusinessApiException exception) {
        Optional<ExternalServiceError> externalServiceErrorResponse = tryGetExternalServiceErrorInExceptionStack(exception);
        if (!externalServiceErrorResponse.isPresent()) {
            return empty();
        }

        HttpStatusCode httpStatusCode = externalServiceErrorResponse.get().getResponse().getHttpStatusCode();
        return getErrorResponseByHttpStatusCode(httpStatusCode, exception);
    }

    private Optional<ResponseEntity<RedfishError>> getErrorResponseByHttpStatusCode(HttpStatusCode httpStatusCode, BusinessApiException exception) {
        if (httpStatusCode.isClientError()) {
            return of(buildErrorResponse(BAD_REQUEST, exception));
        }

        if (httpStatusCode.equals(HttpStatusCode.NOT_IMPLEMENTED)) {
            return of(buildErrorResponse(HttpStatus.NOT_IMPLEMENTED, exception));
        }

        return empty();
    }

    private ResponseEntity<RedfishError> buildErrorResponse(HttpStatus httpStatus, BusinessApiException businessApiException) {
        val externalServiceErrorMessage = tryGetExternalServiceErrorInExceptionStack(businessApiException)
            .map(ExternalServiceError::getRedfishErrorResponse)
            .map(RedfishErrorResponse::getError)
            .map(RedfishErrorResponse.Error::getMessage);

        val redfishError = externalServiceErrorMessage
            .map(message -> createRedfishError(httpStatus, businessApiException.getMessage(), message))
            .orElseGet(() -> createRedfishError(httpStatus, businessApiException.getMessage()));

        log.error("{}", redfishError);
        return status(httpStatus).body(redfishError);
    }
}
