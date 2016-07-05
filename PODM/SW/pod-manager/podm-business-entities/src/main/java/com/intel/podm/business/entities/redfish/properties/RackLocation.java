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

package com.intel.podm.business.entities.redfish.properties;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.common.types.rmm.RackUnitType;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
public class RackLocation extends DomainObject {
    public static final DomainObjectProperty<Integer> U_LOCATION = integerProperty("uLocation");
    public static final DomainObjectProperty<Integer> U_HEIGHT = integerProperty("uHeight");
    public static final DomainObjectProperty<RackUnitType> RACK_UNIT = enumProperty("rackUnit", RackUnitType.class);
    public static final DomainObjectProperty<Integer> X_LOCATION = integerProperty("xLocation");

    public Integer getXLocation() {
        return getProperty(X_LOCATION);
    }

    public void setXLocation(Integer xLocation) {
        setProperty(X_LOCATION, xLocation);
    }

    public Integer getULocation() {
        return getProperty(U_LOCATION);
    }

    public void setULocation(Integer uLocation) {
        setProperty(U_LOCATION, uLocation);
    }

    public Integer getUHeight() {
        return getProperty(U_HEIGHT);
    }

    public void setUHeight(Integer uHeight) {
        setProperty(U_HEIGHT, uHeight);
    }

    public RackUnitType getRackUnit() {
        return getProperty(RACK_UNIT);
    }

    public void setRackUnit(RackUnitType rackUnit) {
        setProperty(RACK_UNIT, rackUnit);
    }
}
