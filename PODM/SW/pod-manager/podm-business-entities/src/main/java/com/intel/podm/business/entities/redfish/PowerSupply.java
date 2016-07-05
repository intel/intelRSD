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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.business.entities.redfish.properties.RackLocation;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINS;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
public class PowerSupply extends DomainObject implements StatusPossessor {
    public static final DomainObjectProperty<String> NAME = stringProperty("name");
    public static final DomainObjectProperty<Integer> POWER_CAPACITY_WATTS = integerProperty("powerCapacityWatts");
    public static final DomainObjectProperty<Integer> LAST_POWER_OUTPUT_WATTS = integerProperty("lastPowerOutputWatt");
    public static final DomainObjectProperty<String> SERIAL_NUMBER = stringProperty("serialNumber");
    public static final DomainObjectProperty<String> MANUFACTURER = stringProperty("manufacturer");
    public static final DomainObjectProperty<String> MODEL = stringProperty("model");
    public static final DomainObjectProperty<String> PART_NUMBER = stringProperty("partNumber");
    public static final DomainObjectProperty<String> FIRMWARE_VERSION = stringProperty("firmwareVersion");

    public void setRackLocation(RackLocation rackLocation) {
        link(CONTAINS, rackLocation);
    }

    public RackLocation getRackLocation() {
        return singleOrNull(getLinked(CONTAINS, RackLocation.class));
    }

    public String getFirmwareVersion() {
        return getProperty(FIRMWARE_VERSION);
    }

    public void setFirmwareVersion(String firmwareVersion) {
        setProperty(FIRMWARE_VERSION, firmwareVersion);
    }

    public String getPartNumber() {
        return getProperty(PART_NUMBER);
    }

    public void setPartNumber(String partNumber) {
        setProperty(PART_NUMBER, partNumber);
    }

    public String getModel() {
        return getProperty(MODEL);
    }

    public void setModel(String model) {
        setProperty(MODEL, model);
    }

    public String getManufacturer() {
        return getProperty(MANUFACTURER);
    }

    public void setManufacturer(String manufacturer) {
        setProperty(MANUFACTURER, manufacturer);
    }

    public String getSerialNumber() {
        return getProperty(SERIAL_NUMBER);
    }

    public void setSerialNumber(String serialNumber) {
        setProperty(SERIAL_NUMBER, serialNumber);
    }

    public Integer getLastPowerOutputWatt() {
        return getProperty(LAST_POWER_OUTPUT_WATTS);
    }

    public void setLastPowerOutputWatt(Integer lastPowerOutputWatt) {
        setProperty(LAST_POWER_OUTPUT_WATTS, lastPowerOutputWatt);
    }

    public Integer getPowerCapacityWatts() {
        return getProperty(POWER_CAPACITY_WATTS);
    }

    public void setPowerCapacityWatts(Integer powerCapacityWatts) {
        setProperty(POWER_CAPACITY_WATTS, powerCapacityWatts);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public String getName() {
        return getProperty(NAME);
    }

    public void setName(String name) {
        setProperty(NAME, name);
    }
}
