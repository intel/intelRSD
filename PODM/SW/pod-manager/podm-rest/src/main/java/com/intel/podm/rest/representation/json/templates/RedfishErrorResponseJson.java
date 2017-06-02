/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.rest.representation.json.templates;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;

import java.util.ArrayList;
import java.util.List;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static java.util.Collections.singletonList;

public final class RedfishErrorResponseJson {
    @JsonProperty("code")
    private final String code;
    @JsonProperty("message")
    private final String message;
    @JsonProperty("@Message.ExtendedInfo")
    private final List<ExtendedInfoJson> extendedInfo;

    public RedfishErrorResponseJson(String errorType, String errorMessage) {
        this.code = errorType;
        this.message = errorMessage;
        this.extendedInfo = new ArrayList<>();
    }

    public RedfishErrorResponseJson(String errorType, String errorMessage, List<ExtendedInfoJson> extendedInfo) {
        this.code = errorType;
        this.message = errorMessage;
        this.extendedInfo = extendedInfo;
    }

    public static List<ExtendedInfoJson> createExtendedInfoJsonList(String extendedInfoMessage) {
        return singletonList(new ExtendedInfoJson(null, extendedInfoMessage));
    }

    @JsonInclude(NON_NULL)
    public static final class ExtendedInfoJson {
        @JsonProperty("MessageId")
        private String messageId;

        @JsonProperty("Message")
        private String message;

        public ExtendedInfoJson(String messageId, String message) {
            this.messageId = messageId;
            this.message = message;
        }
    }
}
