/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import com.intel.rsd.nodecomposer.utils.retry.RetryOnRollback;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.io.Serializable;
import java.util.Collection;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
public class FabricDisassemblyTask extends NodeTask implements Serializable {
    private static final long serialVersionUID = 8812974285949471543L;

    @Autowired
    private transient WebClientBuilder webClientBuilder;
    @Autowired
    private transient GenericDao genericDao;
    @Autowired
    private transient EntityDisassemblerFactory disassemblerFactory;
    @Autowired
    private transient FabricAssetsSelector fabricAssetsSelector;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    @RetryOnRollback(3)
    public void run() {
        decompose();
    }

    private void decompose() {
        ComposedNode composedNode = genericDao.find(ComposedNode.class, composedNodeODataId);
        Collection<DiscoverableEntity> assetsInOrder = fabricAssetsSelector.selectRelatedAssets(composedNode);
        try (WebClient webClient = createWebClient()) {
            for (DiscoverableEntity entity : assetsInOrder) {
                disassemblerFactory
                    .createDisassemblerFor(entity, composedNode)
                    .use(genericDao::remove)
                    .use(webClient)
                    .decompose();
            }
        }
    }

    private WebClient createWebClient() {
        return webClientBuilder.createResourceManagerInstance().retryable().build();
    }
}
