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

package com.intel.podm.business.dto.redfish.attributes;

public final class ExtendedInfoDto {
    private final String messageId;
    private final String message;

    private ExtendedInfoDto(Builder builder) {
        this.messageId = builder.messageId;
        this.message = builder.message;
    }

    public static Builder newExtendedInfoDto() {
        return new Builder();
    }

    public String getMessageId() {
        return messageId;
    }

    public String getMessage() {
        return message;
    }

    public static final class Builder {
        private String messageId;
        private String message;

        private Builder() {
        }

        public ExtendedInfoDto build() {
            return new ExtendedInfoDto(this);
        }

        public Builder messageId(String messageId) {
            this.messageId = messageId;
            return this;
        }

        public Builder message(String message) {
            this.message = message;
            return this;
        }
    }
}
