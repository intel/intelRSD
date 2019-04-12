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

package com.intel.rsd.nodecomposer.business.errors;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.types.redfish.RedfishErrorResponse.Error.ExtendedInfo;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonInclude(NON_NULL)
public final class ExtendedInfoDto implements ExtendedInfo {
    private static final String GENERAL_ERROR_MESSAGE_ID = "Base.1.0.GeneralError";

    @JsonProperty("MessageId")
    private String messageId;

    @JsonProperty("Message")
    private String message;

    @JsonProperty("Resolution")
    private String resolution;

    public ExtendedInfoDto(String messageId, String message, String resolution) {
        this.messageId = messageId != null ? messageId : GENERAL_ERROR_MESSAGE_ID;
        this.message = message;
        this.resolution = resolution;
    }

    @Override
    public String getMessageId() {
        return messageId;
    }

    @Override
    public String getMessage() {
        return message;
    }

    @Override
    public String getResolution() {
        return resolution;
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
            return new ExtendedInfoDto(messageId, message, resolution);
        }
    }
}
