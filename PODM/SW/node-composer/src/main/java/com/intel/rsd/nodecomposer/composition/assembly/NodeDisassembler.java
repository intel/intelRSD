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

package com.intel.rsd.nodecomposer.composition.assembly;

import com.intel.rsd.nodecomposer.business.ODataIdResolvingException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.ChangeTpmStateTaskFactory;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.CleanUpVlanDisassembleTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.ComposedNodeDeletionTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.ComputerSystemDisassembleTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.DeallocatePcieDriveTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.FabricDisassemblyTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeRemovalTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.PowerOffDisassemblyTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.SecureEraseDisassemblyTaskFactory;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.SecureErasePcieDriveTask;
import com.intel.rsd.nodecomposer.persistence.EntityNotFoundException;
import com.intel.rsd.nodecomposer.persistence.NonUniqueResultException;
import com.intel.rsd.nodecomposer.persistence.dao.ComposedNodeDao;
import com.intel.rsd.nodecomposer.persistence.dao.EthernetSwitchPortDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetInterface;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPort;
import com.intel.rsd.nodecomposer.persistence.redfish.Memory;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.TrustedModule;
import com.intel.rsd.nodecomposer.types.actions.ChangeTpmStatusUpdateDefinition;
import com.intel.rsd.nodecomposer.types.net.MacAddress;
import com.intel.rsd.nodecomposer.utils.beans.NodeComposerBeanFactory;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.types.MemoryType.OPTANE;
import static com.intel.rsd.nodecomposer.types.State.ENABLED;
import static com.intel.rsd.nodecomposer.utils.Collections.nullOrEmpty;
import static java.lang.Boolean.FALSE;
import static java.lang.Boolean.TRUE;
import static java.lang.String.format;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class NodeDisassembler {
    private final NodeComposerBeanFactory beanFactory;
    private final ComposedNodeDao composedNodeDao;
    private final EthernetSwitchPortDao ethernetSwitchPortDao;
    private final ChangeTpmStateTaskFactory changeTpmStateTaskFactory;
    private final SecureEraseDisassemblyTaskFactory secureEraseDisassemblyTaskFactory;
    private final Boolean forceOffEnabled;

    @Autowired
    public NodeDisassembler(@Value("${disassembly.node.forceOff:false}") Boolean forceOffEnabled,
                            NodeComposerBeanFactory beanFactory, ComposedNodeDao composedNodeDao, EthernetSwitchPortDao ethernetSwitchPortDao,
                            ChangeTpmStateTaskFactory changeTpmStateTaskFactory, SecureEraseDisassemblyTaskFactory secureEraseDisassemblyTaskFactory) {
        this.beanFactory = beanFactory;
        this.composedNodeDao = composedNodeDao;
        this.ethernetSwitchPortDao = ethernetSwitchPortDao;
        this.changeTpmStateTaskFactory = changeTpmStateTaskFactory;
        this.secureEraseDisassemblyTaskFactory = secureEraseDisassemblyTaskFactory;
        this.forceOffEnabled = forceOffEnabled;
    }

    @Transactional(value = REQUIRES_NEW, rollbackOn = ODataIdResolvingException.class)
    public List<NodeTask> getDisassemblyTasks(ODataId composedNodeODataId) throws ODataIdResolvingException {
        ComposedNode composedNode = resolveComposedNode(composedNodeODataId);
        List<NodeTask> tasks = new ArrayList<>();

        tasks.addAll(preparePcieDriveSecureEraseTasks(composedNode));
        tasks.addAll(preparePcieDriveDeallocationTasks(composedNode));
        tasks.add(beanFactory.create(FabricDisassemblyTask.class));
        if (composedNode.getComputerSystem() != null) {
            tasks.addAll(prepareComputerSystemRelatedTasks(composedNode));
        }
        if (forceOffEnabled) {
            tasks.add(beanFactory.create(PowerOffDisassemblyTask.class));
        }
        tasks.add(beanFactory.create(ComposedNodeDeletionTask.class));

        tasks.forEach(t -> t.setComposedNodeODataId(composedNode.getUri()));
        return tasks;
    }

    private Collection<NodeTask> prepareComputerSystemRelatedTasks(ComposedNode composedNode) {
        ComputerSystem computerSystem = composedNode.getComputerSystem();
        Collection<NodeTask> tasks = new ArrayList<>();

        if (composedNode.isClearTpmOnDelete() && containsEnabledTrustedModule(computerSystem)) {
            tasks.add(changeTpmStateTaskFactory.createChangeTpmStateTask(computerSystem.getUri(), new ChangeTpmStatusUpdateDefinition(TRUE)));
        }
        if (composedNode.isClearOptanePersistentMemoryOnDelete() && containsOptaneMemoryModule(computerSystem)) {
            tasks.add(secureEraseDisassemblyTaskFactory.createSecureEraseDisassemblyTask(computerSystem.getUri(), TRUE, FALSE));
        }
        tasks.add(beanFactory.create(ComputerSystemDisassembleTask.class));
        tasks.addAll(prepareCleanVlansTasks(composedNode.getEthernetInterfaces()));
        return tasks;
    }

    private ComposedNode resolveComposedNode(ODataId composedNodeODataId) throws ODataIdResolvingException {
        try {
            return composedNodeDao.find(composedNodeODataId);
        } catch (EntityNotFoundException e) {
            throw new ODataIdResolvingException(e.getMessage(), composedNodeODataId, e);
        }
    }

    private Collection<? extends NodeRemovalTask> prepareCleanVlansTasks(Set<EthernetInterface> ethernetInterfaces) {
        return ethernetInterfaces.stream()
            .map(EthernetInterface::getMacAddress)
            .map(this::getEthernetSwitchPortFromMac)
            .filter(Objects::nonNull)
            .map(EthernetSwitchPort::getUri)
            .map(id -> beanFactory.create(CleanUpVlanDisassembleTask.class).init(id))
            .collect(toList());
    }

    private boolean containsEnabledTrustedModule(ComputerSystem computerSystem) {
        List<TrustedModule> trustedModules = computerSystem.getTrustedModules();
        if (nullOrEmpty(trustedModules)) {
            return false;
        } else {
            return trustedModules.stream().anyMatch(trustedModule -> ENABLED.equals(trustedModule.getStatus().getState()));
        }
    }

    private boolean containsOptaneMemoryModule(ComputerSystem system) {
        Set<Memory> memoryModules = system.getMemoryModules();
        if (nullOrEmpty(memoryModules)) {
            return false;
        } else {
            return memoryModules.stream().anyMatch(memoryModule -> OPTANE.equals(memoryModule.getMemoryType()));
        }
    }

    private Collection<? extends NodeTask> preparePcieDriveSecureEraseTasks(ComposedNode composedNode) {
        return composedNode.getDrives().stream()
            .map(drive -> beanFactory.create(SecureErasePcieDriveTask.class).setPcieDriveODataId(drive.getUri()))
            .collect(toList());
    }

    private Collection<? extends NodeRemovalTask> preparePcieDriveDeallocationTasks(ComposedNode composedNode) {
        return composedNode.getDrives().stream()
            .map(pcieDrive -> beanFactory.create(DeallocatePcieDriveTask.class).setDriveODataId(pcieDrive.getUri()))
            .collect(toList());
    }

    private EthernetSwitchPort getEthernetSwitchPortFromMac(MacAddress mac) {
        try {
            return ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(mac);
        } catch (NonUniqueResultException e) {
            log.error(format("Could not get neighbor switch port from mac address '%s'.", mac), e);
            return null;
        }
    }
}
