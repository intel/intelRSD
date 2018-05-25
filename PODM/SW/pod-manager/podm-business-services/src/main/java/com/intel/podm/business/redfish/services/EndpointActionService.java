/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.DeleteActionInvoker;
import com.intel.podm.business.redfish.services.actions.EndpointUpdateInvoker;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.actions.EndpointUpdateDefinition;
import com.intel.podm.common.types.redfish.RedfishEndpoint;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.Violations.createWithViolations;
import static com.intel.podm.business.entities.redfish.base.StatusControl.statusOf;
import static com.intel.podm.common.types.Protocol.ISCSI;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EndpointActionService {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EndpointUpdateInvoker endpointUpdateInvoker;

    @Inject
    private DeleteActionInvoker actionInvoker;

    @Inject
    private EndpointAttacher endpointAttacher;

    @Inject
    private EndpointDetachService endpointDetachService;

    @Transactional(REQUIRES_NEW)
    public void updateEndpoint(Context context, RedfishEndpoint redfishEndpoint) throws BusinessApiException {
        Endpoint endpoint = (Endpoint) traverser.traverse(context);
        validateEndpoint(endpoint);

        EndpointUpdateDefinition endpointUpdateDefinition = createEndpointUpdateDefinition(redfishEndpoint);
        endpointUpdateInvoker.updateEndpoint(endpoint, endpointUpdateDefinition);
    }

    @Transactional(REQUIRES_NEW)
    void deleteEndpoint(Context context) throws EntityOperationException, ContextResolvingException {
        Endpoint endpoint = (Endpoint) traverser.traverse(context);
        actionInvoker.delete(endpoint);
    }

    @Transactional(REQUIRES_NEW)
    public void attachEndpoint(Context target, Context resourceContext) throws BusinessApiException {
        Endpoint endpoint = (Endpoint) traverser.traverse(resourceContext);
        ComposedNode composedNode = (ComposedNode) traverser.traverse(target);

        endpointAttacher.attachEndpoint(composedNode, endpoint);
    }

    @Transactional(REQUIRES_NEW)
    public void detachEndpoint(Context composedNodeContext, Context endpointContext) throws BusinessApiException {
        Endpoint endpoint = (Endpoint) traverser.traverse(endpointContext);
        ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);
        validateDetachEndpoint(endpoint, composedNode);
        endpointDetachService.detachEndpoint(composedNode, endpoint);
    }

    private void validateDetachEndpoint(Endpoint endpoint, ComposedNode composedNode) throws RequestValidationException {
        if (!composedNode.getEndpoints().contains(endpoint)) {
            throw new RequestValidationException(createWithViolations("Provided endpoint is not attached to this Node."));
        }
        if (ISCSI.equals(endpoint.getProtocol())) {
            throw new RequestValidationException(createWithViolations("Detach endpoint with iSCSI protocol is not allowed."));
        }
    }

    private EndpointUpdateDefinition createEndpointUpdateDefinition(RedfishEndpoint representation) {
        EndpointUpdateDefinition endpointUpdateDefinition = new EndpointUpdateDefinition();
        RedfishEndpoint.Oem oem = representation.getOem();
        if (oem != null) {
            RedfishEndpoint.RackScaleOem rackScaleOem = oem.getRackScaleOem();
            if (rackScaleOem != null) {
                RedfishEndpoint.Authentication sourceAuthentication = rackScaleOem.getAuthentication();
                if (sourceAuthentication != null) {
                    endpointUpdateDefinition.getOem().getRackScaleOem().getAuthentication().setUsername(sourceAuthentication.getUsername());
                    endpointUpdateDefinition.getOem().getRackScaleOem().getAuthentication().setPassword(sourceAuthentication.getPassword());
                }
            }
        }
        return endpointUpdateDefinition;
    }

    private void validateEndpoint(Endpoint endpoint) throws ResourceStateMismatchException {
        if (!statusOf(endpoint).isEnabled().isHealthy().verify()) {
            throw new ResourceStateMismatchException("Endpoint should be enabled and healthy in order to invoke actions on it.");
        }
    }

}
