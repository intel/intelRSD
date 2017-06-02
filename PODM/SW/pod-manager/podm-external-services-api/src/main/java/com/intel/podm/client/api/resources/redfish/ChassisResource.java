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
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;

import java.util.UUID;

@SuppressWarnings({"checkstyle:MethodCount"})
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
    Ref<String> getLocationId();
    String getLocationParentId();
    RackScaleRackChassisOem getRackChassisAttributes();

    ResourceSupplier getThermal() throws ExternalServiceApiReaderException;
    ResourceSupplier getPower() throws ExternalServiceApiReaderException;

    Iterable<ResourceSupplier> getPowerZones() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getThermalZones() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getContained() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getComputerSystems() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getSwitches() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getDrives() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getStorage() throws ExternalServiceApiReaderException;

    interface RackScaleChassisOem extends OemVendor {
    }

    interface RackScaleRackChassisOem extends OemVendor {
        Integer getRackPuid();
        LocationObject getLocation();
        UUID getUuid();
        String getGeoTag();
        Boolean getRackSupportsDisaggregatedPowerCooling();
    }
}

