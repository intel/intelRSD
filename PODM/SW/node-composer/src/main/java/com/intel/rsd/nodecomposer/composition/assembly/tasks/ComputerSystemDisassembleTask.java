/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.ComputerSystemUpdateInvoker;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.types.actions.ComputerSystemUpdateDefinition;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.io.Serializable;

import static java.lang.Boolean.FALSE;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
public class ComputerSystemDisassembleTask extends NodeRemovalTask implements Serializable {
    private static final long serialVersionUID = -6276494858804777784L;

    @Autowired
    private transient GenericDao genericDao;
    @Autowired
    private transient ComputerSystemUpdateInvoker computerSystemUpdateInvoker;

    @Override
    public void deallocate() {
        ComputerSystem computerSystem = getComputerSystem(findComposedNode());
        if (computerSystem == null) {
            log.error("Couldn't deallocate computerSystem for nodeId={}, computerSystem is null", composedNodeODataId);
            return;
        }
        computerSystem.getMetadata().setAllocated(false);
        computerSystem.getProcessors().forEach(processor -> processor.setAllocated(false));
    }

    @Override
    public void disassemble() {
        ComposedNode composedNode = findComposedNode();
        ComputerSystem computerSystem = getComputerSystem(composedNode);
        if (composedNode == null || computerSystem == null) {
            log.error("Couldn't disassemble computerSystem for nodeId={}, computerSystem or composedNode is null", composedNodeODataId);
            return;
        }
        enableFirmwareUpdateOnComputerSystem(computerSystem);
    }

    private ComposedNode findComposedNode() {
        return genericDao.tryFind(ComposedNode.class, composedNodeODataId).orElse(null);
    }

    private ComputerSystem getComputerSystem(ComposedNode composedNode) {
        if (composedNode != null) {
            return composedNode.getComputerSystem();
        }

        return null;
    }

    private void enableFirmwareUpdateOnComputerSystem(ComputerSystem computerSystem) {
        if (computerSystem.getUserModeEnabled() != null) {
            ComputerSystemUpdateDefinition computerSystemUpdateDefinition = new ComputerSystemUpdateDefinition();
            computerSystemUpdateDefinition.setUserModeEnabled(FALSE);
            try {
                computerSystemUpdateInvoker.updateComputerSystem(computerSystem, computerSystemUpdateDefinition);
            } catch (EntityOperationException e) {
                throw new RuntimeException("Enabling firmware update failed on computer system", e);
            }
        }
    }
}
