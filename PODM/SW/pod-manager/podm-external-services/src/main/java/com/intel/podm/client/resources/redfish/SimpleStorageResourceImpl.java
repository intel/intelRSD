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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.resources.redfish.SimpleStorageDeviceResource;
import com.intel.podm.client.api.resources.redfish.SimpleStorageResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.Status;

import java.util.List;

@OdataTypes({
        "#SimpleStorage.1.0.0.SimpleStorage",
        "#SimpleStorage.1.1.0.SimpleStorage"
})
public class SimpleStorageResourceImpl extends ExternalServiceResourceImpl implements SimpleStorageResource {
    @JsonProperty("UEFIDevicePath")
    String uefiDevicePath;

    @JsonProperty("Devices")
    private List<SimpleStorageDeviceResourceImpl> devices;

    @JsonProperty("Status")
    private Status status;

    @Override
    public String getUefiDevicePath() {
        return uefiDevicePath;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public List<SimpleStorageDeviceResource> getDevices() {
        return (List) devices;
    }
}
