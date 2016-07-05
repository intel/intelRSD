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
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.Status;

import java.util.UUID;

public interface ChassisResource extends ExternalServiceResource {
    ChassisType getChassisType();
    String getManufacturer();
    String getModel();
    String getSku();
    String getSerialNumber();
    String getPartNumber();
    String getAssetTag();
    IndicatorLed getIndicatorLed();
    Status getStatus();
    String getLocationId();
    String getLocationParentId();
    RackScaleRackChassisOem getRackChassisAttributes();

    Iterable<ResourceSupplier> getPowerZones() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getThermalZones() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getContained() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getComputerSystems() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getSwitches() throws ExternalServiceApiReaderException;

    interface RackScaleChassisOem extends OemVendor {
    }

    interface RackScaleRackChassisOem extends OemVendor {
        Integer getRackPuid();
        UUID getUuid();
        String getGeoTag();
        Boolean getRackSupportsDisaggregatedPowerCooling();
    }
}

