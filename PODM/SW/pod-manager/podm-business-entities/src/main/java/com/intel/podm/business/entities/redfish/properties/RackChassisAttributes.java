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

import javax.enterprise.context.Dependent;
import java.util.UUID;

import static com.intel.podm.business.entities.base.DomainObjectProperties.booleanProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.uuidProperty;

@Dependent
public class RackChassisAttributes extends DomainObject {
    public static final DomainObjectProperty<Boolean> SUPPORTING_DISAGGREGATED_POWER_COOLING = booleanProperty("rackSupportsDisaggregatedPowerCooling");
    public static final DomainObjectProperty<UUID> RACK_UUID = uuidProperty("rackUuid");
    public static final DomainObjectProperty<String> GEO_TAG = stringProperty("geoTag");
    public static final DomainObjectProperty<Integer> RACK_PUID = integerProperty("rackPuid");

    public UUID getUuid() {
        return getProperty(RACK_UUID);
    }

    public void setUuid(UUID rackUuid) {
        setProperty(RACK_UUID, rackUuid);
    }

    public String getGeoTag() {
        return getProperty(GEO_TAG);
    }

    public void setGeoTag(String geoTag) {
        setProperty(GEO_TAG, geoTag);
    }

    public Integer getRackPuid() {
        return getProperty(RACK_PUID);
    }

    public void setRackPuid(Integer rackPuid) {
        setProperty(RACK_PUID, rackPuid);
    }

    public Boolean getRackSupportsDisaggregatedPowerCooling() {
        return getProperty(SUPPORTING_DISAGGREGATED_POWER_COOLING);
    }

    public void setRackSupportsDisaggregatedPowerCooling(Boolean rackSupportsDisaggregatedPowerCooling) {
        setProperty(SUPPORTING_DISAGGREGATED_POWER_COOLING, rackSupportsDisaggregatedPowerCooling);
    }
}
