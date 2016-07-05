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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.EthernetInterfaceResource;
import com.intel.podm.client.api.resources.redfish.IpV4AddressObject;
import com.intel.podm.client.api.resources.redfish.IpV6AddressObject;
import com.intel.podm.client.api.resources.redfish.IpV6AddressPolicyObject;
import com.intel.podm.client.api.resources.redfish.LuiEthernetInterfaceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.LinkTechnology;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;

import java.util.Collections;
import java.util.List;

import static java.util.Collections.emptyList;

@OdataTypes({
        "#EthernetInterface.1.0.0.EthernetInterface",
        "#EthernetInterface.1.0.1.EthernetInterface"
})
public class EthernetInterfaceResourceImpl extends ExternalServiceResourceImpl implements EthernetInterfaceResource, LuiEthernetInterfaceResource {
    @JsonProperty ("FQDN")
    private String fqdn;
    @JsonProperty ("HostName")
    private String hostName;
    @JsonProperty ("Status")
    private Status status;
    @JsonProperty ("InterfaceEnabled")
    private Boolean interfaceEnabled;
    @JsonProperty ("PermanentMACAddress")
    private MacAddress permanentMacAddress;
    @JsonProperty ("MACAddress")
    private MacAddress macAddress;
    @JsonProperty ("LinkTechnology")
    private LinkTechnology linkTechnology;
    @JsonProperty ("SpeedMbps")
    private Integer speedMbps;
    @JsonProperty ("AutoNeg")
    private Boolean autoNeg;
    @JsonProperty ("FullDuplex")
    private Boolean fullDuplex;
    @JsonProperty ("MTUSize")
    private Integer mtuSize;
    @JsonProperty("IPv6DefaultGateway")
    private String ipV6DefaultGateway;
    @JsonProperty ("MaxIPv6StaticAddresses")
    private Integer maxIPv6StaticAddresses;
    @JsonProperty("IPv4Addresses")
    private List<IpV4AddressObjectImpl> ipV4Addresses;
    @JsonProperty("IPv6Addresses")
    private List<IpV6AddressObjectImpl> ipV6Addresses;
    @JsonProperty("IPv6StaticAddresses")
    private List<IpV6AddressObjectImpl> ipV6StaticAddresses;
    @JsonProperty("IPv6AddressPolicyTable")
    private List<IpV6AddressPolicyObjectImpl> ipV6AddressesPolicies;
    @JsonProperty("NameServers")
    private List<String> nameServers;
    @JsonProperty("VLAN")
    private Vlan vlan;
    @JsonProperty("VLANs")
    private ODataId vlanCollection;

    @Override
    public String getFqdn() {
        return fqdn;
    }

    @Override
    public String getHostName() {
        return hostName;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public MacAddress getPermanentMacAddress() {
        return permanentMacAddress;
    }

    @Override
    public MacAddress getMacAddress() {
        return macAddress;
    }


    @Override
    public Integer getSpeedMbps() {
        return speedMbps;
    }

    @Override
    public Boolean isFullDuplex() {
        return fullDuplex;
    }

    @Override
    public String getIpV6DefaultGateway() {
        return ipV6DefaultGateway;
    }

    @Override
    public List<IpV4AddressObject> getIpV4Addresses() {
        if (ipV4Addresses == null) {
            return emptyList();
        }
        return (List) ipV4Addresses;
    }

    @Override
    public List<IpV6AddressObject> getIpV6Addresses() {
        if (ipV6Addresses == null) {
            return emptyList();
        }
        return (List) ipV6Addresses;
    }

    @Override
    public List<IpV6AddressObject> getIpV6StaticAddresses() {
        if (ipV6StaticAddresses == null) {
            return Collections.emptyList();
        }
        return (List) ipV6StaticAddresses;
    }

    public List<IpV6AddressPolicyObject> getIpV6AddressesPolicyTable() {
        if (ipV6AddressesPolicies == null) {
            return emptyList();
        }
        return (List) ipV6AddressesPolicies;
    }

    @Override
    public ResourceSupplier getComputerSystem() {
        //FIXME: workaround until link to computer system is available
        return toSupplier(getComputerSystemODataId());
    }

    @Override
    public List<String> getNameServers() {
        if (nameServers == null) {
            return emptyList();
        }
        return (List) nameServers;
    }

    @Override
    public Boolean getInterfaceEnabled() {
        return interfaceEnabled;
    }

    @Override
    public Boolean getAutoNeg() {
        return autoNeg;
    }

    @Override
    public Integer getMtuSize() {
        return mtuSize;
    }

    @Override
    public Integer getMaxIPv6StaticAddresses() {
        return maxIPv6StaticAddresses;
    }

    @Override
    public Boolean getVlanEnable() {
        if (vlan == null) {
            return false;
        }
        return vlan.vlanEnable;
    }

    @Override
    public Integer getVlanId() {
        if (vlan == null) {
            return null;
        }
        return vlan.vlanId;
    }

    @Override
    @LinkName("ethernetInterfaceVlans")
    public Iterable<ResourceSupplier> getVlans() throws ExternalServiceApiReaderException {
        return processMembersListResource(this.vlanCollection);
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    private static final class Vlan {
        @JsonProperty("VLANEnable")
        private Boolean vlanEnable;
        @JsonProperty("VLANId")
        private Integer vlanId;
    }
}
