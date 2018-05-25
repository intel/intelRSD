/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#SimpleStorage" + VERSION_PATTERN + "SimpleStorage"
})
public class SimpleStorageResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("UEFIDevicePath")
    private String uefiDevicePath;

    @JsonProperty("Devices")
    private List<SimpleStorageDeviceResource> devices = new ArrayList<>();

    @JsonProperty("Status")
    private Status status;

    public String getUefiDevicePath() {
        return uefiDevicePath;
    }

    public Status getStatus() {
        return status;
    }

    public List<SimpleStorageDeviceResource> getDevices() {
        return devices;
    }
}
