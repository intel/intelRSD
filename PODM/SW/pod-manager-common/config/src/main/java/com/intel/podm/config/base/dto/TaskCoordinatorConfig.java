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

package com.intel.podm.config.base.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.intel.podm.config.base.ConfigFile;

import java.io.IOException;
import java.time.Duration;

import static java.time.Duration.ofSeconds;

@ConfigFile(filename = "task-synchronization.json")
public class TaskCoordinatorConfig extends BaseConfig {


    @JsonDeserialize(using = DurationDeserializer.class)
    @JsonProperty("MaxTimeToWaitForAsyncTaskSeconds")
    private Duration maxTimeToWaitForAsyncTaskSeconds = ofSeconds(60);

    public Duration getMaxTimeToWaitForAsyncTaskSeconds() {
        return maxTimeToWaitForAsyncTaskSeconds;
    }

    public static class DurationDeserializer extends JsonDeserializer<Duration> {
        @Override
        public Duration deserialize(JsonParser p, DeserializationContext ctxt) throws IOException {
            return ofSeconds(p.getLongValue());
        }
    }
}
