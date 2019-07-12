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

package com.intel.rsd.redfish;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.ToString;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonIgnoreProperties(ignoreUnknown = true)
@JsonInclude(NON_NULL)
@ToString
public class ExtendedInfo {

    @JsonProperty("MessageId")
    private String messageId;

    @JsonProperty("Message")
    private String message;

    @JsonProperty("Resolution")
    private String resolution;

    public ExtendedInfo(String message) {
        this.message = message;
        this.messageId = "Base.1.0.GeneralError";
    }

    public ExtendedInfo(String messageId, String message, String resolution) {
        this.messageId = messageId;
        this.message = message;
        this.resolution = resolution;
    }

    public static final class ExtendedInfoBuilder {
        private final String message;
        private String messageId;
        private String resolution;

        private ExtendedInfoBuilder(String message) {
            this.message = message;
        }

        public static ExtendedInfoBuilder newExtendedInfoBuilder(String message) {
            return new ExtendedInfoBuilder(message);
        }

        public ExtendedInfoBuilder withMessageId(String messageId) {
            this.messageId = messageId;
            return this;
        }

        public ExtendedInfoBuilder withResolution(String resolution) {
            this.resolution = resolution;
            return this;
        }

        public ExtendedInfo build() {
            return new ExtendedInfo(messageId, message, resolution);
        }
    }
}
