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
import com.intel.podm.client.api.resources.redfish.EthernetObject;
import com.intel.podm.client.api.resources.redfish.IscsiBootObject;
import com.intel.podm.client.api.resources.redfish.NetworkDeviceFunctionResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.Status;

@OdataTypes({
    "#NetworkDeviceFunction" + OdataTypes.VERSION_PATTERN + "NetworkDeviceFunction"
})
public class NetworkDeviceFunctionResourceImpl extends ExternalServiceResourceImpl implements NetworkDeviceFunctionResource {

    @JsonProperty("Status")
    private Status status;

    @JsonProperty("DeviceEnabled")
    private Boolean deviceEnabled;

    @JsonProperty("Ethernet")
    private EthernetObjectImpl ethernet;

    @JsonProperty("iSCSIBoot")
    private IscsiBootObjectImpl iscsiBoot;

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public Boolean getDeviceEnabled() {
        return deviceEnabled;
    }

    @Override
    public EthernetObject getEthernet() {
        return ethernet;
    }

    public IscsiBootObject getIscsiBoot() {
        return iscsiBoot;
    }
}
