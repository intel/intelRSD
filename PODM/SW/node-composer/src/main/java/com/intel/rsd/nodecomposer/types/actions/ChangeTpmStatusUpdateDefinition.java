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

package com.intel.rsd.nodecomposer.types.actions;

import com.intel.rsd.nodecomposer.types.InterfaceType;

import java.io.Serializable;

public class ChangeTpmStatusUpdateDefinition implements Serializable {
    private static final long serialVersionUID = -6943114612046856598L;

    private Boolean deviceEnabled;
    private InterfaceType interfaceType;
    private Boolean clearOwnership;

    public ChangeTpmStatusUpdateDefinition(Boolean clearOwnership) {
        this.clearOwnership = clearOwnership;
    }

    public ChangeTpmStatusUpdateDefinition(Boolean deviceEnabled, InterfaceType interfaceType, Boolean clearOwnership) {
        this.deviceEnabled = deviceEnabled;
        this.interfaceType = interfaceType;
        this.clearOwnership = clearOwnership;
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
