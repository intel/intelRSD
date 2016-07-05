/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.redfish.json.templates.assembly;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.business.dto.redfish.RequestedEthernetInterface;
import com.intel.podm.business.dto.redfish.RequestedLocalDrive;
import com.intel.podm.business.dto.redfish.RequestedMemory;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.dto.redfish.RequestedProcessor;
import com.intel.podm.business.dto.redfish.RequestedRemoteDrive;

import java.util.List;

import static java.util.Collections.emptyList;
import static java.util.Optional.ofNullable;

public final class RequestedNodeJson implements RequestedNode {
    @JsonProperty
    private String name = "Composed Node";

    @JsonProperty
    private String description;

    @JsonProperty
    private List<RequestedProcessorImpl> processors;

    @JsonProperty
    private List<RequestedMemoryImpl> memory;

    @JsonProperty
    private List<RequestedRemoteDriveImpl> remoteDrives;

    @JsonProperty
    private List<RequestedLocalDriveImpl> localDrives;

    @JsonProperty
    private List<RequestedEthernetInterfaceImpl> ethernetInterfaces;

    @JsonProperty
    private Object oem;

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public List<RequestedProcessor> getProcessors() {
        return ofNullable((List) processors).orElse(emptyList());
    }

    @Override
    public List<RequestedMemory> getMemoryModules() {
        return ofNullable((List) memory).orElse(emptyList());
    }

    @Override
    public List<RequestedRemoteDrive> getRemoteDrives() {
        return ofNullable((List) remoteDrives).orElse(emptyList());
    }

    @Override
    public List<RequestedLocalDrive> getLocalDrives() {
        return ofNullable((List) localDrives).orElse(emptyList());
    }

    @Override
    public List<RequestedEthernetInterface> getEthernetInterfaces() {
        return ofNullable((List) ethernetInterfaces).orElse(emptyList());
    }

    @Override
    public Object getOem() {
        return oem;
    }
}
