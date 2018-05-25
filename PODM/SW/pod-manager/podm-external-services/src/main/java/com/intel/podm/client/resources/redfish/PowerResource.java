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
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.function.Predicate;
import java.util.regex.Matcher;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;
import static com.intel.podm.common.utils.Collections.firstByPredicate;

@OdataTypes({
    "#Power" + VERSION_PATTERN + "Power"
})
public class PowerResource extends ExternalServiceResourceImpl {
    @JsonProperty("PowerControl")
    List<PowerControlItem> powerControls = new ArrayList<>();

    @JsonProperty("Voltages")
    List<VoltageItem> voltages = new ArrayList<>();

    @JsonProperty("PowerSupplies")
    List<PowerSupplyItem> powerSupplies = new ArrayList<>();

    @JsonProperty("Redundancy")
    List<RedundancyItem> redundancy = new ArrayList<>();

    @JsonProperty("Oem")
    Oem oem = new Oem();

    @LinkName("powerControls")
    public Iterable<ResourceSupplier> getPowerControls() throws WebClientRequestException {
        return toSuppliersFromResources(powerControls);
    }

    @LinkName("voltages")
    public Iterable<ResourceSupplier> getVoltages() throws WebClientRequestException {
        return toSuppliersFromResources(voltages);
    }

    @LinkName("redundancy")
    public Iterable<ResourceSupplier> getRedundancy() throws WebClientRequestException {
        return toSuppliersFromResources(redundancy);
    }

    @LinkName("powerSupplies")
    public Iterable<ResourceSupplier> getPowerSupplies() throws WebClientRequestException {
        return toSuppliersFromResources(powerSupplies);
    }

    @Override
    @SuppressWarnings({"checkstyle:ReturnCount"})
    public ExternalServiceResource getByFragment(String uriFragment) {
        Matcher matcher = createMatcher(uriFragment, "name", "id");

        if (matcher.matches()) {
            String id = matcher.group("id");
            String name = matcher.group("name");
            Predicate<ReferenceableMember> withTheSameId =
                referenceableMember -> Objects.equals(referenceableMember.getIdFromUriFragment(referenceableMember.getUri().getFragment()), id);

            switch (name) {
                case "Voltages":
                    return firstByPredicate(voltages, withTheSameId).orElse(null);
                case "Redundancy":
                    return firstByPredicate(redundancy, withTheSameId).orElse(null);
                case "PowerControl":
                    return firstByPredicate(powerControls, withTheSameId).orElse(null);
                case "PowerSupplies":
                    return firstByPredicate(powerSupplies, withTheSameId).orElse(null);
                default:
            }
        }

        return null;
    }

    public BigDecimal getInputAcPowerWatts() {
        return oem.rackScaleOem.inputAcPowerWatts;
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private final RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String oDataType = "#Intel.Oem.Power";
            @JsonProperty("InputACPowerWatts")
            private BigDecimal inputAcPowerWatts;
        }
    }
}
