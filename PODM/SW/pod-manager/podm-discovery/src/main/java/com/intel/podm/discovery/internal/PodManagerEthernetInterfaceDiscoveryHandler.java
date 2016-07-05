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

package com.intel.podm.discovery.internal;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.properties.IpV4Address;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Status;
import com.intel.podm.config.network.NetworkConfigurationIOException;
import com.intel.podm.config.network.NetworkConfigurationReader;
import com.intel.podm.config.network.dto.EthernetInterfaceDto;
import com.intel.podm.config.network.dto.NetworkIpV4AddressDto;
import com.intel.podm.config.network.dto.VlanEthernetInterfaceDto;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.State.ENABLED;

@Dependent
public class PodManagerEthernetInterfaceDiscoveryHandler {

    @Inject
    Logger logger;

    @Inject
    private NetworkConfigurationReader networkConfigurationReader;

    @Inject
    private NetworkParametersReader networkParametersReader;

    @Inject
    private GenericDao genericDao;

    public void addNetworkInterfaces(Manager manager) {
        for (EthernetInterfaceDto interfaceDto : networkParametersReader.discoverEnabledSystemNetworkInterfaces()) {
            addNetworkInterface(manager, interfaceDto);
        }
    }

    private void addNetworkInterface(Manager manager, EthernetInterfaceDto fromSystem) {
        EthernetInterface ethernetInterface = genericDao.create(EthernetInterface.class);
        ethernetInterface.setName(fromSystem.getName());
        ethernetInterface.setDescription(fromSystem.getDescription());
        ethernetInterface.setMacAddress(fromSystem.getMacAddress());
        ethernetInterface.setStatus(new Status(fromSystem.getState(), fromSystem.getHealth(), null));
        ethernetInterface.setIpV6DefaultGateway(fromSystem.getIpV6DefaultGateway());

        addNetworkIpV4Addresses(fromSystem, ethernetInterface);
        addNetworkInterfaceVlans(fromSystem, ethernetInterface);

        enhanceFromConfig(ethernetInterface);
        manager.addEthernetInterface(ethernetInterface);
    }

    private void addNetworkInterfaceVlans(EthernetInterfaceDto fromSystem, EthernetInterface ethernetInterface) {
        for (VlanEthernetInterfaceDto vlan : fromSystem.getVlans()) {
            EthernetSwitchPortVlan ethernetSwitchPortVlan = genericDao.create(EthernetSwitchPortVlan.class);
            ethernetSwitchPortVlan.setName(vlan.getName());
            ethernetSwitchPortVlan.setDescription(vlan.getDescription());
            ethernetSwitchPortVlan.setVlanId(vlan.getVlanId());
            ethernetSwitchPortVlan.setVlanEnable(vlan.getVlanEnable());
            ethernetSwitchPortVlan.setTagged(vlan.getTagged());
            ethernetSwitchPortVlan.setStatus(new Status(ENABLED, OK, null));

            ethernetInterface.addVlan(ethernetSwitchPortVlan);
        }
    }

    private void addNetworkIpV4Addresses(EthernetInterfaceDto fromSystem, EthernetInterface ethernetInterface) {
        for (NetworkIpV4AddressDto dto : fromSystem.getIpV4Addresses()) {
            IpV4Address ipAddress = ethernetInterface.addIpV4Address();
            ipAddress.setAddress(dto.getAddress());
        }
    }

    private void enhanceFromConfig(EthernetInterface ethernetInterface) {
        try {
            EthernetInterfaceDto fromConfig = networkConfigurationReader.readConfiguration("interface-" + ethernetInterface.getName(),
                    EthernetInterfaceDto.class);

            ethernetInterface.setStatus(new Status(fromConfig.getState(), fromConfig.getHealth(), null));
            ethernetInterface.setPermanentMacAddress(fromConfig.getFactoryMacAddress());
            ethernetInterface.setFqdn(fromConfig.getFqdn());
            ethernetInterface.setAutoNeg(fromConfig.getAutosense());
            ethernetInterface.setFullDuplex(fromConfig.getFullDuplex());
            ethernetInterface.setInterfaceEnabled(fromConfig.getInterfaceEnabled());
            ethernetInterface.setHostName(fromConfig.getHostName());
            ethernetInterface.setSpeedMbps(fromConfig.getSpeedMbps());
            ethernetInterface.setIpV6DefaultGateway(fromConfig.getIpV6DefaultGateway());
            ethernetInterface.setMaxIPv6StaticAddresses(fromConfig.getMaxIpV6StaticAddresses());
            ethernetInterface.setMtuSize(fromConfig.getFrameSize());
            enhanceIpV4AddressesFromConfig(ethernetInterface, fromConfig);
        } catch (NetworkConfigurationIOException e) {
            logger.e("Pod manager network interface {} configuration is unavailable", ethernetInterface.getName());
        }
    }

    private void enhanceIpV4AddressesFromConfig(EthernetInterface ethernetInterface, EthernetInterfaceDto fromConfig) {
        ethernetInterface.getIpV4Addresses()
                .stream()
                .filter(ipAddress -> Objects.equals(ipAddress.getAddress(), fromConfig.getIpV4Address().getAddress()))
                .forEach(ipAddress -> {
                    ipAddress.setGateway(fromConfig.getIpV4Address().getGateway());
                    ipAddress.setSubnetMask(fromConfig.getIpV4Address().getSubnetMask());
        });
    }
}
