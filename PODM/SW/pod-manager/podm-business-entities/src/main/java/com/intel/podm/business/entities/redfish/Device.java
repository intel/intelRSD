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
import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.common.types.DriveType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StorageControllerInterface;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.math.BigDecimal;
import java.net.URI;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectProperties.decimalProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
public class Device extends DomainObject implements Discoverable, StatusPossessor, Descriptable, LocalStorage {
    public static final DomainObjectProperty<StorageControllerInterface> INTERFACE = enumProperty("interface", StorageControllerInterface.class);
    public static final DomainObjectProperty<BigDecimal> CAPACITY_GIB = decimalProperty("capacityGib");
    public static final DomainObjectProperty<DriveType> TYPE = enumProperty("type", DriveType.class);
    public static final DomainObjectProperty<Integer> RPM = integerProperty("rpm");
    public static final DomainObjectProperty<String> MANUFACTURER = stringProperty("manufacturer");
    public static final DomainObjectProperty<String> MODEL = stringProperty("model");
    public static final DomainObjectProperty<String> SERIAL_NUMBER = stringProperty("serialNumber");
    public static final DomainObjectProperty<String> FIRMWARE_VERSION = stringProperty("firmwareVersion");
    public static final DomainObjectProperty<String> BUS_INFO = stringProperty("busInfo");

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

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public String getBusInfo() {
        return getProperty(BUS_INFO);
    }

    public void setBusInfo(String busInfo) {
        setProperty(BUS_INFO, busInfo);
    }

    public String getFirmwareVersion() {
        return getProperty(FIRMWARE_VERSION);
    }

    public void setFirmwareVersion(String firmwareVersion) {
        setProperty(FIRMWARE_VERSION, firmwareVersion);
    }

    @Override
    public String getSerialNumber() {
        return getProperty(SERIAL_NUMBER);
    }

    public void setSerialNumber(String serialNumber) {
        setProperty(SERIAL_NUMBER, serialNumber);
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

    @Override
    public Integer getRpm() {
        return getProperty(RPM);
    }

    public void setRpm(Integer rpm) {
        setProperty(RPM, rpm);
    }

    @Override
    public DriveType getType() {
        return getProperty(TYPE);
    }

    public void setType(DriveType type) {
        setProperty(TYPE, type);
    }

    @Override
    public BigDecimal getCapacityGib() {
        return getProperty(CAPACITY_GIB);
    }

    public void setCapacityGib(BigDecimal capacityGib) {
        setProperty(CAPACITY_GIB, capacityGib);
    }

    @Override
    public StorageControllerInterface getInterface() {
        return getProperty(INTERFACE);
    }

    public void setInterface(StorageControllerInterface anInterface) {
        setProperty(INTERFACE, anInterface);
    }

    public Adapter getAdapter() {
        try {
            return single(getLinked(CONTAINED_BY, Adapter.class));
        } catch (IllegalStateException e) {
            throw new IllegalStateException("Device has to be contained by exactly one Adapter", e);
        }
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
    public Adapter getParent() {
        return getAdapter();
    }
}
