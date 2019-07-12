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

package com.intel.rsd.redfish;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Getter;
import lombok.ToString;
import org.springframework.http.HttpStatus;

import java.util.Collections;
import java.util.List;
import java.util.stream.Stream;

import static java.util.stream.Collectors.toList;

@Getter
@ToString
public final class RedfishError {
    @JsonProperty("error")
    private Error error;

    @JsonIgnore
    private HttpStatus httpStatus;

    public RedfishError(HttpStatus httpStatus, RedfishErrorCode redfishErrorCode, String errorMessage) {
        this.error = new Error(redfishErrorCode, errorMessage);
        this.httpStatus = httpStatus;
    }

    public RedfishError(HttpStatus httpStatus, RedfishErrorCode redfishErrorCode, String errorMessage, String... extendedInfo) {
        this.error = new Error(redfishErrorCode, errorMessage, extendedInfo);
        this.httpStatus = httpStatus;
    }

    @Getter
    @ToString
    public static class Error {
        @JsonProperty("code")
        private RedfishErrorCode redfishErrorCode;
        @JsonProperty("message")
        private String message;
        @JsonProperty("@Message.ExtendedInfo")
        private List<ExtendedInfo> extendedInfo = Collections.emptyList();

        Error(RedfishErrorCode redfishErrorCode, String errorMessage) {
            this.redfishErrorCode = redfishErrorCode;
            this.message = errorMessage;
        }

        Error(RedfishErrorCode redfishErrorCode, String errorMessage, String... extendedInfo) {
            this.redfishErrorCode = redfishErrorCode;
            this.message = errorMessage;
            this.extendedInfo = Stream.of(extendedInfo).map(ExtendedInfo::new).collect(toList());
        }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    @ToString
    public static final class ExtendedInfo {
        @JsonProperty("Message")
        private String message;

        @JsonProperty("Resolution")
        private String resolution;

        @JsonProperty("MessageId")
        private final String messageId = "Base.1.0.GeneralError";

        public ExtendedInfo(String message) {
            this.message = message;
        }
    }
}

