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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.business.entities.redfish.properties.RemoteTargetIscsiAddress;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;
import java.util.Objects;

import static com.intel.podm.business.entities.base.DomainObjectLink.COLLECTS;
import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.INCLUDED;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.USES;
import static com.intel.podm.business.entities.base.DomainObjectProperties.booleanProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.lang.Boolean.TRUE;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class RemoteTarget extends DomainObject implements Discoverable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<String> TYPE = stringProperty("type");
    public static final DomainObjectProperty<String> ISCSI_INITIATOR_IQN = stringProperty("iscsiInitiatorIqn");
    public static final DomainObjectProperty<Boolean> ALLOCATED = booleanProperty("allocated");

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

    public String getType() {
        return getProperty(TYPE);
    }

    public void setType(String type) {
        setProperty(TYPE, type);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public String getIscsiInitiatorIqn() {
        return getProperty(ISCSI_INITIATOR_IQN);
    }

    public void setIscsiInitiatorIqn(String iscsiInitiatorIqn) {
        setProperty(ISCSI_INITIATOR_IQN, iscsiInitiatorIqn);
    }

    public Boolean isAllocated() {
        return Objects.equals(TRUE, getProperty(ALLOCATED));
    }

    public void setAllocated(Boolean allocated) {
        setProperty(ALLOCATED, allocated);
    }

    public StorageService getStorageService() {
        return single(getLinked(CONTAINED_BY, StorageService.class));
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

    public void addLogicalDrive(LogicalDrive logicalDrive) {
        link(USES, logicalDrive);
    }

    public List<LogicalDrive> getLogicalDrives() {
        return getLinked(USES, LogicalDrive.class);
    }

    public List<RemoteTargetIscsiAddress> getRemoteTargetIscsiAddresses() {
        return getLinked(COLLECTS, RemoteTargetIscsiAddress.class);
    }

    public void linkRemoteTargetIscsiAddresses(RemoteTargetIscsiAddress remoteTargetIscsiAddress) {
        link(COLLECTS, remoteTargetIscsiAddress);
    }

    public ComposedNode getComposedNode() {
        return singleOrNull(getLinked(INCLUDED, ComposedNode.class));
    }
}
