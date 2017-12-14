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
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OdataTypeVersions;

@OdataTypes({
    "#NetworkDeviceFunction" + OdataTypeVersions.VERSION_PATTERN + "NetworkDeviceFunction"
})
public class NetworkDeviceFunctionResource extends ExternalServiceResourceImpl implements ExternalServiceResource {

    @JsonProperty("Status")
    private Status status;

    @JsonProperty("DeviceEnabled")
    private Boolean deviceEnabled;

    @JsonProperty("Ethernet")
    private EthernetObject ethernet;

    @JsonProperty("iSCSIBoot")
    private IscsiBootObject iscsiBoot;

    public Status getStatus() {
        return status;
    }

    public Boolean getDeviceEnabled() {
        return deviceEnabled;
    }

    public EthernetObject getEthernet() {
        return ethernet;
    }

    public IscsiBootObject getIscsiBoot() {
        return iscsiBoot;
    }
}
