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

package com.intel.podm.business.entities.redfish.properties;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectLink;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.common.types.DriveType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StorageControllerInterface;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.math.BigDecimal;

import static com.intel.podm.business.entities.base.DomainObjectProperties.decimalProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.single;
import static java.lang.Math.pow;
import static java.math.BigDecimal.ROUND_HALF_DOWN;
import static java.math.BigDecimal.valueOf;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class SimpleStorageDevice extends DomainObject implements StatusPossessor, LocalStorage {
    public static final DomainObjectProperty<String> NAME = stringProperty("name");
    public static final DomainObjectProperty<String> MANUFACTURER = stringProperty("manufacturer");
    public static final DomainObjectProperty<String> MODEL = stringProperty("model");
    public static final DomainObjectProperty<BigDecimal> CAPACITY_BYTES = decimalProperty("capacityBytes");

    //TODO: move this logic from business
    private static final double BYTES_IN_GIBIBYTE = pow(2, 30);

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
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

    public String getName() {
        return getProperty(NAME);
    }

    public void setName(String name) {
        setProperty(NAME, name);
    }

    public BigDecimal getCapacityBytes() {
        return getProperty(CAPACITY_BYTES);
    }

    public void setCapacityBytes(BigDecimal capacityBytes) {
        setProperty(CAPACITY_BYTES, capacityBytes);
    }

    @Override
    public BigDecimal getCapacityGib() {
        if (getCapacityBytes() == null) {
            return null;
        }

        return getCapacityBytes().divide(valueOf(BYTES_IN_GIBIBYTE), ROUND_HALF_DOWN);
    }

    @Override
    public DriveType getType() {
        return null;
    }

    @Override
    public Integer getRpm() {
        return null;
    }

    @Override
    public StorageControllerInterface getInterface() {
        return null;
    }

    @Override
    public String getSerialNumber() {
        return null;
    }

    @Override
    public SimpleStorage getParent() {
        return single(getLinked(DomainObjectLink.SIMPLE_STORAGE, SimpleStorage.class));
    }
}
