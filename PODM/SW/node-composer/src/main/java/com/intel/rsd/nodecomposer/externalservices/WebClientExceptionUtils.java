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

package com.intel.rsd.nodecomposer.externalservices;

import com.intel.rsd.nodecomposer.externalservices.redfish.response.RedfishClientException;
import com.intel.rsd.nodecomposer.externalservices.redfish.response.UnexpectedRedirectionException;
import com.intel.rsd.nodecomposer.types.net.HttpStatusCode;
import com.intel.rsd.nodecomposer.types.redfish.ExternalServiceError;

import java.util.Objects;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.types.net.HttpStatusCode.BAD_REQUEST;
import static com.intel.rsd.nodecomposer.types.net.HttpStatusCode.NOT_FOUND;
import static com.intel.rsd.nodecomposer.types.net.HttpStatusCode.SERVICE_UNAVAILABLE;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static org.apache.commons.lang3.exception.ExceptionUtils.getThrowableList;

public final class WebClientExceptionUtils {
    private WebClientExceptionUtils() {
    }

    public static boolean isResourceNotFoundTheRootCause(Throwable e) {
        return isRootCauseWithGivenStatus(e, NOT_FOUND);
    }

    public static boolean isServiceUnavailableTheRootCause(Throwable e) {
        return isRootCauseWithGivenStatus(e, SERVICE_UNAVAILABLE);
    }

    public static boolean badRequestInTheRootCause(Throwable e) {
        return isRootCauseWithGivenStatus(e, BAD_REQUEST);
    }

    public static boolean isConnectionExceptionTheRootCause(Throwable e) {
        Optional<RedfishClientException> redfishRootCause = getRedfishExceptionRootCause(e);
        return redfishRootCause.isPresent()
            && redfishRootCause.get().getCause() instanceof WebClientConnectionException;
    }

    private static boolean isRootCauseWithGivenStatus(Throwable e, HttpStatusCode statusCode) {
        Optional<RedfishClientException> redfishRootCause = getRedfishExceptionRootCause(e);

        if (!redfishRootCause.isPresent()) {
            return false;
        }

        ExternalServiceError externalServiceError = redfishRootCause.get().getExternalServiceError();
        return externalServiceError != null
            && Objects.equals(externalServiceError.getResponse().getHttpStatusCode(), statusCode);
    }

    @SuppressWarnings({"unchecked"})
    private static Optional<RedfishClientException> getRedfishExceptionRootCause(Throwable e) {
        return getThrowableList(e).stream()
            .filter(throwable -> throwable instanceof RedfishClientException)
            .map(throwable -> (RedfishClientException) throwable)
            .findFirst();
    }

    public static Optional<UnexpectedRedirectionException> tryGetUnderlyingRedirectionException(Throwable e) {
        Optional<RedfishClientException> redfishRootCause = getRedfishExceptionRootCause(e);
        if (redfishRootCause.isPresent()) {
            RedfishClientException redfishClientException = redfishRootCause.get();
            if (redfishClientException.getCause() instanceof UnexpectedRedirectionException) {
                return of((UnexpectedRedirectionException) redfishClientException.getCause());
            }
        }
        return empty();
    }
}
