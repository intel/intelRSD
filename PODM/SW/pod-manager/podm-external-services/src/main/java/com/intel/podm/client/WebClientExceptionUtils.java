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

package com.intel.podm.client;

import com.intel.podm.client.redfish.response.RedfishClientException;
import com.intel.podm.client.redfish.response.UnexpectedRedirectionException;

import java.util.Optional;

import static java.util.Optional.empty;
import static java.util.Optional.of;
import static org.apache.commons.lang3.exception.ExceptionUtils.getThrowableList;

public final class WebClientExceptionUtils {
    private WebClientExceptionUtils() {
    }

    public static boolean isConnectionExceptionTheRootCause(Throwable e) {
        Optional<RedfishClientException> redfishRootCause = getRedfishExceptionRootCause(e);
        return redfishRootCause.isPresent()
            && redfishRootCause.get().getCause() instanceof WebClientConnectionException;
    }

    @SuppressWarnings({"unchecked"})
    private static Optional<RedfishClientException> getRedfishExceptionRootCause(Throwable e) {
        return getThrowableList(e).stream()
            .filter(throwable -> throwable instanceof RedfishClientException)
            .map(redfishException -> (RedfishClientException) redfishException)
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
