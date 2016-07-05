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
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.common.types.DriveType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StorageControllerInterface;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.math.BigDecimal;
import java.net.URI;
import java.util.Collection;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.USED_BY;
import static com.intel.podm.business.entities.base.DomainObjectProperties.decimalProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class PhysicalDrive extends DomainObject implements Discoverable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<StorageControllerInterface> CONTROLLER_INTERFACE =
            enumProperty("controllerInterface", StorageControllerInterface.class);
    public static final DomainObjectProperty<BigDecimal> CAPACITY_GIB = decimalProperty("capacityGib");
    public static final DomainObjectProperty<DriveType> TYPE = enumProperty("type", DriveType.class);
    public static final DomainObjectProperty<Integer> RPM = integerProperty("rpm");
    public static final DomainObjectProperty<String> MANUFACTURER = stringProperty("manufacturer");
    public static final DomainObjectProperty<String> MODEL = stringProperty("model");
    public static final DomainObjectProperty<String> SERIAL_NUMBER = stringProperty("serialNumber");

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

    public StorageControllerInterface getControllerInterface() {
        return getProperty(CONTROLLER_INTERFACE);
    }

    public void setControllerInterface(StorageControllerInterface storageControllerInterface) {
        setProperty(CONTROLLER_INTERFACE, storageControllerInterface);
    }

    public BigDecimal getCapacityGib() {
        return getProperty(CAPACITY_GIB);
    }

    public void setCapacityGib(BigDecimal capacityGib) {
        setProperty(CAPACITY_GIB, capacityGib);
    }

    public DriveType getType() {
        return getProperty(TYPE);
    }

    public void setType(DriveType driveType) {
        setProperty(TYPE, driveType);
    }

    public Integer getRpm() {
        return getProperty(RPM);
    }

    public void setRpm(Integer rpm) {
        setProperty(RPM, rpm);
    }

    public String getManufacturer() {
        return getProperty(MANUFACTURER);
    }

    public void setManufacturer(String manufacturer) {
        setProperty(MANUFACTURER, manufacturer);
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

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public Collection<LogicalDrive> getUsedBy() {
        return getLinked(USED_BY, LogicalDrive.class);
    }

    public StorageService getStorageService() {
        return single(getLinked(CONTAINED_BY, StorageService.class));
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
}
