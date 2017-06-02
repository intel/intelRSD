/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.client.api.RedfishApiException;

import java.net.ConnectException;
import java.util.Objects;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static org.apache.commons.lang.exception.ExceptionUtils.getRootCause;

@SuppressWarnings({"checkstyle:IllegalCatch"})
final class RetryInvoker {
    private static final int MAX_RETRY_ATTEMPTS = 3;
    private static final long DEFAULT_INTERVAL_MS = 100;

    protected RetryInvoker() {
    }

    public static <T extends RedfishApiException> void retry(RunnableWithRedfishException<T> runnable) throws T {
        retry(() -> {
            runnable.run();
            return null;
        });
    }

    public static <T, U extends RedfishApiException> T retry(SupplierWithRedfishException<T, U> supplier) throws U {
        Exception lastException;
        int currentAttempt = 1;

        do {
            try {
                return supplier.get();
            } catch (Exception e) {
                if (isNotConnectionException(e) || maximumAttemptsReached(currentAttempt)) {
                    throw e;
                }

                sleep(currentAttempt);
                lastException = e;
                currentAttempt++;
            }
        } while (currentAttempt <= MAX_RETRY_ATTEMPTS);

        throw new IllegalStateException(lastException);
    }

    private static boolean isNotConnectionException(Throwable e) {
        return !(getRootCause(e) instanceof ConnectException);
    }

    private static boolean maximumAttemptsReached(Integer currentAttemptNumber) {
        requiresNonNull(currentAttemptNumber, "currentAttemptNumber");
        return Objects.equals(MAX_RETRY_ATTEMPTS, currentAttemptNumber);
    }

    private static void sleep(int currentAttempt) {
        try {
            Thread.sleep(currentAttempt * DEFAULT_INTERVAL_MS);
        } catch (InterruptedException e) {
        }
    }

    @FunctionalInterface
    interface RunnableWithRedfishException<T extends RedfishApiException> {
        void run() throws T;
    }

    @FunctionalInterface
    interface SupplierWithRedfishException<T, U extends RedfishApiException> {
        T get() throws U;
    }
}
