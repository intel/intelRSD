/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.ReferenceableMember;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.function.Predicate;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;
import static com.intel.podm.common.utils.Collections.firstByPredicate;

@OdataTypes({
    "#Thermal" + VERSION_PATTERN + "Thermal"
})
public class ThermalResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Temperatures")
    List<TemperatureItem> temperatures = new ArrayList<>();
    @JsonProperty("Fans")
    List<ThermalFanItem> fans = new ArrayList<>();
    @JsonProperty("Redundancy")
    List<RedundancyItem> redundancies = new ArrayList<>();
    @JsonProperty("Oem")
    Oem oem = new Oem();
    @JsonProperty("Status")
    private Status status;

    public Status getStatus() {
        return status;
    }

    public Integer getDesiredSpeedPwm() {
        return this.oem.rackScaleOem.desiredSpeedPwm;
    }

    public Integer getVolumetricAirflowCfm() {
        return this.oem.rackScaleOem.volumetricAirflowCfm;
    }

    @Override
    public ExternalServiceResource getByFragment(String uriFragment) {
        Pattern pattern = Pattern.compile("/?(?<name>\\w+)/(?<id>\\d+)");
        Matcher matcher = pattern.matcher(uriFragment);

        if (matcher.matches()) {
            String id = matcher.group("id");
            String name = matcher.group("name");
            Predicate<ReferenceableMember> withTheSameId =
                referenceableMember -> Objects.equals(referenceableMember.getIdFromUriFragment(referenceableMember.getUri().getFragment()), id);

            switch (name) {
                case "Redundancy":
                    return firstByPredicate(redundancies, withTheSameId).orElse(null);
                case "Fans":
                    return firstByPredicate(fans, withTheSameId).orElse(null);
                case "Temperatures":
                    return firstByPredicate(temperatures, withTheSameId).orElse(null);
                default:
            }
        }

        return null;
    }

    @LinkName("temperatures")
    public Iterable<ResourceSupplier> getTemperatures() throws WebClientRequestException {
        return toSuppliersFromResources(temperatures);
    }

    @LinkName("fans")
    public Iterable<ResourceSupplier> getFans() throws WebClientRequestException {
        return toSuppliersFromResources(fans);
    }

    @LinkName("redundancy")
    public Iterable<ResourceSupplier> getRedundancies() throws WebClientRequestException {
        return toSuppliersFromResources(redundancies);
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String oDataType = "#Intel.Oem.Thermal";
            @JsonProperty("DesiredSpeedPwm")
            private Integer desiredSpeedPwm;
            @JsonProperty("VolumetricAirflowCfm")
            private Integer volumetricAirflowCfm;
        }
    }
}
