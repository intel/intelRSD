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
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
public class Temperature extends DomainObject implements StatusPossessor {
    public static final DomainObjectProperty<String> NAME = stringProperty("name");
    public static final DomainObjectProperty<Integer> READING_CELSIUS = integerProperty("readingCelsius");
    public static final DomainObjectProperty<PhysicalContext> PHYSICAL_CONTEXT = enumProperty("physicalContext", PhysicalContext.class);

    public PhysicalContext getPhysicalContext() {
        return getProperty(PHYSICAL_CONTEXT);
    }

    public void setPhysicalContext(PhysicalContext physicalContext) {
        setProperty(PHYSICAL_CONTEXT, physicalContext);
    }

    public Integer getReadingCelsius() {
        return getProperty(READING_CELSIUS);
    }

    public void setReadingCelsius(Integer readingCelsius) {
        setProperty(READING_CELSIUS, readingCelsius);
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
