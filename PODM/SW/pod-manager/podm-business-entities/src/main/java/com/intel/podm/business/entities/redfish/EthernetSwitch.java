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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.business.entities.redfish.base.Manageable;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Collection;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINS;
import static com.intel.podm.business.entities.base.DomainObjectLink.MANAGED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class EthernetSwitch extends DomainObject implements Discoverable, Manageable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<String> SWITCH_ID = stringProperty("switchId");
    public static final DomainObjectProperty<String> MANUFACTURER = stringProperty("manufacturer");
    public static final DomainObjectProperty<String> MANUFACTURING_DATE = stringProperty("manufacturingDate");
    public static final DomainObjectProperty<String> MODEL = stringProperty("model");
    public static final DomainObjectProperty<String> SERIAL_NUMBER = stringProperty("serialNumber");
    public static final DomainObjectProperty<String> PART_NUMBER = stringProperty("partNumber");
    public static final DomainObjectProperty<String> FIRMWARE_NAME = stringProperty("firmwareName");
    public static final DomainObjectProperty<String> FIRMWARE_VERSION = stringProperty("firmwareVersion");
    public static final DomainObjectProperty<String> ROLE = stringProperty("role");

    @Override
    public String getName() {
        return getProperty(NAME);
    }

    @Override
    public void setName(String name) {
        setProperty(NAME, name);
    }

    @Override
    public String getDescription() {
        return getProperty(DESCRIPTION);
    }

    @Override
    public void setDescription(String description) {
        setProperty(DESCRIPTION, description);
    }

    public String getSwitchId() {
        return getProperty(SWITCH_ID);
    }

    public void setSwitchId(String switchId) {
        setProperty(SWITCH_ID, switchId);
    }

    public String getManufacturer() {
        return getProperty(MANUFACTURER);
    }

    public void setManufacturer(String manufacturer) {
        setProperty(MANUFACTURER, manufacturer);
    }

    public String getManufacturingDate() {
        return getProperty(MANUFACTURING_DATE);
    }

    public void setManufacturingDate(String manufacturingDate) {
        setProperty(MANUFACTURING_DATE, manufacturingDate);
    }

    public String getModel() {
        return getProperty(MODEL);
    }

    public void setModel(String model) {
        setProperty(MODEL, model);
    }

    public String getSerialNumber() {
        return getProperty(SERIAL_NUMBER);
    }

    public void setSerialNumber(String serialNumber) {
        setProperty(SERIAL_NUMBER, serialNumber);
    }

    public String getPartNumber() {
        return getProperty(PART_NUMBER);
    }

    public void setPartNumber(String partNumber) {
        setProperty(PART_NUMBER, partNumber);
    }

    public String getFirmwareName() {
        return getProperty(FIRMWARE_NAME);
    }

    public void setFirmwareName(String firmwareName) {
        setProperty(FIRMWARE_NAME, firmwareName);
    }

    public String getFirmwareVersion() {
        return getProperty(FIRMWARE_VERSION);
    }

    public void setFirmwareVersion(String version) {
        setProperty(FIRMWARE_VERSION, version);
    }

    public String getRole() {
        return getProperty(ROLE);
    }

    public void setRole(String role) {
        setProperty(ROLE, role);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public Chassis getChassis() {
        return singleOrNull(getLinked(CONTAINED_BY, Chassis.class));
    }

    public void addPort(EthernetSwitchPort port) {
        link(CONTAINS, port);
    }

    public Collection<EthernetSwitchPort> getPortCollection() {
        return getLinked(CONTAINS, EthernetSwitchPort.class);
    }

    @Override
    public URI getSourceUri() {
        return getProperty(SOURCE_URI);
    }

    @Override
    public void setSourceUri(URI sourceUri) {
        setProperty(SOURCE_URI, sourceUri);
    }

    @Override
    public ExternalService getService() {
        return singleOrNull(getLinked(OWNED_BY, ExternalService.class));
    }

    @Override
    public Collection<Manager> getManagers() {
        return getLinked(MANAGED_BY);
    }

    @Override
    public void addManager(Manager manager) {
        link(MANAGED_BY, manager);
    }
}
