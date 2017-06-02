/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.actions;

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.actions.RemoteTargetResourceActions;
import com.intel.podm.client.api.actions.RemoteTargetResourceActionsFactory;
import com.intel.podm.client.api.resources.redfish.RemoteTargetResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.RemoteTargetUpdateDefinition;
import com.intel.podm.mappers.redfish.RemoteTargetMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requires;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class RemoteTargetUpdateInvoker {

    @Inject
    private RemoteTargetResourceActionsFactory remoteTargetResourceActionsFactory;

    @Inject
    private RemoteTargetMapper mapper;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void updateRemoteTarget(RemoteTarget remoteTarget, RemoteTargetUpdateDefinition remoteTargetUpdateDefinition)
        throws ActionException {

        ExternalService service = remoteTarget.getService();
        requires(service != null, "There is no Service associated with selected Remote Target");

        URI baseUri = service.getBaseUri();
        URI remoteTargetSourceUri = remoteTarget.getSourceUri();

        try (RemoteTargetResourceActions actions = remoteTargetResourceActionsFactory.create(baseUri)) {
            RemoteTargetResource updatedTarget = actions.update(remoteTargetSourceUri, remoteTargetUpdateDefinition);
            mapper.map(updatedTarget, remoteTarget);
        } catch (ExternalServiceApiActionException e) {
            String errorMessage = "Update on selected Remote Target failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), remoteTargetSourceUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        } catch (ExternalServiceApiReaderException e) {
            String errorMessage = "Update action was successful, but refreshing selected Remote Target failed";
            logger.i(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), remoteTargetSourceUri);
            throw new ActionException(errorMessage, e.getErrorResponse(), e);
        }
    }

}
