/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import java.util.Objects;

import static com.intel.podm.client.WebClientExceptionUtils.isConnectionExceptionTheRootCause;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;

@SuppressWarnings({"checkstyle:IllegalCatch"})
final class RetryInvoker {
    private static final int MAX_RETRY_ATTEMPTS = 3;
    private static final long DEFAULT_INTERVAL_MS = 100;

    protected RetryInvoker() {
    }

    static <T extends WebClientRequestException> void retry(RunnableWithRedfishException<T> runnable) throws T {
        retry(() -> {
            runnable.run();
            return null;
        });
    }

    static <T, U extends WebClientRequestException> T retry(SupplierWithRedfishException<T, U> supplier) throws U {
        Exception lastException;
        int currentAttempt = 1;

        do {
            try {
                return supplier.get();
            } catch (Exception e) {
                if (!isConnectionExceptionTheRootCause(e) || maximumAttemptsReached(currentAttempt)) {
                    throw e;
                }

                sleep(currentAttempt);
                lastException = e;
                currentAttempt++;
            }
        } while (currentAttempt <= MAX_RETRY_ATTEMPTS);

        throw new IllegalStateException(lastException);
    }

    private static boolean maximumAttemptsReached(Integer currentAttemptNumber) {
        requiresNonNull(currentAttemptNumber, "currentAttemptNumber");
        return Objects.equals(MAX_RETRY_ATTEMPTS, currentAttemptNumber);
    }

    private static void sleep(int currentAttempt) {
        try {
            Thread.sleep(currentAttempt * DEFAULT_INTERVAL_MS);
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
