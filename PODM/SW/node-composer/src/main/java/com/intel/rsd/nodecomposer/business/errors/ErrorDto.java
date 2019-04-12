/*
 * Copyright (c) 2015-2019 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.types.redfish.RedfishErrorResponse.Error;

import java.util.ArrayList;
import java.util.List;

public final class ErrorDto implements Error {
    @JsonProperty("code")
    private final String code;
    @JsonProperty("message")
    private final String message;
    @JsonProperty("@Message.ExtendedInfo")
    private final List<ExtendedInfo> extendedInfo;

    public ErrorDto(String errorType, String errorMessage, List<ExtendedInfo> extendedInfo) {
        this.code = errorType;
        this.message = errorMessage;
        this.extendedInfo = extendedInfo;
    }

    @Override
    public String getCode() {
        return code;
    }

    @Override
    public String getMessage() {
        return message;
    }

    @Override
    public List<ExtendedInfo> getExtendedInfo() {
        return extendedInfo;
    }

    public static final class ErrorBuilder {
        private final String errorMessage;
        private String errorCode;
        private List<ExtendedInfo> extendedInfos = new ArrayList<>();

        private ErrorBuilder(String errorMessage) {
            this.errorMessage = errorMessage;
        }

        public static ErrorBuilder newErrorBuilder(String errorMessage) {
            return new ErrorBuilder(errorMessage);
        }

        public ErrorBuilder withCode(String errorCode) {
            this.errorCode = errorCode;
            return this;
        }

        public ErrorBuilder addExtendedInfo(ExtendedInfo extendedInfo) {
            this.extendedInfos.add(extendedInfo);
            return this;
        }

        public ErrorBuilder addExtendedInfoList(List<ExtendedInfo> extendedInfoList) {
            this.extendedInfos.addAll(extendedInfoList);
            return this;
        }

        public Error build() {
            return new ErrorDto(errorCode, errorMessage, extendedInfos);
        }
    }
}
