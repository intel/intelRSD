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

package com.intel.podm.business.entities.redfish.components;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Device;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.common.types.ComposedNodeState;
import com.intel.podm.common.types.NodeSystemType;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.math.BigDecimal;
import java.util.Collection;

import static com.intel.podm.business.entities.base.DomainObjectLink.INCLUDES;
import static com.intel.podm.business.entities.base.DomainObjectProperties.decimalProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.common.types.Health.Critical;
import static com.intel.podm.common.types.State.OFFLINE;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.util.Arrays.stream;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
public class ComposedNode extends DomainObject implements StatusPossessor, Descriptable {
    public static final Status OFFLINE_CRITICAL_STATUS = new Status(OFFLINE, Critical, null);

    public static final DomainObjectProperty<NodeSystemType> SYSTEM_TYPE = enumProperty("systemType", NodeSystemType.class);
    public static final DomainObjectProperty<ComposedNodeState> COMPOSED_NODE_STATE = enumProperty("composedNodeState", ComposedNodeState.class);
    public static final DomainObjectProperty<BigDecimal> REMOTE_DRIVE_CAPACITY_GIB = decimalProperty("remoteDriveCapacityGib");

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

    public NodeSystemType getSystemType() {
        return getProperty(SYSTEM_TYPE);
    }

    public void setSystemType(NodeSystemType systemType) {
        setProperty(SYSTEM_TYPE, systemType);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public ComposedNodeState getComposedNodeState() {
        return getProperty(COMPOSED_NODE_STATE);
    }

    public void setComposedNodeState(ComposedNodeState composedNodeState) {
        setProperty(COMPOSED_NODE_STATE, composedNodeState);
    }

    public boolean isInAnyOfStates(ComposedNodeState... states) {
        ComposedNodeState actualComposedNodeState = getComposedNodeState();
        return actualComposedNodeState != null
                && stream(states).anyMatch(expectedComposedNodeState -> actualComposedNodeState == expectedComposedNodeState);
    }

    public ComputerSystem getComputerSystem() {
        return singleOrNull(getLinked(INCLUDES, ComputerSystem.class));
    }

    public void setComputerSystem(ComputerSystem computerSystem) {
        link(INCLUDES, computerSystem);
    }

    public Collection<EthernetInterface> getEthernetInterfaces() {
        return getLinked(INCLUDES, EthernetInterface.class);
    }

    public void addEthernetInterface(EthernetInterface ethernetInterface) {
        link(INCLUDES, ethernetInterface);
    }

    public Collection<Device> getLocalDrives() {
        return getLinked(INCLUDES, Device.class);
    }

    public void addLocalDrive(Device localDrive) {
        link(INCLUDES, localDrive);
    }

    public Collection<SimpleStorage> getSimpleStorages() {
        return getLinked(INCLUDES, SimpleStorage.class);
    }

    public void addSimpleStorage(SimpleStorage simpleStorage) {
        link(INCLUDES, simpleStorage);
    }

    public Collection<RemoteTarget> getRemoteDrives() {
        return getLinked(INCLUDES, RemoteTarget.class);
    }

    public void addRemoteDrive(RemoteTarget remoteTarget) {
        link(INCLUDES, remoteTarget);
    }

    public BigDecimal getRemoteDriveCapacityGib() {
        return getProperty(REMOTE_DRIVE_CAPACITY_GIB);
    }

    public void setRemoteDriveCapacityGib(BigDecimal remoteDriveCapacityGib) {
        setProperty(REMOTE_DRIVE_CAPACITY_GIB, remoteDriveCapacityGib);
    }
}
