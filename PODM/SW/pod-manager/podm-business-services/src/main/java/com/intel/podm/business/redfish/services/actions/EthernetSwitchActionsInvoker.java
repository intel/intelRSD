/*
 * Copyright (c) 2018 Intel Corporation
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
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.resources.redfish.EthernetSwitchResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitch;
import com.intel.podm.discovery.external.EntityMultiMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class EthernetSwitchActionsInvoker {
    @Inject
    private Logger logger;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private EntityMultiMapper entityMultiMapper;

    @Transactional(MANDATORY)
    public void updateEthernetSwitch(EthernetSwitch ethernetSwitch, RedfishEthernetSwitch redfishEthernetSwitch) throws EntityOperationException {
        ExternalService service = ethernetSwitch.getService();
        if (service == null) {
            throw new IllegalStateException("There is no ExternalService associated with selected EthernetSwitch");
        }

        URI ethernetSwitchUri = ethernetSwitch.getSourceUri();
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            EthernetSwitchResource ethernetSwitchResource = webClient.patchAndRetrieve(ethernetSwitchUri, redfishEthernetSwitch);
            entityMultiMapper.map(ethernetSwitchResource, service);
        } catch (WebClientRequestException e) {
            String errorMessage = "Modification failed on selected EthernetSwitch";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), ethernetSwitchUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
