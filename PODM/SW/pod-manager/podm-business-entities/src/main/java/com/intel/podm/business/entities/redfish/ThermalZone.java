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
import com.intel.podm.business.entities.redfish.properties.Fan;
import com.intel.podm.business.entities.redfish.properties.RackLocation;
import com.intel.podm.business.entities.redfish.properties.Temperature;
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
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class ThermalZone extends DomainObject implements Discoverable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<Integer> MAX_FANS_SUPPORTED = integerProperty("maxFansSupported");
    public static final DomainObjectProperty<Integer> DESIRED_SPEED_PWM = integerProperty("desiredSpeedPwm");
    public static final DomainObjectProperty<Integer> DESIRED_SPEED_RPM = integerProperty("desiredSpeedRpm");
    public static final DomainObjectProperty<String> PRESENCE = stringProperty("presence");
    public static final DomainObjectProperty<Integer> NUMBER_OF_FANS_PRESENT = integerProperty("numberOfFansPresent");
    public static final DomainObjectProperty<Integer> VOLUMETRIC_AIRFLOW = integerProperty("volumetricAirflow");

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

    public Integer getVolumetricAirflow() {
        return getProperty(VOLUMETRIC_AIRFLOW);
    }

    public void setVolumetricAirflow(Integer volumetricAirflow) {
        setProperty(VOLUMETRIC_AIRFLOW, volumetricAirflow);
    }

    public Integer getNumberOfFansPresent() {
        return getProperty(NUMBER_OF_FANS_PRESENT);
    }

    public void setNumberOfFansPresent(Integer numberOfFansPresent) {
        setProperty(NUMBER_OF_FANS_PRESENT, numberOfFansPresent);
    }

    public String getPresence() {
        return getProperty(PRESENCE);
    }

    public void setPresence(String presence) {
        setProperty(PRESENCE, presence);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public Integer getDesiredSpeedRpm() {
        return getProperty(DESIRED_SPEED_RPM);
    }

    public void setDesiredSpeedRpm(Integer desiredSpeedRpm) {
        setProperty(DESIRED_SPEED_RPM, desiredSpeedRpm);
    }

    public Integer getDesiredSpeedPwm() {
        return getProperty(DESIRED_SPEED_PWM);
    }

    public void setDesiredSpeedPwm(Integer desiredSpeedPwm) {
        setProperty(DESIRED_SPEED_PWM, desiredSpeedPwm);
    }

    public Integer getMaxFansSupported() {
        return getProperty(MAX_FANS_SUPPORTED);
    }

    public void setMaxFansSupported(Integer maxFansSupported) {
        setProperty(MAX_FANS_SUPPORTED, maxFansSupported);
    }

    public List<Fan> getFans() {
        return getLinked(CONTAINS, Fan.class);
    }

    public void link(Fan fan) {
        link(CONTAINS, fan);
    }

    public void link(Temperature temperature) {
        link(CONTAINS, temperature);
    }

    public List<Temperature> getTemperatures() {
        return getLinked(CONTAINS, Temperature.class);
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

    public RackLocation getRackLocation() {
        return singleOrNull(getLinked(CONTAINS, RackLocation.class));
    }

    public void setRackLocation(RackLocation rackLocation) {
        link(CONTAINS, rackLocation);
    }
}
