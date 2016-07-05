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

package com.intel.podm.redfish.json.templates.assembly;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.intel.podm.business.dto.redfish.RequestedEthernetInterface;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.deserialization.PositiveIntegerDeserializer;

import java.util.List;
import java.util.Optional;

import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_INTERFACE;
import static com.intel.podm.redfish.UriConverter.getContextFromUri;
import static java.util.Optional.ofNullable;

public final class RequestedEthernetInterfaceImpl implements RequestedEthernetInterface {
    @JsonProperty
    @JsonDeserialize(using = PositiveIntegerDeserializer.class)
    private Integer speedMbps;

    @JsonProperty("PrimaryVLAN")
    @JsonDeserialize(using = PositiveIntegerDeserializer.class)
    private Integer primaryVlan;

    @JsonProperty("VLANs")
    private List<VlanImpl> vlans;

    private Context resourceContext;

    private Context chassisContext;

    @Override
    public Integer getSpeedMbps() {
        return speedMbps;
    }

    @Override
    public Integer getPrimaryVlan() {
        return primaryVlan;
    }

    @Override
    public Optional<List<Vlan>> getVlans() {
        return ofNullable((List) vlans);
    }

    @JsonProperty("Resource")
    public void setResourceContext(ODataId resource) {
        if (resource == null) {
            return;
        }

        resourceContext = getContextFromUri(resource.toUri(), ETHERNET_INTERFACE);
    }

    @Override
    public Context getResourceContext() {
        return resourceContext;
    }

    @JsonProperty("Chassis")
    public void setChassisContext(ODataId chassis) {
        if (chassis == null) {
            return;
        }

        chassisContext = getContextFromUri(chassis.toUri(), CHASSIS);
    }

    @Override
    public Context getChassisContext() {
        return chassisContext;
    }

    private static final class VlanImpl implements Vlan {
        @JsonProperty("VLANId")
        private Integer vlanId;

        @JsonProperty
        private boolean tagged;

        @Override
        public boolean isTagged() {
            return tagged;
        }

        @Override
        public Integer getVlanId() {
            return vlanId;
        }
    }
}
