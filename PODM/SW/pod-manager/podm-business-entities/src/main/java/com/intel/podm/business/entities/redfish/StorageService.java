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
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.business.entities.redfish.base.Manageable;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Collection;
import java.util.List;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINS;
import static com.intel.podm.business.entities.base.DomainObjectLink.MANAGED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.common.types.VolumeMode.LVG;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class StorageService extends DomainObject implements Discoverable, Manageable, StatusPossessor, Descriptable {
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

    public List<RemoteTarget> getRemoteTargets() {
        return getLinked(CONTAINS, RemoteTarget.class);
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

    @Override
    public Collection<Manager> getManagers() {
        return getLinked(MANAGED_BY, Manager.class);
    }

    @Override
    public void addManager(Manager manager) {
        link(MANAGED_BY, manager);
    }

    public Collection<LogicalDrive> getLogicalDrives() {
        return getLinked(CONTAINS, LogicalDrive.class);
    }

    public Collection<PhysicalDrive> getPhysicalDrives() {
        return getLinked(CONTAINS, PhysicalDrive.class);
    }

    public void addPhysicalDrive(PhysicalDrive physicalDrive) {
        link(CONTAINS, physicalDrive);
    }

    public void addLogicalDrive(LogicalDrive logicalDrive) {
        link(CONTAINS, logicalDrive);
    }

    public void addRemoteTarget(RemoteTarget remoteTarget) {
        link(CONTAINS, remoteTarget);
    }

    public List<LogicalDrive> getLogicalVolumeGroups() {
        return getLogicalDrives()
                .stream()
                .filter(ld -> ld.getMode().equals(LVG))
                .collect(toList());
    }
}
