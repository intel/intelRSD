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
import com.intel.rsd.nodecomposer.business.redfish.services.actions.RestRequestInvoker;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.actions.ChangeTpmStateRequest;
import com.intel.rsd.nodecomposer.persistence.dao.ComputerSystemDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.types.actions.ChangeTpmStatusUpdateDefinition;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import lombok.Setter;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.io.Serializable;
import java.net.URI;

import static com.intel.rsd.nodecomposer.types.redfish.ActionsResourceNames.ACTIONS_OEM_CHANGE_TPM_STATE;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;
import static org.springframework.web.util.UriComponentsBuilder.fromUri;

@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
public class ChangeTpmStateTask extends NodeTask implements Serializable {
    private static final long serialVersionUID = -235711294955253319L;

    @Autowired
    private transient ComputerSystemDao computerSystemDao;
    @Autowired
    private transient RestRequestInvoker restRequestInvoker;

    @Setter
    private ChangeTpmStatusUpdateDefinition changeTpmStatusUpdateDefinition;
    private ODataId computerSystemODataId;

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        try {
            ComputerSystem computerSystem = computerSystemDao.find(computerSystemODataId);
            URI requestUri = fromUri(computerSystem.getUri().toUri()).pathSegment(ACTIONS_OEM_CHANGE_TPM_STATE).build().toUri();
            restRequestInvoker.post(requestUri, new ChangeTpmStateRequest(changeTpmStatusUpdateDefinition));
        } catch (EntityOperationException e) {
            throw new RuntimeException("Change Tpm State failed on Computer system action", e);
        }
    }

    public ChangeTpmStateTask init(ODataId computerSystemODataId) {
        this.computerSystemODataId = computerSystemODataId;
        return this;
    }
}
