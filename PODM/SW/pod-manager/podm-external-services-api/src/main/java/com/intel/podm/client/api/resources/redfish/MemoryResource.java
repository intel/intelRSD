/*
 * Copyright (c) 2016-2017 Intel Corporation
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
import com.intel.podm.common.types.BaseModuleType;
import com.intel.podm.common.types.ErrorCorrection;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.MemoryMedia;
import com.intel.podm.common.types.MemoryType;
import com.intel.podm.common.types.OperatingMemoryMode;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;
import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount"})
public interface MemoryResource extends ExternalServiceResource {
    Ref<MemoryDeviceType> getMemoryDeviceType();
    Ref<BaseModuleType> getBaseModuleType();
    Ref<Integer> getDataWidthBits();
    Ref<Integer> getBusWidthBits();
    Ref<String> getManufacturer();
    Ref<String> getSerialNumber();
    Ref<String> getPartNumber();
    Ref<List<Integer>> getAllowedSpeedsMhz();
    Ref<String> getDeviceLocator();
    Ref<Integer> getRankCount();
    Ref<ErrorCorrection> getErrorCorrection();
    Ref<BigDecimal> getVoltageVolt();

    MemoryLocationObject getMemoryLocation();
    Ref<MemoryType> getMemoryType();
    Ref<List<MemoryMedia>> getMemoryMedia();
    Ref<Integer> getCapacityMib();
    Ref<Status> getStatus();
    Ref<Integer> getOperatingSpeedMhz();
    Ref<List<MemoryRegionObject>> getRegions();
    Ref<String> getFirmwareRevision();
    Ref<String> getFirmwareApiVersion();
    Ref<List<String>> getFunctionClasses();
    Ref<String> getVendorId();
    Ref<String> getDeviceId();
    Ref<List<OperatingMemoryMode>> getOperatingMemoryModes();

    ResourceSupplier getComputerSystem();
}
