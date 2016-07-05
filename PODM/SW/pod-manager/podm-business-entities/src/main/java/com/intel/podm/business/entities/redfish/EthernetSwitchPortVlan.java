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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectProperties.booleanProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class EthernetSwitchPortVlan extends DomainObject implements Discoverable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<Boolean> VLAN_ENABLE = booleanProperty("vlanEnable");
    public static final DomainObjectProperty<Boolean> TAGGED = booleanProperty("tagged");
    public static final DomainObjectProperty<Integer> VLAN_ID = integerProperty("vlanId");

    @Override
    public String getName() {
        return getProperty(NAME);
    }

    @Override
    public void setName(String name) {
        setProperty(NAME, name);
    }

    @Override
    public String getDescription() {
        return getProperty(DESCRIPTION);
    }

    @Override
    public void setDescription(String description) {
        setProperty(DESCRIPTION, description);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public Boolean getVlanEnable() {
        return getProperty(VLAN_ENABLE);
    }

    public void setVlanEnable(Boolean vlanEnable) {
        setProperty(VLAN_ENABLE, vlanEnable);
    }

    public Boolean getTagged() {
        return getProperty(TAGGED);
    }

    public void setTagged(Boolean tagged) {
        setProperty(TAGGED, tagged);
    }

    public Integer getVlanId() {
        return getProperty(VLAN_ID);
    }

    public void setVlanId(Integer vlanId) {
        setProperty(VLAN_ID, vlanId);
    }

    public EthernetSwitchPort getSwitchPort() {
        return single(getLinked(CONTAINED_BY, EthernetSwitchPort.class));
    }

    @Override
    public URI getSourceUri() {
        return getProperty(SOURCE_URI);
    }

    @Override
    public void setSourceUri(URI sourceUri) {
        setProperty(SOURCE_URI, sourceUri);
    }

    @Override
    public ExternalService getService() {
        return singleOrNull(getLinked(OWNED_BY, ExternalService.class));
    }
}
