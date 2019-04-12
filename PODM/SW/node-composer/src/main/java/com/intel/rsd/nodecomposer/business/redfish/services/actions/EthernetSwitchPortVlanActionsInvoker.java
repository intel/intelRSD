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

package com.intel.rsd.nodecomposer.business.redfish.services.actions;

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.discovery.external.partial.EthernetSwitchPortVlanObtainer;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.CreateVlanRequest;
import com.intel.rsd.nodecomposer.externalservices.actions.UpdateVlanRequest;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EthernetSwitchPortVlanResource;
import com.intel.rsd.nodecomposer.mappers.redfish.EthernetSwitchPortVlanMapper;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPort;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPortVlan;
import com.intel.rsd.nodecomposer.types.actions.VlanCreationRequest;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;

import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchPortVlanActionsInvoker {
    private final WebClientBuilder webClientBuilder;
    private final EthernetSwitchPortVlanObtainer vlanObtainer;
    private final EthernetSwitchPortVlanMapper mapper;

    @Autowired
    public EthernetSwitchPortVlanActionsInvoker(WebClientBuilder webClientBuilder, EthernetSwitchPortVlanObtainer vlanObtainer,
                                                EthernetSwitchPortVlanMapper mapper) {
        this.webClientBuilder = webClientBuilder;
        this.vlanObtainer = vlanObtainer;
        this.mapper = mapper;
    }

    @Transactional(MANDATORY)
    public EthernetSwitchPortVlan create(EthernetSwitchPort port, VlanCreationRequest request) throws EntityOperationException {
        URI switchPortUri = port.getUri().toUri();

        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            URI vlanCollectionUri = URI.create(switchPortUri + "/VLANs");
            URI vlanUri = webClient.post(vlanCollectionUri, new CreateVlanRequest(request.getId(), request.isTagged(), request.isEnabled()));
            return vlanObtainer.discoverEthernetSwitchPortVlan(vlanUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "Vlan creation failed on selected switch port";
            log.warn(errorMessage + " on [ path: {} ]", switchPortUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    @Transactional(MANDATORY)
    public void update(EthernetSwitchPortVlan vlan, UpdateVlanRequest request) throws EntityOperationException {
        URI vlanUri = vlan.getUri().toUri();

        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            EthernetSwitchPortVlanResource ethernetSwitchPortVlanResource = webClient.patchAndRetrieve(vlanUri, request);
            mapper.map(ethernetSwitchPortVlanResource, vlan);
        } catch (WebClientRequestException e) {
            String errorMessage = "Vlan update failed";
            log.warn(errorMessage + " on [ path: {} ]", vlanUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
