/*
 * Copyright (c) 2017 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.InterfaceType;
import com.intel.podm.common.types.InterfaceTypeSelection;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.annotations.AsUnassigned;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;

import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

class TrustedModuleObject implements RedfishComputerSystem.TrustedModule {
    @JsonProperty("FirmwareVersion")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> firmwareVersion = unassigned();

    @JsonProperty("FirmwareVersion2")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> firmwareVersion2 = unassigned();

    @JsonProperty("InterfaceType")
    @AsUnassigned(WHEN_NULL)
    private Ref<InterfaceType> interfaceType = unassigned();

    @JsonProperty("InterfaceTypeSelection")
    @AsUnassigned(WHEN_NULL)
    private Ref<InterfaceTypeSelection> interfaceTypeSelection = unassigned();

    @JsonProperty("Status")
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status = unassigned();

    @Override
    public Ref<String> getFirmwareVersion() {
        return firmwareVersion;
    }

    @Override
    public Ref<String> getFirmwareVersion2() {
        return firmwareVersion2;
    }

    @Override
    public Ref<InterfaceType> getInterfaceType() {
        return interfaceType;
    }

    @Override
    public Ref<InterfaceTypeSelection> getInterfaceTypeSelection() {
        return interfaceTypeSelection;
    }

    @Override
    public Ref<Status> getStatus() {
        return status;
    }
}
