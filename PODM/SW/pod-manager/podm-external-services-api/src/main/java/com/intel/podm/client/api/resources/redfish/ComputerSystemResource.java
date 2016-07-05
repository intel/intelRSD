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

import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.actions.ResetType;

import java.util.List;
import java.util.UUID;

public interface ComputerSystemResource extends ExternalServiceResource {
    SystemType getSystemType();
    String getAssetTag();
    String getDescription();
    UUID getUuid();
    String getHostName();
    Status getStatus();
    IndicatorLed getIndicatorLed();
    PowerState getPowerState();
    BootObject getBoot();
    String getBiosVersion();

    Integer getProcessorsCount();
    Status getProcessorsStatus();

    Integer getTotalSystemMemoryGiB();
    Status getMemoryStatus();

    Integer getMemorySockets();

    Integer getProcessorsSockets();

    boolean isBasic();
    List<ResetType> getAllowableResetTypes();

    Iterable<ResourceSupplier> getProcessors() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getEthernetInterfaces() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getAdapters() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getMemoryChunks() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getMemoryModules() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getSimpleStorages() throws ExternalServiceApiReaderException;
}
