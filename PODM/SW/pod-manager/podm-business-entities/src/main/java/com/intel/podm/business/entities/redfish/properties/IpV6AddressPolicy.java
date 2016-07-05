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

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class IpV6AddressPolicy extends DomainObject {
    public static final DomainObjectProperty<String> PREFIX = stringProperty("prefix");
    public static final DomainObjectProperty<Integer> PRECEDENCE = integerProperty("precedence");
    public static final DomainObjectProperty<Integer> LABEL = integerProperty("label");

    public String getPrefix() {
        return getProperty(PREFIX);
    }

    public void setPrefix(String prefix) {
        setProperty(PREFIX, prefix);
    }

    public Integer getPrecedence() {
        return getProperty(PRECEDENCE);
    }

    public void setPrecedence(Integer precedence) {
        setProperty(PRECEDENCE, precedence);
    }

    public Integer getLabel() {
        return getProperty(LABEL);
    }

    public void setLabel(Integer label) {
        setProperty(LABEL, label);
    }
}
