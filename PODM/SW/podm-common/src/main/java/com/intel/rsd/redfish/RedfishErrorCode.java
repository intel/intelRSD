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

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonValue;

import java.util.stream.Stream;

public enum RedfishErrorCode {
    InvalidEndpoint("Base.1.0.InvalidEndpoint"),
    GeneralError("Base.1.0.GeneralError"),
    ServiceUnavailable("Base.1.0.ServiceUnavailable"),
    NotImplemented("Base.1.0.NotImplemented"),
    GatewayTimeout("Base.1.0.GatewayTimeout");

    private String errorCode;

    RedfishErrorCode(String errorCode) {
        this.errorCode = errorCode;
    }

    public String asString() {
        return errorCode;
    }

    @JsonCreator
    public static RedfishErrorCode forValue(String redfishErrorCodeAsString) throws IllegalAccessException {
        return Stream.of(values()).filter(it -> it.asString().equals(redfishErrorCodeAsString)).findAny().orElseThrow(IllegalAccessException::new);
    }

    @JsonValue
    public String toValue() {
        return errorCode;
    }
}
