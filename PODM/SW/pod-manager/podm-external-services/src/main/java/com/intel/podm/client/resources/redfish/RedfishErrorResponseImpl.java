/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.redfish.RedfishErrorResponse;

import java.util.ArrayList;
import java.util.List;

import static com.google.common.base.MoreObjects.toStringHelper;

@JsonIgnoreProperties(ignoreUnknown = true)
public class RedfishErrorResponseImpl implements RedfishErrorResponse {
    @JsonProperty("error")
    private ErrorImpl error;

    @Override
    public ErrorImpl getError() {
        return error;
    }

    @Override
    public String toString() {
        return toStringHelper(this)
                .add("error", error)
                .toString();
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static final class ErrorImpl implements RedfishErrorResponse.Error {
        @JsonProperty("code")
        private String code;
        @JsonProperty("message")
        private String message;
        @JsonProperty("@Message.ExtendedInfo")
        private final List<ExtendedInfoImpl> extendedInfo = new ArrayList<>();

        @Override
        public String getCode() {
            return code;
        }

        @Override
        public String getMessage() {
            return message;
        }

        @Override
        public List<ExtendedInfoImpl> getExtendedInfo() {
            return extendedInfo;
        }

        @Override
        public String toString() {
            return toStringHelper(this)
                    .add("code", code)
                    .add("message", message)
                    .add("extendedInfo", extendedInfo)
                    .toString();
        }

        @JsonIgnoreProperties(ignoreUnknown = true)
        public static final class ExtendedInfoImpl implements ExtendedInfo {
            @JsonProperty("Message")
            private String message;

            @Override
            public String getMessageId() {
                return "Base.1.0.ExternalServiceError";
            }

            @Override
            public String getMessage() {
                return message;
            }

            @Override
            public String toString() {
                return toStringHelper(this)
                        .add("message", message)
                        .toString();
            }
        }
    }
}
