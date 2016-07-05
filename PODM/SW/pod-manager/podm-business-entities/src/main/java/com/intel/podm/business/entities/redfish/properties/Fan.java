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
public class Fan extends DomainObject implements StatusPossessor {
    public static final DomainObjectProperty<String> NAME = stringProperty("name");
    public static final DomainObjectProperty<Integer> READING_RPM = integerProperty("readingRpm");

    public Integer getReadingRpm() {
        return getProperty(READING_RPM);
    }

    public void setReadingRpm(Integer reading) {
        setProperty(READING_RPM, reading);
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

    public void setName(String fanName) {
        setProperty(NAME, fanName);
    }

    public void setRackLocation(RackLocation rackLocation) {
        link(CONTAINS, rackLocation);
    }

    public RackLocation getRackLocation() {
        return singleOrNull(getLinked(CONTAINS, RackLocation.class));
    }
}
