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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.properties.IpV4Address;
import com.intel.podm.business.entities.redfish.properties.IpV6Address;
import com.intel.podm.business.entities.redfish.properties.IpV6AddressPolicy;
import com.intel.podm.business.entities.redfish.properties.NameServer;
import com.intel.podm.client.api.resources.redfish.EthernetInterfaceResource;
import com.intel.podm.client.api.resources.redfish.IpV4AddressObject;
import com.intel.podm.client.api.resources.redfish.IpV6AddressObject;
import com.intel.podm.client.api.resources.redfish.IpV6AddressPolicyObject;
import com.intel.podm.mappers.DomainObjectMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static java.util.stream.Collectors.toSet;

@Dependent
public class EthernetInterfaceMapper extends DomainObjectMapper<EthernetInterfaceResource, EthernetInterface> {

    @Inject
    private IpAddressMapperHelper ipAddressMapperHelper;

    @Inject
    private GenericDao genericDao;

    public EthernetInterfaceMapper() {
        super(EthernetInterfaceResource.class, EthernetInterface.class);
        registerProvider(IpV4Address.class, this::provideIpV4Address);
        registerProvider(IpV6Address.class, this::provideIpV6Address);
        registerProvider(IpV6AddressPolicy.class, this::provideIpV6AddressPolicy);
        registerProvider(NameServer.class, this::provideNameServer);
    }

    private IpV4Address provideIpV4Address(IpV4AddressObject address) {
        return ipAddressMapperHelper.provideIpV4Address(address, target.getIpV4Addresses());
    }

    private IpV6Address provideIpV6Address(IpV6AddressObject address) {
        return ipAddressMapperHelper.provideIpV6Address(address, getIpV6AddressTargetCollection(address));
    }

    private Collection<IpV6Address> getIpV6AddressTargetCollection(IpV6AddressObject address) {
        return source.getIpV6StaticAddresses().contains(address)
                ? target.getIpV6StaticAddresses()
                : target.getIpV6Addresses();
    }

    private IpV6AddressPolicy provideIpV6AddressPolicy(IpV6AddressPolicyObject addressPolicy) {
        for (IpV6AddressPolicy policy : target.getIpV6AddressesPolicyTable()) {
            if (Objects.equals(policy.getLabel(), addressPolicy.getLabel())
                    && Objects.equals(policy.getPrecedence(), addressPolicy.getPrecedence())
                    && Objects.equals(policy.getPrefix(), addressPolicy.getPrefix())) {
                return policy;
            }
        }
        return genericDao.create(IpV6AddressPolicy.class);
    }

    @Override
    protected void performNotAutomatedMapping(EthernetInterfaceResource source, EthernetInterface target) {
        super.performNotAutomatedMapping(source, target);
        ipAddressMapperHelper.cleanIpV4Addresses(source.getIpV4Addresses(), target.getIpV4Addresses());
        ipAddressMapperHelper.cleanIpV6Addresses(source.getIpV6Addresses(), target.getIpV6Addresses());
        ipAddressMapperHelper.cleanIpV6Addresses(source.getIpV6StaticAddresses(), target.getIpV6StaticAddresses());
        cleanIpV6AddressPolicies(source, target);
        cleanNameServers(source, target);
    }

    private void cleanIpV6AddressPolicies(EthernetInterfaceResource source, EthernetInterface target) {
        Set<String> existingPolicies = source.getIpV6AddressesPolicyTable().stream()
                .map(policy -> getAddressPolicyKey(policy.getPrefix(), policy.getLabel(), policy.getPrecedence()))
                .collect(toSet());

        target.getIpV6AddressesPolicyTable().stream().
                filter(policy -> {
                    String addressPolicyKey = getAddressPolicyKey(policy.getPrefix(), policy.getLabel(), policy.getPrecedence());
                    return !existingPolicies.contains(addressPolicyKey);
                }).
                forEach(genericDao::remove);
    }

    private String getAddressPolicyKey(String prefix, Integer label, Integer precedence) {
        return label + prefix + precedence;
    }

    private NameServer provideNameServer(String sourceNameServer) {
        for (NameServer nameServer : target.getNameServers()) {
            if (nameServer.getNameServer().equals(sourceNameServer)) {
                return nameServer;
            }
        }

        NameServer nameServer = genericDao.create(NameServer.class);
        nameServer.setNameServer(sourceNameServer);
        return nameServer;
    }

    private void cleanNameServers(EthernetInterfaceResource source, EthernetInterface target) {
        Set<String> existingNameServers = new HashSet<>(source.getNameServers());
        target.getNameServers().stream().
                filter(nameServer -> !existingNameServers.contains(nameServer.getNameServer())).
                forEach(genericDao::remove);
    }

}
