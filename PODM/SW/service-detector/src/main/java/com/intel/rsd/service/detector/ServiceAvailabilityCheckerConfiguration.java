/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.service.detector;

import lombok.Data;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.context.annotation.Configuration;

import java.util.StringJoiner;

@Data
@Configuration
@ConfigurationProperties("service-availability")
public class ServiceAvailabilityCheckerConfiguration {
    private Integer recheckIntervalSeconds = 30;
    private ThreadPoolExecutorConfig threadPoolExecutor;

    @Data
    public static class ThreadPoolExecutorConfig {
        private Integer minThreads = 4;
        private Integer maxThreads = 8;
        private Integer keepAliveTimerSeconds = 30;
        private Integer queueCapacity = 1000;

        @Override
        public String toString() {
            return new StringJoiner(", ", ThreadPoolExecutorConfig.class.getSimpleName() + "[", "]")
                .add("minThreads=" + minThreads)
                .add("maxThreads=" + maxThreads)
                .add("keepAliveTimerSeconds=" + keepAliveTimerSeconds)
                .add("queueCapacity=" + queueCapacity)
                .toString();
        }
    }
}
