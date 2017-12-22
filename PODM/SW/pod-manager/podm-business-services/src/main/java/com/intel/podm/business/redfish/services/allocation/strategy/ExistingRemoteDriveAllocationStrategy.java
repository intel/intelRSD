/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.strategy;

import com.intel.podm.business.Violations;
import com.intel.podm.business.entities.dao.RemoteTargetIscsiAddressDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.RemoteTargetIscsiAddress;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTask;
import com.intel.podm.business.redfish.services.assembly.tasks.PatchNetworkDeviceFunctionAssemblyTaskFactory;
import com.intel.podm.business.services.redfish.requests.RequestedNode;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.List;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class ExistingRemoteDriveAllocationStrategy implements RemoteDriveAllocationStrategy {
    @Inject
    private RemoteTargetIscsiAddressDao remoteTargetIscsiAddressDao;

    @Inject
    private ExistingRemoteDriveResourcesFinder finder;

    @Inject
    private PatchNetworkDeviceFunctionAssemblyTaskFactory networkDeviceFunctionTaskFactory;

    private RequestedNode.RemoteDrive drive;
    private ExistingRemoteDriveResourcesFinder.ExistingRemoteDriveAllocationResources resources;
    private List<NodeTask> tasks = new ArrayList<>();

    public void setDrive(RequestedNode.RemoteDrive drive) {
        this.drive = drive;
    }

    @Override
    public Violations validate() {
        Violations violations = new Violations();

        if (drive.getIscsiAddress() == null) {
            violations.addMissingPropertyViolation("iSCSIAddress");
        }

        RemoteTargetIscsiAddress target;
        try {
            target = remoteTargetIscsiAddressDao.getRemoteTargetIscsiAddressByTargetIqn(drive.getIscsiAddress());
        } catch (IllegalStateException e) {
            violations.addViolation("Specified remote target (" + drive.getIscsiAddress() + ") does not exist.");
            return violations;
        }

        validateRemoteTarget(target, violations);

        return violations;
    }

    private void validateRemoteTarget(RemoteTargetIscsiAddress target, Violations violations) {
        RemoteTarget remoteTarget = target.getRemoteTarget();

        boolean anyDriveIsNotBootable = remoteTarget.getLogicalDrives().stream().anyMatch(drive -> !drive.getBootable());
        if (anyDriveIsNotBootable) {
            violations.addViolation("All drives should be bootable.");
        }

        if (!remoteTarget.isEnabledAndHealthy()) {
            violations.addViolation("Specified remote target should be enabled and healthy in order to invoke actions on it.");
        }
    }

    @Override
    public Violations findResources() {
        resources = finder.find(drive);
        return resources.getViolations();
    }

    @Override
    public void allocate(ComposedNode composedNode) {
        RemoteTarget remoteTarget = resources.getRemoteTarget();
        composedNode.addRemoteTarget(remoteTarget);
        composedNode.setAssociatedStorageServiceUuid(remoteTarget.getService().getUuid());
        composedNode.setAssociatedRemoteTargetIqn(remoteTarget.getTargetIqn());
        remoteTarget.getMetadata().setAllocated(true);

        addPatchNetworkDeviceFunctionAssemblyTaskIfNeeded(composedNode);
    }

    private void addPatchNetworkDeviceFunctionAssemblyTaskIfNeeded(ComposedNode composedNode) {
        if (networkDeviceFunctionPatchCanBePerformed(composedNode)) {
            tasks.add(networkDeviceFunctionTaskFactory.create());
        }
    }

    @Override
    public List<NodeTask> getTasks() {
        return tasks;
    }
}
