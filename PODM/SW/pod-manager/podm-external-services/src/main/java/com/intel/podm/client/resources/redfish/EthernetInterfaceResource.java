/*
 * Copyright (c) 2015-2018 Intel Corporation
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
import com.fasterxml.jackson.annotation.JsonSetter;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.LinkStatus;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.annotations.AsUnassigned;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.common.types.redfish.OemType;

import java.util.List;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@OdataTypes({
    "#EthernetInterface" + VERSION_PATTERN + "EthernetInterface"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class EthernetInterfaceResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonSetter(value = "FQDN", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<String> fqdn = unassigned();
    @JsonSetter(value = "HostName", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<String> hostName = unassigned();
    @JsonSetter(value = "Status", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status = unassigned();
    @JsonSetter(value = "LinkStatus", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<LinkStatus> linkStatus;
    @JsonSetter(value = "InterfaceEnabled", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Boolean> interfaceEnabled = unassigned();
    @JsonSetter(value = "PermanentMACAddress", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<MacAddress> permanentMacAddress = unassigned();
    @JsonSetter(value = "MACAddress", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<MacAddress> macAddress = unassigned();
    @JsonSetter(value = "SpeedMbps", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> speedMbps = unassigned();
    @JsonSetter(value = "AutoNeg", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Boolean> autoNeg = unassigned();
    @JsonSetter(value = "FullDuplex", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Boolean> fullDuplex = unassigned();
    @JsonSetter(value = "MTUSize", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> mtuSize = unassigned();
    @JsonSetter(value = "IPv6DefaultGateway", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<String> ipV6DefaultGateway = unassigned();
    @JsonSetter(value = "MaxIPv6StaticAddresses", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> maxIPv6StaticAddresses = unassigned();
    @JsonSetter(value = "IPv4Addresses", nulls = AS_EMPTY)
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<IpV4AddressObject>> ipV4Addresses = unassigned();
    @JsonSetter(value = "IPv6Addresses", nulls = AS_EMPTY)
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<IpV6AddressObject>> ipV6Addresses = unassigned();
    @JsonSetter(value = "IPv6StaticAddresses", nulls = AS_EMPTY)
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<IpV6AddressObject>> ipV6StaticAddresses = unassigned();
    @JsonSetter(value = "IPv6AddressPolicyTable", nulls = AS_EMPTY)
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<IpV6AddressPolicyObject>> ipV6AddressesPolicies = unassigned();
    @JsonSetter(value = "NameServers", nulls = AS_EMPTY)
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<String>> nameServers = unassigned();
    @JsonSetter(value = "VLAN", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Vlan> vlan = unassigned();
    @JsonProperty("VLANs")
    private ODataId vlanCollection;
    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public Ref<String> getFqdn() {
        return fqdn;
    }

    public Ref<String> getHostName() {
        return hostName;
    }

    public Ref<Status> getStatus() {
        return status;
    }

    public Ref<LinkStatus> getLinkStatus() {
        return linkStatus;
    }

    public Ref<Boolean> getInterfaceEnabled() {
        return interfaceEnabled;
    }

    public Ref<MacAddress> getPermanentMacAddress() {
        return permanentMacAddress;
    }

    public Ref<MacAddress> getMacAddress() {
        return macAddress;
    }

    public Ref<Integer> getSpeedMbps() {
        return speedMbps;
    }

    public Ref<Boolean> getAutoNeg() {
        return autoNeg;
    }

    public Ref<Boolean> isFullDuplex() {
        return fullDuplex;
    }

    public Ref<Integer> getMtuSize() {
        return mtuSize;
    }

    public Ref<Boolean> getVlanEnable() {
        return vlan.flatMap(v -> v.vlanEnable);
    }

    public Ref<Integer> getVlanId() {
        return vlan.flatMap(v -> v.vlanId);
    }

    public Ref<String> getIpV6DefaultGateway() {
        return ipV6DefaultGateway;
    }

    public Ref<Integer> getMaxIPv6StaticAddresses() {
        return maxIPv6StaticAddresses;
    }

    public Ref<List<IpV4AddressObject>> getIpV4Addresses() {
        return ipV4Addresses;
    }

    public Ref<List<IpV6AddressObject>> getIpV6Addresses() {
        return ipV6Addresses;
    }

    public Ref<List<IpV6AddressObject>> getIpV6StaticAddresses() {
        return ipV6StaticAddresses;
    }

    public Ref<List<IpV6AddressPolicyObject>> getIpV6AddressesPolicyTable() {
        return ipV6AddressesPolicies;
    }

    public Ref<List<String>> getNameServers() {
        return nameServers;
    }

    public Ref<List<Protocol>> getSupportedProtocols() {
        return oem.rackScaleOem.supportedProtocols;
    }

    public ResourceSupplier getComputerSystem() {
        //FIXME: workaround until link to computer system is available
        return toSupplier(getComputerSystemODataId());
    }

    @LinkName("ethernetInterfaceVlans")
    public Iterable<ResourceSupplier> getVlans() throws WebClientRequestException {
        return processMembersListResource(this.vlanCollection);
    }

    private static final class Vlan {
        @JsonSetter(value = "VLANEnable", nulls = AS_EMPTY)
        @AsUnassigned(WHEN_NULL)
        private Ref<Boolean> vlanEnable = unassigned();
        @JsonSetter(value = "VLANId", nulls = AS_EMPTY)
        @AsUnassigned(WHEN_NULL)
        private Ref<Integer> vlanId = unassigned();
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonSetter(value = "SupportedProtocols", nulls = AS_EMPTY)
            @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
            private Ref<List<Protocol>> supportedProtocols = unassigned();
        }
    }
}
