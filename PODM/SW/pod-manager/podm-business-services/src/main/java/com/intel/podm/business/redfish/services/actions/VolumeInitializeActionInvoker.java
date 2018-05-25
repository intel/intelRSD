/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.actions;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.dto.actions.InitializeActionDto;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.common.types.actions.InitializeType;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.types.redfish.ActionsResourceNames.ACTIONS_VOLUME_INITIALIZE;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static javax.ws.rs.core.UriBuilder.fromUri;

@Dependent
public class VolumeInitializeActionInvoker {

    @Inject
    private RestRequestInvoker restRequestInvoker;

    @Transactional(MANDATORY)
    public void initialize(Volume volume, InitializeType initializeType) throws EntityOperationException {
        ExternalService service = volume.getService();
        requiresNonNull(service, "There is no ExternalService associated with selected Volume");

        URI baseUri = service.getBaseUri();
        URI initializeUri = fromUri(volume.getSourceUri()).path(ACTIONS_VOLUME_INITIALIZE).build();

        restRequestInvoker.post(baseUri, initializeUri, new InitializeActionDto(initializeType));
    }
}
