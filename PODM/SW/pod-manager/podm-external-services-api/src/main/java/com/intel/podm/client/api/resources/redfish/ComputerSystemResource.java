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

import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.actions.ResetType;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;

import java.math.BigDecimal;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

@SuppressWarnings({"checkstyle:MethodCount"})
public interface ComputerSystemResource extends ExternalServiceResource {
    Ref<SystemType> getSystemType();
    Ref<String> getAssetTag();
    Ref<UUID> getUuid();
    Ref<String> getHostName();
    Ref<Status> getStatus();
    Ref<IndicatorLed> getIndicatorLed();
    Ref<PowerState> getPowerState();
    Ref<String> getBiosVersion();
    Ref<String> getManufacturer();
    Ref<String> getModel();
    Ref<String> getSku();
    Ref<String> getSerialNumber();
    Ref<String> getPartNumber();

    /* Changed name due to problem with ModelMapper library and improper Boot mapping */
    ComputerSystemResource.Boot getBootObject();
    Ref<String> getProcessorModel();
    Ref<LinkedHashSet<RedfishComputerSystem.Device>> getPciDevices();
    Ref<Integer> getProcessorsCount();
    Ref<Status> getProcessorStatus();
    Ref<BigDecimal> getTotalSystemMemoryGiB();
    Ref<Status> getMemoryStatus();
    Ref<Integer> getMemorySockets();
    Ref<List<String>> getPcieConnectionId();
    Ref<Integer> getProcessorSockets();

    boolean isBasic();
    Ref<LinkedHashSet<ResetType>> getAllowableResetTypes();

    Iterable<ResourceSupplier> getProcessors() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getEthernetInterfaces() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getMemoryModules() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getSimpleStorages() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getStorages() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getEndpoints() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getAdapters() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getPcieDevices() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getPcieFunctions() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getNetworkInterfaces() throws ExternalServiceApiReaderException;

    interface Boot {
        Ref<BootSourceState> getBootSourceOverrideEnabled();

        Ref<BootSourceType> getBootSourceOverrideTarget();

        Set<BootSourceType> getBootSourceOverrideTargetAllowableValues();

        Ref<BootSourceMode> getBootSourceOverrideMode();

        Set<BootSourceMode> getBootSourceOverrideModeAllowableValues();
    }
}
