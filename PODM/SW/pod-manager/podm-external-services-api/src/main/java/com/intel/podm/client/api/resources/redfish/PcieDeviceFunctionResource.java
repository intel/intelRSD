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
import com.intel.podm.common.types.DeviceClass;
import com.intel.podm.common.types.FunctionType;
import com.intel.podm.common.types.Status;

@SuppressWarnings({"checkstyle:MethodCount"})
public interface PcieDeviceFunctionResource extends ExternalServiceResource {
    Integer getFunctionId();
    DeviceClass getDeviceClass();
    FunctionType getFunctionType();
    String getDeviceId();
    String getVendorId();
    String getClassCode();
    String getRevisionId();
    String getSubsystemId();
    String getSubsystemVendorId();
    Status getStatus();
    ResourceSupplier getPcieDevice();
    Iterable<ResourceSupplier> getDrives() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getStorageControllers() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getEthernetInterfaces() throws ExternalServiceApiReaderException;
}
