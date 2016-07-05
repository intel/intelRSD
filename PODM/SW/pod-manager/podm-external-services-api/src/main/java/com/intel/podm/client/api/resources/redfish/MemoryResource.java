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

package com.intel.podm.client.api.resources.redfish;

import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.types.MemoryMedia;
import com.intel.podm.common.types.MemoryType;
import com.intel.podm.common.types.OperatingMemoryMode;
import com.intel.podm.common.types.Status;

import java.util.List;

public interface MemoryResource extends ExternalServiceResource {
    MemoryType getMemoryType();
    List<MemoryMedia> getMemoryMedia();
    Integer getCapacityMib();
    Status getStatus();
    Integer getOperatingSpeedMhz();
    Iterable<MemoryRegionObject> getRegions();
    String getFirmwareRevision();
    String getFirmwareApiVersion();
    List<String> getFunctionClasses();
    String getVendorId();
    String getDeviceId();
    String getDeviceLocator();
    List<OperatingMemoryMode> getOperatingMemoryModes();

    ResourceSupplier getComputerSystem();
}
