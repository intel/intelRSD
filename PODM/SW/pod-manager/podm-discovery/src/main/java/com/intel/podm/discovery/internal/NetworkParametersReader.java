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

import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.config.network.dto.EthernetInterfaceDto;
import com.intel.podm.config.network.dto.NetworkIpV4AddressDto;
import com.intel.podm.config.network.dto.VlanEthernetInterfaceDto;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.State.ENABLED;
import static java.util.Collections.list;

@Dependent
public class NetworkParametersReader {

    private static final String VLAN_SEPARATOR = ".";
    private static final String VLAN_SEPARATOR_REGEX = "\\.";

    @Inject
    private Logger logger;

    public Iterable<EthernetInterfaceDto> discoverEnabledSystemNetworkInterfaces() {
        Map<String, EthernetInterfaceDto> networkInterfacesMap = new HashMap<>();
        List<EthernetInterfaceDto> vlanEthernetInterfaceDtos = new ArrayList<>();
        List<NetworkInterface> networkInterfaces;

        try {
            networkInterfaces = list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface networkInterface : networkInterfaces) {
                if (shouldBeOmitted(networkInterface)) {
                    continue;
                }
                processNetworkInterface(networkInterfacesMap, vlanEthernetInterfaceDtos, networkInterface);
            }
            mapVlansToNetworkInterfaces(networkInterfacesMap, vlanEthernetInterfaceDtos);
        } catch (SocketException e) {
            logger.e("Could not discover system network interfaces", e);
        }

        return networkInterfacesMap.values();
    }

    private void processNetworkInterface(Map<String, EthernetInterfaceDto> networkInterfacesMap,
                                         List<EthernetInterfaceDto> vlanEthernetInterfaceDtos,
                                         NetworkInterface networkInterface) {
        EthernetInterfaceDto ethernetInterfaceDto = getNetworkInterfaceDto(networkInterface);

        if (isVlan(networkInterface)) {
            vlanEthernetInterfaceDtos.add(ethernetInterfaceDto);
        } else {
            EthernetInterfaceDto oldEthernetInterfaceDto = networkInterfacesMap.put(ethernetInterfaceDto.getName(), ethernetInterfaceDto);
            if (oldEthernetInterfaceDto != null) {
                logger.w("Duplicated network interface: {}", oldEthernetInterfaceDto.getName());
            }
        }
    }

    private void mapVlansToNetworkInterfaces(Map<String, EthernetInterfaceDto> networkInterfacesMap, List<EthernetInterfaceDto> vlanNetworkInterfaces) {
        for (EthernetInterfaceDto vlanNetworkInterface : vlanNetworkInterfaces) {
            String networkInterfaceName = getNetworkInterfaceNameForVlanNetworkInterface(vlanNetworkInterface);

            EthernetInterfaceDto ethernetInterfaceDto = networkInterfacesMap.get(networkInterfaceName);
            if (ethernetInterfaceDto == null) {
                logger.w("Network interface {} for Vlan network interface {} not found", networkInterfaceName, vlanNetworkInterface.getName());
            } else {
                VlanEthernetInterfaceDto vlanEthernetInterfaceDto = getVlanNetworkInterfaceDto(vlanNetworkInterface);
                ethernetInterfaceDto.getVlans().add(vlanEthernetInterfaceDto);
                ethernetInterfaceDto.getIpV4Addresses().addAll(vlanNetworkInterface.getIpV4Addresses());
            }
        }
    }

    private EthernetInterfaceDto getNetworkInterfaceDto(NetworkInterface networkInterface) {
        EthernetInterfaceDto interfaceDto = new EthernetInterfaceDto();
        interfaceDto.setName(networkInterface.getName());
        interfaceDto.setDescription(networkInterface.getDisplayName());

        interfaceDto.setVlanEnable(isVlan(networkInterface));
        interfaceDto.setVlanId(getVlanIdForNetworkInterface(networkInterface));

        interfaceDto.setFrameSize(getMtu(networkInterface));
        interfaceDto.setMacAddress(getMacAddress(networkInterface));
        interfaceDto.setIpV4Addresses(getInetAddresses(networkInterface));
        interfaceDto.setHealth(OK);
        interfaceDto.setState(ENABLED);
        return interfaceDto;
    }

    private VlanEthernetInterfaceDto getVlanNetworkInterfaceDto(EthernetInterfaceDto ethernetInterfaceDto) {
        VlanEthernetInterfaceDto vlanEthernetInterfaceDto = new VlanEthernetInterfaceDto();
        vlanEthernetInterfaceDto.setName(ethernetInterfaceDto.getName());
        vlanEthernetInterfaceDto.setDescription(ethernetInterfaceDto.getDescription());
        vlanEthernetInterfaceDto.setTagged(true);
        vlanEthernetInterfaceDto.setVlanEnable(true);
        vlanEthernetInterfaceDto.setVlanId(ethernetInterfaceDto.getVlanId());

        return vlanEthernetInterfaceDto;
    }

    private boolean shouldBeOmitted(NetworkInterface networkInterface) {
        try {
            return networkInterface.isLoopback();
        } catch (SocketException e) {
            logger.e("network interface identification failed for network interface: {}", networkInterface.getName(), e);
        }

        return false;
    }

    private List<NetworkIpV4AddressDto> getInetAddresses(NetworkInterface networkInterface) {
        List<NetworkIpV4AddressDto> inetAddressesDtos = new ArrayList<>();
        List<InetAddress> inetAddresses = list(networkInterface.getInetAddresses());


        for (InetAddress inetAddress : inetAddresses) {
            if (inetAddress instanceof Inet4Address) {
                NetworkIpV4AddressDto addressDto = new NetworkIpV4AddressDto();
                addressDto.setAddress(inetAddress.getHostAddress());
                addressDto.setGateway(null);
                addressDto.setSubnetMask(null);
                inetAddressesDtos.add(addressDto);
            }
        }

        return inetAddressesDtos;
    }

    private MacAddress getMacAddress(NetworkInterface networkInterface) {
        byte[] hardwareAddress = null;

        try {
            hardwareAddress = networkInterface.getHardwareAddress();
        } catch (SocketException e) {
            logger.e("mac reading failed for network interface: {}", networkInterface.getName(), e);
        }

        if (hardwareAddress == null) {
            return null;
        }

        return new MacAddress(hardwareAddress);
    }

    private Integer getMtu(NetworkInterface networkInterface) {
        try {
            return networkInterface.getMTU();
        } catch (SocketException e) {
            logger.e("MTU reading failed for network interface: {}", networkInterface.getName(), e);
            return null;
        }
    }

    private boolean isVlan(NetworkInterface networkInterface) {
        return networkInterface.getName().contains(VLAN_SEPARATOR);
    }

    private Integer getVlanIdForNetworkInterface(NetworkInterface networkInterface) {
        String[] parts = networkInterface.getName().split(VLAN_SEPARATOR_REGEX);

        if (parts.length > 1) {
            try {
                return Integer.valueOf(parts[1]);
            } catch (NumberFormatException e) {
                logger.e("VlanId parsing failed for network interface: {}", networkInterface.getName(), e);
                return null;
            }
        }

        return null;
    }

    private String getNetworkInterfaceNameForVlanNetworkInterface(EthernetInterfaceDto ethernetInterfaceDto) {
        String[] parts = ethernetInterfaceDto.getName().split(VLAN_SEPARATOR_REGEX);

        if (parts.length > 1) {
            return parts[0];
        } else {
            return null;
        }
    }
}
