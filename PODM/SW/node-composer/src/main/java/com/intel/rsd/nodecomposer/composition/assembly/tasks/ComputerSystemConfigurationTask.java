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
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.dao.ComputerSystemDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.types.actions.ComputerSystemUpdateDefinition;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import lombok.Setter;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.io.Serializable;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
public class ComputerSystemConfigurationTask extends NodeTask implements Serializable {
    private static final long serialVersionUID = 7175212874892883268L;

    @Autowired
    private transient ComputerSystemUpdateInvoker computerSystemUpdateInvoker;
    @Autowired
    private transient ComputerSystemDao computerSystemDao;

    @Setter
    private ComputerSystemUpdateDefinition computerSystemUpdateDefinition;
    @Setter
    private ODataId computerSystemODataId;

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @Transactional(REQUIRES_NEW)
    public void run() {
        try {
            ComputerSystem computerSystem = computerSystemDao.find(computerSystemODataId);
            computerSystemUpdateInvoker.updateComputerSystem(computerSystem, computerSystemUpdateDefinition);
        } catch (EntityOperationException e) {
            throw new RuntimeException("Assembly failed on Computer system action", e);
        }
    }
}
