/*
 * Copyright (c) 2018-2019 Intel Corporation
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

import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

@Component
public class RetryInvokerConfig {

    private static int maxRetryAttempts = 3;
    private static long intervalMs = 1000;

    public RetryInvokerConfig(@Value("${retryInvoker.maxRetryAttempts:3}") int maxRetryAttempts, @Value("${retryInvoker.intervalMs:1000}") int intervalMs) {
        RetryInvokerConfig.maxRetryAttempts = maxRetryAttempts;
        RetryInvokerConfig.intervalMs = intervalMs;
    }

    static int getMaxRetryAttempts() {
        return maxRetryAttempts;
    }

    static long getIntervalMs() {
        return intervalMs;
    }
}
