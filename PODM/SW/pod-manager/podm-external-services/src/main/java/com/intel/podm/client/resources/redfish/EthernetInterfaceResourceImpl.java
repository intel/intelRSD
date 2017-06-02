/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.EthernetInterfaceResource;
import com.intel.podm.client.api.resources.redfish.IpV4AddressObject;
import com.intel.podm.client.api.resources.redfish.IpV6AddressObject;
import com.intel.podm.client.api.resources.redfish.IpV6AddressPolicyObject;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.annotations.AsUnassigned;
import com.intel.podm.common.types.net.MacAddress;

import java.util.List;

import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

@OdataTypes({
    "#EthernetInterface" + OdataTypes.VERSION_PATTERN + "EthernetInterface"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class EthernetInterfaceResourceImpl extends ExternalServiceResourceImpl implements EthernetInterfaceResource {
    @JsonProperty ("FQDN")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> fqdn = unassigned();
    @JsonProperty ("HostName")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> hostName = unassigned();
    @JsonProperty ("Status")
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status = unassigned();
    @JsonProperty ("InterfaceEnabled")
    @AsUnassigned(WHEN_NULL)
    private Ref<Boolean> interfaceEnabled = unassigned();
    @JsonProperty ("PermanentMACAddress")
    @AsUnassigned(WHEN_NULL)
    private Ref<MacAddress> permanentMacAddress = unassigned();
    @JsonProperty ("MACAddress")
    @AsUnassigned(WHEN_NULL)
    private Ref<MacAddress> macAddress = unassigned();
    @JsonProperty ("SpeedMbps")
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> speedMbps = unassigned();
    @JsonProperty ("AutoNeg")
    @AsUnassigned(WHEN_NULL)
    private Ref<Boolean> autoNeg = unassigned();
    @JsonProperty ("FullDuplex")
    @AsUnassigned(WHEN_NULL)
    private Ref<Boolean> fullDuplex = unassigned();
    @JsonProperty ("MTUSize")
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> mtuSize = unassigned();
    @JsonProperty("IPv6DefaultGateway")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> ipV6DefaultGateway = unassigned();
    @JsonProperty ("MaxIPv6StaticAddresses")
    @AsUnassigned(WHEN_NULL)
    private Ref<Integer> maxIPv6StaticAddresses = unassigned();
    @JsonProperty("IPv4Addresses")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<IpV4AddressObjectImpl>> ipV4Addresses = unassigned();
    @JsonProperty("IPv6Addresses")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<IpV6AddressObjectImpl>> ipV6Addresses = unassigned();
    @JsonProperty("IPv6StaticAddresses")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<IpV6AddressObjectImpl>> ipV6StaticAddresses = unassigned();
    @JsonProperty("IPv6AddressPolicyTable")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<IpV6AddressPolicyObjectImpl>> ipV6AddressesPolicies = unassigned();
    @JsonProperty("NameServers")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<String>> nameServers = unassigned();
    @JsonProperty("VLAN")
    @AsUnassigned(WHEN_NULL)
    private Ref<Vlan> vlan = unassigned();
    @JsonProperty("VLANs")
    private ODataId vlanCollection;

    @Override
    public Ref<String> getFqdn() {
        return fqdn;
    }

    @Override
    public Ref<String> getHostName() {
        return hostName;
    }

    @Override
    public Ref<Status> getStatus() {
        return status;
    }

    @Override
    public Ref<Boolean> getInterfaceEnabled() {
        return interfaceEnabled;
    }

    @Override
    public Ref<MacAddress> getPermanentMacAddress() {
        return permanentMacAddress;
    }

    @Override
    public Ref<MacAddress> getMacAddress() {
        return macAddress;
    }

    @Override
    public Ref<Integer> getSpeedMbps() {
        return speedMbps;
    }

    @Override
    public Ref<Boolean> getAutoNeg() {
        return autoNeg;
    }

    @Override
    public Ref<Boolean> isFullDuplex() {
        return fullDuplex;
    }

    @Override
    public Ref<Integer> getMtuSize() {
        return mtuSize;
    }

    @Override
    public Ref<Boolean> getVlanEnable() {
        return vlan.flatMap(v -> v.vlanEnable);
    }

    @Override
    public Ref<Integer> getVlanId() {
        return vlan.flatMap(v -> v.vlanId);
    }

    @Override
    public Ref<String> getIpV6DefaultGateway() {
        return ipV6DefaultGateway;
    }

    @Override
    public Ref<Integer> getMaxIPv6StaticAddresses() {
        return maxIPv6StaticAddresses;
    }

    @Override
    public Ref<List<IpV4AddressObject>> getIpV4Addresses() {
        return (Ref) ipV4Addresses;
    }

    @Override
    public Ref<List<IpV6AddressObject>> getIpV6Addresses() {
        return (Ref) ipV6Addresses;
    }

    @Override
    public Ref<List<IpV6AddressObject>> getIpV6StaticAddresses() {
        return (Ref) ipV6StaticAddresses;
    }

    @Override
    public Ref<List<IpV6AddressPolicyObject>> getIpV6AddressesPolicyTable() {
        return (Ref) ipV6AddressesPolicies;
    }

    @Override
    public Ref<List<String>> getNameServers() {
        return nameServers;
    }

    @Override
    public ResourceSupplier getComputerSystem() {
        //FIXME: workaround until link to computer system is available
        return toSupplier(getComputerSystemODataId());
    }

    @Override
    @LinkName("ethernetInterfaceVlans")
    public Iterable<ResourceSupplier> getVlans() throws ExternalServiceApiReaderException {
        return processMembersListResource(this.vlanCollection);
    }

    private static final class Vlan {
        @JsonProperty("VLANEnable")
        @AsUnassigned(WHEN_NULL)
        private Ref<Boolean> vlanEnable = unassigned();
        @JsonProperty("VLANId")
        @AsUnassigned(WHEN_NULL)
        private Ref<Integer> vlanId = unassigned();
    }
}
