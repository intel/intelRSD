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
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.NetworkInterface;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTask;

import java.util.List;
import java.util.Set;

import static org.apache.commons.collections.CollectionUtils.isEmpty;
import static org.apache.commons.collections.CollectionUtils.isNotEmpty;

public interface RemoteDriveAllocationStrategy {
    Violations validate();
    Violations findResources();
    void allocate(ComposedNode composedNode);
    List<NodeTask> getTasks();

    default boolean networkDeviceFunctionPatchCanBePerformed(ComposedNode composedNode) {
        Set<NetworkInterface> networkInterfaces = composedNode.getComputerSystem().getNetworkInterfaces();
        return isNotEmpty(networkInterfaces)
            && allNetworkInterfacesContainsNetworkDeviceFunction(networkInterfaces);
    }

    default boolean allNetworkInterfacesContainsNetworkDeviceFunction(Set<NetworkInterface> networkInterfaces) {
        return networkInterfaces.stream()
            .noneMatch(networkInterface -> isEmpty(networkInterface.getNetworkDeviceFunctions()));
    }
}
