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
import com.intel.podm.business.entities.redfish.properties.IpV4Address;
import com.intel.podm.business.entities.redfish.properties.IpV6Address;
import com.intel.podm.client.api.resources.redfish.IpV4AddressObject;
import com.intel.podm.client.api.resources.redfish.IpV6AddressObject;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.Objects;
import java.util.Set;

import static java.util.stream.Collectors.toSet;

@Dependent
public class IpAddressMapperHelper {

    @Inject
    private GenericDao genericDao;

    public void cleanIpV4Addresses(Collection<? extends IpV4AddressObject> srcAddrs,
                                    Collection<IpV4Address> destAddrs) {
        Set<String> existingAddresses = srcAddrs.stream().map(IpV4AddressObject::getAddress).collect(toSet());
        destAddrs.forEach(address -> {
            if (!existingAddresses.contains(address.getAddress())) {
                genericDao.remove(address);
            }
        });
    }

    public void cleanIpV6Addresses(Collection<? extends IpV6AddressObject> srcAddrs,
                                    Collection<IpV6Address> destAddrs) {
        Set<String> existingAddrs = srcAddrs.stream().map(IpV6AddressObject::getAddress).collect(toSet());
        destAddrs.stream()
                .filter(address -> !existingAddrs.contains(address.getAddress()))
                .forEach(genericDao::remove);
    }

    public IpV4Address provideIpV4Address(IpV4AddressObject address,
                                                          Collection<IpV4Address> existingAddresses) {
        for (IpV4Address v4Address : existingAddresses) {
            if (Objects.equals(v4Address.getAddress(), address.getAddress())) {
                return v4Address;
            }
        }
        return genericDao.create(IpV4Address.class);
    }

    public IpV6Address provideIpV6Address(IpV6AddressObject address,
                                                          Collection<IpV6Address> existingAddresses) {
        for (IpV6Address v6Address : existingAddresses) {
            if (Objects.equals(v6Address.getAddress(), address.getAddress())
                    && Objects.equals(v6Address.getPrefixLength(), address.getPrefixLength())) {
                return v6Address;
            }
        }
        return genericDao.create(IpV6Address.class);
    }
}
