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
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.business.entities.redfish.properties.RackLocation;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINS;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
public class PowerZone extends DomainObject implements Discoverable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<Integer> MAX_PSUS_SUPPORTED = integerProperty("maxPsusSupported");
    public static final DomainObjectProperty<String> PRESENCE = stringProperty("presence");
    public static final DomainObjectProperty<Integer> NUMBER_OF_PSUS_PRESENT = integerProperty("numberOfPsusPresent");
    public static final DomainObjectProperty<Integer> POWER_CONSUMED_WATTS  = integerProperty("powerConsumedWatts");
    public static final DomainObjectProperty<Integer> POWER_OUTPUT_WATTS = integerProperty("powerOutputWatts");
    public static final DomainObjectProperty<Integer> POWER_CAPACITY_WATTS = integerProperty("powerCapacityWatts");

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

    public Integer getNumberOfPsusPresent() {
        return getProperty(NUMBER_OF_PSUS_PRESENT);
    }

    public void setNumberOfPsusPresent(Integer numberOfPsusPresent) {
        setProperty(NUMBER_OF_PSUS_PRESENT, numberOfPsusPresent);
    }

    public String getPresence() {
        return getProperty(PRESENCE);
    }

    public void setPresence(String presence) {
        setProperty(PRESENCE, presence);
    }

    public PowerSupply addPowerSupply() {
        return addDomainObject(CONTAINS, PowerSupply.class);
    }

    public List<PowerSupply> getPowerSupplies() {
        return getLinked(CONTAINS, PowerSupply.class);
    }

    public void setRackLocation(RackLocation rackLocation) {
        link(CONTAINS, rackLocation);
    }

    public RackLocation getRackLocation() {
        return singleOrNull(getLinked(CONTAINS, RackLocation.class));
    }

    public Integer getMaxPsusSupported() {
        return getProperty(MAX_PSUS_SUPPORTED);
    }

    public void setMaxPsusSupported(Integer maxPsusSupported) {
        setProperty(MAX_PSUS_SUPPORTED, maxPsusSupported);
    }

    public Integer getPowerCapacityWatts() {
        return getProperty(POWER_CAPACITY_WATTS);
    }

    public void setPowerCapacityWatts(Integer powerAvailableWatts) {
        setProperty(POWER_CAPACITY_WATTS, powerAvailableWatts);
    }

    public Integer getPowerOutputWatts() {
        return getProperty(POWER_OUTPUT_WATTS);
    }

    public void setPowerOutputWatts(Integer powerOutputWatts) {
        setProperty(POWER_OUTPUT_WATTS, powerOutputWatts);
    }

    public Integer getPowerConsumedWatts() {
        return getProperty(POWER_CONSUMED_WATTS);
    }

    public void setPowerConsumedWatts(Integer powerConsumedWatts) {
        setProperty(POWER_CONSUMED_WATTS, powerConsumedWatts);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
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
