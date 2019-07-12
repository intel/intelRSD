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

import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.extern.slf4j.Slf4j;

import java.util.function.Consumer;

@Slf4j
public abstract class EntityDisassembler<T extends DiscoverableEntity> {
    T entity;
    ComposedNode node;
    WebClient webClient;
    Consumer<DiscoverableEntity> entityRemover;

    abstract void deallocate();

    abstract void unlink();

    public boolean isExisting() {
        try {
            webClient.get(entity.getUri().toUri());
            return true;
        } catch (WebClientRequestException e) {
            return false;
        }
    }

    public void decomposeRemotely() throws WebClientRequestException {
        if (isExisting()) {
            webClient.delete(entity.getUri().toUri());
            log.debug("Removed remote asset: {}", entity);
        }
    }

    public void decomposeLocaly() {
        entityRemover.accept(entity);
        log.debug("Removed entity: {}", entity);
    }

    public void decompose() {
        try {
            log.debug("Deallocating: {}", entity);
            deallocate();
            decomposeRemotely();
            decomposeLocaly();
        } catch (WebClientRequestException e) {
            unlink();
            log.debug("Remote asset cannot be removed({}), unlinking: {}", e.getMessage(), entity);
        }
    }
}
