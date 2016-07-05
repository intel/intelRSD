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
import com.intel.podm.common.types.LogicalDriveType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.VolumeMode;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.math.BigDecimal;
import java.net.URI;
import java.util.Collection;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.MASTERED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.USED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.USES;
import static com.intel.podm.business.entities.base.DomainObjectProperties.booleanProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.decimalProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.types.ComposedNodeState.ALLOCATED;
import static com.intel.podm.common.types.ComposedNodeState.ALLOCATING;
import static com.intel.podm.common.types.ComposedNodeState.ASSEMBLING;
import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.math.BigDecimal.ZERO;
import static java.util.Objects.nonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class LogicalDrive extends DomainObject implements Discoverable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<LogicalDriveType> TYPE = enumProperty("type", LogicalDriveType.class);
    public static final DomainObjectProperty<VolumeMode> MODE = enumProperty("mode", VolumeMode.class);
    public static final DomainObjectProperty<Boolean> WRITE_PROTECTED = booleanProperty("writeProtected");
    public static final DomainObjectProperty<BigDecimal> CAPACITY_GIB = decimalProperty("capacityGib");
    public static final DomainObjectProperty<String> IMAGE = stringProperty("image");
    public static final DomainObjectProperty<Boolean> BOOTABLE = booleanProperty("bootable");
    public static final DomainObjectProperty<Boolean> SNAPSHOT = booleanProperty("snapshot");

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

    public Boolean getSnapshot() {
        return getProperty(SNAPSHOT);
    }

    public void setSnapshot(Boolean snapshot) {
        setProperty(SNAPSHOT, snapshot);
    }

    public Boolean getBootable() {
        return getProperty(BOOTABLE);
    }

    public void setBootable(Boolean bootable) {
        setProperty(BOOTABLE, bootable);
    }

    public String getImage() {
        return getProperty(IMAGE);
    }

    public void setImage(String image) {
        setProperty(IMAGE, image);
    }

    public BigDecimal getCapacityGib() {
        return getProperty(CAPACITY_GIB);
    }

    public void setCapacityGib(BigDecimal capacityGib) {
        setProperty(CAPACITY_GIB, capacityGib);
    }

    public Boolean getWriteProtected() {
        return getProperty(WRITE_PROTECTED);
    }

    public void setWriteProtected(Boolean writeProtected) {
        setProperty(WRITE_PROTECTED, writeProtected);
    }

    public VolumeMode getMode() {
        return getProperty(MODE);
    }

    public void setMode(VolumeMode volumeMode) {
        setProperty(MODE, volumeMode);
    }

    public LogicalDriveType getType() {
        return getProperty(TYPE);
    }

    public void setType(LogicalDriveType type) {
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

    public void addPhysicalDrive(PhysicalDrive physicalDrive) {
        link(USES, physicalDrive);
    }

    public Collection<PhysicalDrive> getPhysicalDrives() {
        return getLinked(USES, PhysicalDrive.class);
    }

    public void addLogicalDrive(LogicalDrive logicalDrive) {
        link(USES, logicalDrive);
    }

    public Collection<LogicalDrive> getLogicalDrives() {
        return getLinked(USES, LogicalDrive.class);
    }

    public Collection<LogicalDrive> getUsedBy() {
        return getLinked(USED_BY, LogicalDrive.class);
    }

    public void addMasterDrive(LogicalDrive masterDrive) {
        if (masterDrive == null) {
            return;
        }

        link(MASTERED_BY, masterDrive);
    }

    public LogicalDrive getMasterDrive() {
        return singleOrNull(getLinked(MASTERED_BY, LogicalDrive.class));
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

    public Collection<RemoteTarget> getRemoteTargets() {
        return getLinked(USED_BY, RemoteTarget.class);
    }

    public Collection<ComposedNode> getComposedNodes() {
        return getLinked(USED_BY, ComposedNode.class);
    }

    public void addComposedNode(ComposedNode composedNode) {
        link(USED_BY, composedNode);
    }

    public BigDecimal getFreeSpaceGib() {
        Collection<LogicalDrive> usedBy = getUsedBy();
        BigDecimal freeSpaceGib = getCapacityGib();

        for (LogicalDrive logicalDrive : usedBy) {
            freeSpaceGib = freeSpaceGib.subtract(logicalDrive.getCapacityGib());
        }

        freeSpaceGib = freeSpaceGib.subtract(getSpaceReservedByComposedNodes());
        return freeSpaceGib;
    }

    private BigDecimal getSpaceReservedByComposedNodes() {
        return getComposedNodes().stream()
                .filter(composedNode -> composedNode.isInAnyOfStates(ALLOCATING, ALLOCATED, ASSEMBLING))
                .filter(composedNode -> nonNull(composedNode.getRemoteDriveCapacityGib()))
                .map(ComposedNode::getRemoteDriveCapacityGib)
                .reduce(ZERO, BigDecimal::add);
    }

    public void use(LogicalDrive logicalDrive) {
        link(USES, logicalDrive);
    }
}
