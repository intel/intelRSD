/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.strategy;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.VolumeHelper;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.RemoteDrive;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationStrategy;
import com.intel.rsd.nodecomposer.composition.allocation.validation.ExistingRemoteDriveValidator;
import com.intel.rsd.nodecomposer.composition.assembly.IscsiAssemblyTasksProvider;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.InitiatorEndpointAssemblyTaskFactory;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.ZoneTaskFactory;
import com.intel.rsd.nodecomposer.persistence.dao.EndpointDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.List;

import static com.intel.rsd.collections.IterableHelper.any;
import static com.intel.rsd.nodecomposer.types.Protocol.ISCSI;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
@Transactional(MANDATORY)
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class ExistingRemoteDriveAllocationStrategy implements AllocationStrategy {
    private final InitiatorEndpointAssemblyTaskFactory initiatorEndpointAssemblyTaskFactory;
    private final ZoneTaskFactory zoneTaskFactory;
    private final IscsiAssemblyTasksProvider iscsiAssemblyTasksProvider;
    private final VolumeHelper volumeHelper;
    private final ExistingRemoteDriveValidator existingRemoteDriveValidator;
    private final EndpointDao endpointDao;

    private RemoteDrive drive;
    private ResourceAllocationStrategy resourceAllocationStrategy;
    private List<NodeTask> tasks = new ArrayList<>();

    @Autowired
    @SuppressWarnings({"checkstyle:ParameterNumber"})
    public ExistingRemoteDriveAllocationStrategy(ZoneTaskFactory zoneTaskFactory,
                                                 InitiatorEndpointAssemblyTaskFactory initiatorEndpointAssemblyTaskFactory,
                                                 IscsiAssemblyTasksProvider iscsiAssemblyTasksProvider, VolumeHelper volumeHelper,
                                                 ExistingRemoteDriveValidator existingRemoteDriveValidator, EndpointDao endpointDao) {
        this.endpointDao = endpointDao;
        this.volumeHelper = volumeHelper;
        this.zoneTaskFactory = zoneTaskFactory;
        this.iscsiAssemblyTasksProvider = iscsiAssemblyTasksProvider;
        this.existingRemoteDriveValidator = existingRemoteDriveValidator;
        this.initiatorEndpointAssemblyTaskFactory = initiatorEndpointAssemblyTaskFactory;
    }

    public void setResourceAllocationStrategy(ResourceAllocationStrategy resourceAllocationStrategy) {
        this.resourceAllocationStrategy = resourceAllocationStrategy;
    }

    public void setDrive(RemoteDrive drive) {
        this.drive = drive;
    }

    @Override
    public Violations validate() {
        Violations violations = new Violations();
        violations.addAll(resourceAllocationStrategy.validate());
        violations.addAll(existingRemoteDriveValidator.validateProtocolFromExistingRequestedDrive(drive));

        return violations;
    }

    @Override
    public void allocate(ComposedNode composedNode) {
        tasks.addAll(resourceAllocationStrategy.allocate(composedNode));

        ComputerSystem computerSystem = composedNode.getComputerSystem();
        Volume volume = any(composedNode.getVolumes());
        Fabric fabric = volumeHelper.retrieveFabricFromVolume(volume);

        if (endpointDao.findInitiatorEndpointBySystemAndFabric(computerSystem.getUri(), fabric.getUri()) == null) {
            tasks.add(initiatorEndpointAssemblyTaskFactory.create(fabric.getUri()));
        }
        if (computerSystem.hasNetworkInterfaceWithNetworkDeviceFunction() && ISCSI.equals(fabric.getFabricType())) {
            tasks.addAll(iscsiAssemblyTasksProvider.createTasks(volume.getBootable()));
        }
        tasks.add(zoneTaskFactory.create(fabric.getUri()));
    }

    @Override
    public List<NodeTask> getTasks() {
        return tasks;
    }
}
