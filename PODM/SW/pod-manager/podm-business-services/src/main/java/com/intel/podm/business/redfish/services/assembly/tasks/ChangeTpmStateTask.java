/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.redfish.services.actions.RestRequestInvoker;
import com.intel.podm.client.actions.ChangeTpmStateRequest;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.actions.ChangeTpmStatusUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.UUID;

import static com.intel.podm.common.types.redfish.ActionsResourceNames.ACTIONS_OEM_CHANGE_TPM_STATE;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static javax.ws.rs.core.UriBuilder.fromUri;

@Dependent
public class ChangeTpmStateTask extends NodeTask {

    @Inject
    private RestRequestInvoker restRequestInvoker;

    @Inject
    private GenericDao genericDao;

    private ChangeTpmStatusUpdateDefinition changeTpmStatusUpdateDefinition;

    private Id computerSystemId;

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @Transactional(REQUIRES_NEW)
    public void run() {
        try {
            ComputerSystem computerSystem = genericDao.find(ComputerSystem.class, computerSystemId);
            requiresNonNull(computerSystem.getService(), "There is no Service associated with selected computer system");
            URI baseUri = computerSystem.getService().getBaseUri();
            URI requestUri = fromUri(computerSystem.getSourceUri()).path(ACTIONS_OEM_CHANGE_TPM_STATE).build();
            restRequestInvoker.post(baseUri, requestUri, new ChangeTpmStateRequest(changeTpmStatusUpdateDefinition));
        } catch (EntityOperationException e) {
            throw new RuntimeException("Change Tpm State failed on Computer system action", e);
        }
    }

    public void setChangeTpmStatusUpdateDefinition(ChangeTpmStatusUpdateDefinition changeTpmStatusUpdateDefinition) {
        this.changeTpmStatusUpdateDefinition = changeTpmStatusUpdateDefinition;
    }

    @Override
    @Transactional(REQUIRES_NEW)
    // synchronization on computerSystem.getUuid() in case of long-term operation
    public UUID getServiceUuid() {
        return genericDao.find(ComputerSystem.class, computerSystemId).getUuid();
    }

    public ChangeTpmStateTask init(Id computerSystemId) {
        this.computerSystemId = computerSystemId;
        return this;
    }
}
