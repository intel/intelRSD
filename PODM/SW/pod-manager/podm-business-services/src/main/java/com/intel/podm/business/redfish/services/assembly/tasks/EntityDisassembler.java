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

import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.common.logger.Logger;

import javax.inject.Inject;
import java.util.function.Consumer;

public abstract class EntityDisassembler<T extends DiscoverableEntity> {
    T entity;
    ComposedNode node;
    WebClient webClient;
    Consumer<DiscoverableEntity> entityRemover;

    @Inject
    private Logger logger;

    abstract void deallocate();

    abstract void unlink();

    public void removeRemoteAsset() throws WebClientRequestException {
        webClient.delete(entity.getSourceUri());
        logger.d("Removed remote asset: {}", entity);
    }

    public void removeEntity() {
        entityRemover.accept(entity);
        logger.d("Removed entity: {}", entity);
    }

    public void decompose() {
        try {
            logger.d("Deallocating: {}", entity);
            deallocate();
            removeRemoteAsset();
            removeEntity();
        } catch (WebClientRequestException e) {
            unlink();
            logger.d("Remote asset cannot be removed({}), unlinking: {}", e.getMessage(), entity);
        }
    }
}
