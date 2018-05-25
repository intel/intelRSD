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

package com.intel.podm.discovery.external;

import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.resources.redfish.ComputerSystemResource;
import com.intel.podm.common.synchronization.TaskCanceledException;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.discovery.external.restgraph.RestGraph;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import java.util.Collection;

import static java.util.stream.Collectors.toList;

@ApplicationScoped
public class DiscoveryRunnerHooks {
    @Inject
    private ComputerSystemDao computerSystemDao;

    void onRestGraphCreated(RestGraph graph, ExternalService service) {
        if (service.getServiceType() != ServiceType.INBAND) {
            return;
        }

        Collection<ComputerSystemResource> computerSystems = graph.getResources().stream()
                .filter(ComputerSystemResource.class::isInstance)
                .map(ComputerSystemResource.class::cast)
                .collect(toList());

        checkWhetherPrimaryServiceExist(computerSystems);
    }

    private void checkWhetherPrimaryServiceExist(Collection<ComputerSystemResource> computerSystems) {
        for (ComputerSystemResource computerSystem : computerSystems) {
            if (!computerSystem.getUuid().isAssigned()) {
                throw new TaskCanceledException("Computer system exposed by InBand service doesn't provide obligatory value for UUID");
            }

            if (!computerSystemDao.findPrimarySystem(computerSystem.getUuid().get()).isPresent()) {
                throw new TaskCanceledException("Primary service is not discovered yet");
            }
        }
    }
}
