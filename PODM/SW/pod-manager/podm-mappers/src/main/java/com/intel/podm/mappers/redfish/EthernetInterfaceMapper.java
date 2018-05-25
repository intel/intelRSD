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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.client.resources.redfish.EthernetInterfaceResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.IpV4AddressMapper;
import com.intel.podm.mappers.subresources.IpV6AddressMapper;
import com.intel.podm.mappers.subresources.IpV6AddressPolicyTableMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class EthernetInterfaceMapper extends EntityMapper<EthernetInterfaceResource, EthernetInterface> {
    @Inject
    private IpV4AddressMapper ipV4AddressMapper;

    @Inject
    private IpV6AddressPolicyTableMapper ipV6AddressPolicyTableMapper;

    @Inject
    private IpV6AddressMapper ipV6AddressMapper;

    @Inject
    private SimpleTypeMapper simpleTypeMapper;

    public EthernetInterfaceMapper() {
        super(EthernetInterfaceResource.class, EthernetInterface.class);
    }

    @Override
    protected void performNotAutomatedMapping(EthernetInterfaceResource source, EthernetInterface target) {
        super.performNotAutomatedMapping(source, target);
        source.getIpV4Addresses().ifAssigned(ipV4Addresses ->
            ipV4AddressMapper.map(ipV4Addresses, target.getIpV4Addresses(), target::addIpV4Address)
        );
        source.getIpV6Addresses().ifAssigned(ipV6Addresses ->
            ipV6AddressMapper.map(ipV6Addresses, target.getIpV6Addresses(), target::addIpV6Address)
        );
        source.getIpV6StaticAddresses().ifAssigned(ipV6Addresses ->
            ipV6AddressMapper.map(ipV6Addresses, target.getIpV6StaticAddresses(), target::addIpV6StaticAddress)
        );
        source.getIpV6AddressesPolicyTable().ifAssigned(ipV6AddressesPolicyTable ->
            ipV6AddressPolicyTableMapper.map(ipV6AddressesPolicyTable, target.getIpV6AddressPolicyTable(), target::addIpV6Policy)
        );
        source.getNameServers().ifAssigned(nameServers ->
            simpleTypeMapper.map(nameServers, target.getNameServers(), target::addNameServer)
        );

        source.getSupportedProtocols().ifAssigned(supportedProtocols ->
            simpleTypeMapper.map(supportedProtocols, target.getSupportedProtocols(), target::addSupportedProtocol)
        );
    }
}
