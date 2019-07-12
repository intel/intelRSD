/*
 * Copyright (c) 2016-2019 Intel Corporation
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

import lombok.extern.slf4j.Slf4j;

import java.util.Objects;

import static com.intel.rsd.nodecomposer.externalservices.WebClientExceptionUtils.isConnectionExceptionTheRootCause;
import static com.intel.rsd.nodecomposer.externalservices.WebClientExceptionUtils.isServiceUnavailableTheRootCause;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;

@Slf4j
final class RetryInvoker {

    protected RetryInvoker() {
    }

    static <T extends WebClientRequestException> void retry(RunnableWithRedfishException<T> runnable) throws T {
        retry(() -> {
            runnable.run();
            return null;
        });
    }

    @SuppressWarnings({"checkstyle:IllegalCatch"})
    static <T, U extends WebClientRequestException> T retry(SupplierWithRedfishException<T, U> supplier) throws U {
        Exception lastException;
        int currentAttempt = 1;

        do {
            try {
                return supplier.get();
            } catch (Exception e) {
                if ((!isServiceUnavailableTheRootCause(e) && !isConnectionExceptionTheRootCause(e)) || maximumAttemptsReached(currentAttempt)) {
                    throw e;
                }
                log.info("Retrying (service unavailable: {}, connection exception: {}, maximumAttemptsReached: {}) operation due to: {}",
                    isServiceUnavailableTheRootCause(e), isConnectionExceptionTheRootCause(e), maximumAttemptsReached(currentAttempt), e.getMessage());

                sleep(currentAttempt);
                lastException = e;
                currentAttempt++;
            }
        } while (currentAttempt <= RetryInvokerConfig.getMaxRetryAttempts());

        throw new IllegalStateException(lastException);
    }

    private static boolean maximumAttemptsReached(Integer currentAttemptNumber) {
        requiresNonNull(currentAttemptNumber, "currentAttemptNumber");
        return Objects.equals(RetryInvokerConfig.getMaxRetryAttempts(), currentAttemptNumber);
    }

    private static void sleep(int currentAttempt) {
        try {
            Thread.sleep(currentAttempt * RetryInvokerConfig.getIntervalMs());
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    @FunctionalInterface
    interface RunnableWithRedfishException<T extends WebClientRequestException> {
        void run() throws T;
    }

    @FunctionalInterface
    interface SupplierWithRedfishException<T, U extends WebClientRequestException> {
        T get() throws U;
    }
}
