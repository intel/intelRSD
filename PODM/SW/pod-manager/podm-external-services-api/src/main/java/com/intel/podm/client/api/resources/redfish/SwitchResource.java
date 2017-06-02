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
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.actions.ResetType;

import java.util.Set;

@SuppressWarnings({"checkstyle:MethodCount"})
public interface SwitchResource extends ExternalServiceResource {
    Iterable<ResourceSupplier> getPorts() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getChassis() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getManagedBy() throws ExternalServiceApiReaderException;
    Protocol getSwitchType();
    String getModel();
    String getManufacturer();
    String getSku();
    String getSerialNumber();
    String getPartNumber();
    String getAssetTag();
    Integer getDomainId();
    Boolean getManaged();
    Integer getTotalSwitchWidth();
    IndicatorLed getIndicatorLed();
    PowerState getPowerState();
    Status getStatus();
    Set<ResetType> getAllowableResetTypes();
}
