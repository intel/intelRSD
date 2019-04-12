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

package com.intel.rsd.nodecomposer.externalservices.actions;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.types.InterfaceType;
import com.intel.rsd.nodecomposer.types.actions.ChangeTpmStatusUpdateDefinition;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

public class ChangeTpmStateRequest {
    @JsonProperty("DeviceEnabled")
    private Boolean deviceEnabled;

    @JsonProperty("InterfaceType")
    @JsonInclude(NON_NULL)
    private InterfaceType interfaceType;

    @JsonProperty("ClearOwnership")
    @JsonInclude(NON_NULL)
    private Boolean clearOwnership;

    public ChangeTpmStateRequest(ChangeTpmStatusUpdateDefinition changeTpmStatusUpdateDefinition) {
        this.deviceEnabled = changeTpmStatusUpdateDefinition.getDeviceEnabled();
        this.interfaceType = changeTpmStatusUpdateDefinition.getInterfaceType();
        this.clearOwnership = changeTpmStatusUpdateDefinition.getClearOwnership();
    }

    public Boolean getDeviceEnabled() {
        return deviceEnabled;
    }

    public InterfaceType getInterfaceType() {
        return interfaceType;
    }

    public Boolean getClearOwnership() {
        return clearOwnership;
    }
}
