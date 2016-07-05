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

package com.intel.podm.business.entities.redfish.properties;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.common.types.IpV4AddressOrigin;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class IpV4Address extends DomainObject {
    public static final DomainObjectProperty<String> ADDRESS = stringProperty("address");
    public static final DomainObjectProperty<String> SUBNET_MASK = stringProperty("subnetMask");
    public static final DomainObjectProperty<IpV4AddressOrigin> ADDRESS_ORIGIN = enumProperty("addressOrigin", IpV4AddressOrigin.class);
    public static final DomainObjectProperty<String> GATEWAY = stringProperty("gateway");

    public String getAddress() {
        return getProperty(ADDRESS);
    }

    public void setAddress(String address) {
        setProperty(ADDRESS, address);
    }

    public String getSubnetMask() {
        return getProperty(SUBNET_MASK);
    }

    public void setSubnetMask(String subnetMask) {
        setProperty(SUBNET_MASK, subnetMask);
    }

    public IpV4AddressOrigin getAddressOrigin() {
        return getProperty(ADDRESS_ORIGIN);
    }

    public void setAddressOrigin(IpV4AddressOrigin addressOrigin) {
        setProperty(ADDRESS_ORIGIN, addressOrigin);
    }

    public String getGateway() {
        return getProperty(GATEWAY);
    }

    public void setGateway(String gateway) {
        setProperty(GATEWAY, gateway);
    }
}
