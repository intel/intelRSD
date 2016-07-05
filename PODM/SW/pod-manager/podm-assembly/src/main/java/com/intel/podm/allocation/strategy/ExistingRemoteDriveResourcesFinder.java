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

package com.intel.podm.allocation.strategy;

import com.intel.podm.allocation.validation.Violations;
import com.intel.podm.business.dto.redfish.RequestedRemoteDrive;
import com.intel.podm.business.entities.dao.RemoteTargetIscsiAddressDao;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.properties.RemoteTargetIscsiAddress;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class ExistingRemoteDriveResourcesFinder {
    @Inject
    private RemoteTargetIscsiAddressDao remoteTargetIscsiAddressDao;

    public ExistingRemoteDriveAllocationResources find(RequestedRemoteDrive requested) {
        ExistingRemoteDriveAllocationResources resources = new ExistingRemoteDriveAllocationResources();

        RemoteTargetIscsiAddress target = remoteTargetIscsiAddressDao.getRemoteTargetIscsiAddressByTargetIqn(requested.getIscsiAddress());

        if (target == null) {
            throw new IllegalStateException("Specified remote target should be available.");
        }

        if (target.getRemoteTarget().isAllocated()) {
            resources.addViolation("Specified remote target (" + requested.getIscsiAddress() + ") is currently in use.");
        }

        resources.setRemoteTarget(target.getRemoteTarget());
        return resources;
    }

    static class ExistingRemoteDriveAllocationResources {
        RemoteTarget remoteTarget;
        Violations violations;

        ExistingRemoteDriveAllocationResources() {
            violations = new Violations();
        }

        public RemoteTarget getRemoteTarget() {
            return remoteTarget;
        }

        public void setRemoteTarget(RemoteTarget remoteTarget) {
            this.remoteTarget = remoteTarget;
        }

        public Violations getViolations() {
            return violations;
        }

        public void addViolation(String message) {
            violations.addViolation(message);
        }
    }
}
