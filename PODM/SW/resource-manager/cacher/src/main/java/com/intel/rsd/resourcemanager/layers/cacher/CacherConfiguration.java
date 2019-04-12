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

package com.intel.rsd.resourcemanager.layers.cacher;

import lombok.Data;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.context.annotation.Configuration;

import java.time.Duration;

import static java.time.Duration.ofDays;

@Configuration
@ConfigurationProperties("cacher")
@Data
public class CacherConfiguration {
    private Duration entriesTimeToLive = ofDays(1);
    private int maxHeapSizeInMb = 25;

    public int getEntriesTimeToLiveSeconds() {
        try {
            return Math.toIntExact(entriesTimeToLive.getSeconds());
        } catch (ArithmeticException e) {
            throw new RuntimeException("cacher.entriesTimeToLive property value in seconds have to be in 32 bit integer range");
        }
    }
}
