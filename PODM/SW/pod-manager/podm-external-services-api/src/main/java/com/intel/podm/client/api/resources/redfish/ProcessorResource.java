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
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorArchitecture;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;

import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount"})
public interface ProcessorResource extends ExternalServiceResource {
    Ref<String> getSocket();
    Ref<ProcessorType> getProcessorType();
    Ref<ProcessorArchitecture> getProcessorArchitecture();
    Ref<InstructionSet> getInstructionSet();
    Ref<String> getManufacturer();
    Ref<ProcessorBrand> getBrand();
    Ref<List<String>> getCapabilities();
    Ref<Integer> getTotalCores();
    Ref<Integer> getTotalThreads();
    Ref<String> getModel();
    ProcessorIdObject getProcessorId();

    Ref<Integer> getMaxSpeedMhz();
    Ref<Status> getStatus();

    ResourceSupplier getComputerSystem();
}
