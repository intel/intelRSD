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
import com.intel.podm.common.types.AddressState;
import com.intel.podm.common.types.IpV6AddressOrigin;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class IpV6Address extends DomainObject {
    public static final DomainObjectProperty<String> ADDRESS = stringProperty("address");
    public static final DomainObjectProperty<Integer> PREFIX_LENGTH = integerProperty("prefixLength");
    public static final DomainObjectProperty<IpV6AddressOrigin> ADDRESS_ORIGIN = enumProperty("addressOrigin", IpV6AddressOrigin.class);
    public static final DomainObjectProperty<AddressState> ADDRESS_STATE = enumProperty("addressState", AddressState.class);

    public String getAddress() {
        return getProperty(ADDRESS);
    }

    public void setAddress(String address) {
        setProperty(ADDRESS, address);
    }

    public Integer getPrefixLength() {
        return getProperty(PREFIX_LENGTH);
    }

    public void setPrefixLength(Integer prefixLength) {
        setProperty(PREFIX_LENGTH, prefixLength);
    }

    public IpV6AddressOrigin getAddressOrigin() {
        return getProperty(ADDRESS_ORIGIN);
    }

    public void setAddressOrigin(IpV6AddressOrigin addressOrigin) {
        setProperty(ADDRESS_ORIGIN, addressOrigin);
    }

    public AddressState getAddressState() {
        return getProperty(ADDRESS_STATE);
    }

    public void setAddressState(AddressState addressState) {
        setProperty(ADDRESS_STATE, addressState);
    }
}
