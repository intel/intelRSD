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

package com.intel.podm.services.configuration;

import com.intel.podm.client.WebClient;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.services.configuration.client.InitiatorFinder;
import com.intel.podm.services.configuration.client.RestActionInvoker;
import com.intel.podm.services.configuration.tasks.DiscoveryServiceTask;
import com.intel.podm.services.configuration.tasks.DiscoveryServiceTaskObtainer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;

import static com.intel.podm.common.utils.Collections.nullOrEmpty;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class DiscoveryServiceConfigurer {
    @Inject
    private Logger logger;

    @Inject
    private DiscoveryServiceTaskObtainer discoveryServiceTaskObtainer;

    @Inject
    private ComposedNodeEndpointsFinder composedNodeEndpointsFinder;

    @Inject
    private InitiatorFinder initiatorFinder;

    @Inject
    private RestActionInvoker restActionInvoker;

    @Transactional(REQUIRES_NEW)
    public void configureUsingWebClient(WebClient webClient) {
        requiresNonNull(webClient, "webClient");

        List<DiscoveryServiceTask> listOfTasks = discoveryServiceTaskObtainer.getListOfTasks(
            webClient,
            composedNodeEndpointsFinder.createComputerSystemUuidWithEndpointCollectionMapping(),
            initiatorFinder.createComputerSystemUuidWithInitiatorEndpointMapping(webClient)
        );

        URI discoveryServiceUri = webClient.getBaseUri();
        if (!nullOrEmpty(listOfTasks)) {
            logger.i(format("Discovery service %s state need to be updated", discoveryServiceUri));
            runTasksOnDiscoveryService(discoveryServiceUri, listOfTasks);
        } else {
            logger.d(format("Discovery service %s state is up to date", discoveryServiceUri));
        }
    }

    private void runTasksOnDiscoveryService(URI discoveryServiceUri, List<DiscoveryServiceTask> discoveryServiceTasks) {
        logger.i("Discovery Service {} configure process started.", discoveryServiceUri);
        discoveryServiceTasks.forEach(task -> {
            task.setDiscoveryServiceUri(discoveryServiceUri);
            task.setRestActionInvoker(restActionInvoker);
            task.perform();
        });

        logger.i("Discovery Service {} configure process finished.", discoveryServiceUri);
    }
}
