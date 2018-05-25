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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.entities.dao.ComposedNodeDao;
import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.UUID;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class StorageServiceDisassemblyTask extends NodeTask {

    private UUID storageServiceUuid;

    @Inject
    private ComposedNodeDao composedNodeDao;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private ExternalServiceDao externalServiceDao;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Inject
    private EntityDisassemblerFactory disassemblerFactory;

    @Inject
    private StorageServiceAssetsSelector storageServiceAssetsSelector;

    public StorageServiceDisassemblyTask init(UUID storageServiceUuid) {
        this.storageServiceUuid = storageServiceUuid;
        return this;
    }

    @Override
    public UUID getServiceUuid() {
        return this.storageServiceUuid;
    }

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        requiresNonNull(storageServiceUuid, "storageServiceUuid");
        ExternalService service = externalServiceDao.tryGetUniqueExternalServiceByUuid(getServiceUuid());
        if (service == null) {
            return;
        }
        decompose(service);
    }

    private void decompose(ExternalService service) {
        ComposedNode composedNode = composedNodeDao.find(nodeId);
        Collection<DiscoverableEntity> assetsInOrder = storageServiceAssetsSelector.selectRelatedAssets(getServiceUuid(), composedNode);
        try (WebClient webClient = createWebClient(service)) {
            for (DiscoverableEntity entity : assetsInOrder) {
                disassemblerFactory
                    .createDisassemblerFor(entity, composedNode)
                    .use(discoverableEntityDao::removeWithConnectedExternalLinks)
                    .use(webClient)
                    .decompose();
            }
        }
    }

    private WebClient createWebClient(ExternalService service) {
        return webClientBuilder.newInstance(service.getBaseUri()).retryable().build();
    }

}
