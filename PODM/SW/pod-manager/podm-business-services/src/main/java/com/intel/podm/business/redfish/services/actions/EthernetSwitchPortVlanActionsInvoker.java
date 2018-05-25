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
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.CreateVlanRequest;
import com.intel.podm.client.actions.UpdateVlanRequest;
import com.intel.podm.client.resources.redfish.EthernetSwitchPortVlanResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.VlanCreationRequest;
import com.intel.podm.discovery.external.partial.EthernetSwitchPortVlanObtainer;
import com.intel.podm.mappers.redfish.EthernetSwitchPortVlanMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchPortVlanActionsInvoker {
    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private Logger logger;

    @Inject
    private EthernetSwitchPortVlanObtainer vlanObtainer;

    @Inject
    private EthernetSwitchPortVlanMapper mapper;

    @Transactional(MANDATORY)
    public EthernetSwitchPortVlan create(EthernetSwitchPort port, VlanCreationRequest request) throws EntityOperationException {
        ExternalService service = port.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected switch port");

        URI serviceBaseUri = service.getBaseUri();
        URI switchPortUri = port.getSourceUri();

        try (WebClient webClient = webClientBuilder.newInstance(serviceBaseUri).retryable().build()) {
            URI vlanCollectionUri = URI.create(switchPortUri + "/VLANs");
            URI vlanUri = webClient.post(vlanCollectionUri, new CreateVlanRequest(request.getId(), request.isTagged(), request.isEnabled()));
            return vlanObtainer.discoverEthernetSwitchPortVlan(service, vlanUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "Vlan creation failed on selected switch port";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", serviceBaseUri, switchPortUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    @Transactional(MANDATORY)
    public void update(EthernetSwitchPortVlan vlan, UpdateVlanRequest request) throws EntityOperationException {
        ExternalService service = vlan.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected vlan");

        URI serviceBaseUri = service.getBaseUri();
        URI vlanUri = vlan.getSourceUri();

        try (WebClient webClient = webClientBuilder.newInstance(serviceBaseUri).retryable().build()) {
            EthernetSwitchPortVlanResource ethernetSwitchPortVlanResource = webClient.patchAndRetrieve(vlanUri, request);
            mapper.map(ethernetSwitchPortVlanResource, vlan);
        } catch (WebClientRequestException e) {
            String errorMessage = "Vlan update failed";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", serviceBaseUri, vlanUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
