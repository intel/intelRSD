/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.services.configuration;

import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.FabricResource;
import com.intel.rsd.nodecomposer.services.configuration.client.RestActionInvoker;
import com.intel.rsd.nodecomposer.services.configuration.client.ServiceResourceFinder;
import com.intel.rsd.nodecomposer.services.configuration.tasks.DiscoveryServiceTask;
import com.intel.rsd.nodecomposer.services.configuration.tasks.DiscoveryServiceTaskObtainer;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.types.Protocol.FPGA_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.types.Protocol.OEM;
import static com.intel.rsd.nodecomposer.utils.Collections.nullOrEmpty;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.Collections.emptyList;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class DiscoveryServiceConfigurer {
    private final DiscoveryServiceTaskObtainer discoveryServiceTaskObtainer;
    private final UuidEndpointsMapProvider uuidEndpointsMapProvider;
    private final ServiceResourceFinder serviceResourceFinder;
    private final RestActionInvoker restActionInvoker;

    @Inject
    public DiscoveryServiceConfigurer(DiscoveryServiceTaskObtainer discoveryServiceTaskObtainer, UuidEndpointsMapProvider uuidEndpointsMapProvider,
                                      ServiceResourceFinder serviceResourceFinder, RestActionInvoker restActionInvoker) {
        this.discoveryServiceTaskObtainer = discoveryServiceTaskObtainer;
        this.uuidEndpointsMapProvider = uuidEndpointsMapProvider;
        this.serviceResourceFinder = serviceResourceFinder;
        this.restActionInvoker = restActionInvoker;
    }

    @Transactional(REQUIRES_NEW)
    public void execute(WebClient webClient) {
        requiresNonNull(webClient, "webClient");

        List<DiscoveryServiceTask> listOfTasks = getListOfTasks(webClient, NVME_OVER_FABRICS, NVME_OVER_FABRICS);
        listOfTasks.addAll(getListOfTasks(webClient, OEM, FPGA_OVER_FABRICS));

        URI discoveryServiceUri = webClient.getBaseUri();
        if (!nullOrEmpty(listOfTasks)) {
            log.info(format("Discovery Service %s state needs to be updated", discoveryServiceUri));
            runTasksOnDiscoveryService(discoveryServiceUri, listOfTasks);
        } else {
            log.debug(format("Discovery service %s state is up to date", discoveryServiceUri));
        }
    }

    private List<DiscoveryServiceTask> getListOfTasks(WebClient webClient, Protocol resourceProtocol, Protocol entityProtocol) {
        Optional<FabricResource> fabricResourceCandidate = serviceResourceFinder.getFabricResource(webClient, resourceProtocol);
        if (fabricResourceCandidate.isPresent()) {
            return discoveryServiceTaskObtainer.getListOfTasks(
                uuidEndpointsMapProvider.createInitiatorUuidToZoneEndpointsMap(entityProtocol),
                serviceResourceFinder.createUuidWithInitiatorEndpointMapping(webClient, resourceProtocol),
                fabricResourceCandidate.get()
            );
        }
        return emptyList();
    }

    private void runTasksOnDiscoveryService(URI discoveryServiceUri, List<DiscoveryServiceTask> discoveryServiceTasks) {
        log.info("Discovery Service {} configure process started.", discoveryServiceUri);
        discoveryServiceTasks.forEach(task -> {
            task.setDiscoveryServiceUri(discoveryServiceUri);
            task.setRestActionInvoker(restActionInvoker);
            log.info("Sending configuration of {} to Discovery Service", task);
            task.perform();
        });

        log.info("Discovery Service {} configure process finished.", discoveryServiceUri);
    }
}
