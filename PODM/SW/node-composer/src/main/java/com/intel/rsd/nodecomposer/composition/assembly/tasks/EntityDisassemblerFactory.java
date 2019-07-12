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
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.persistence.redfish.Zone;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.utils.beans.NodeComposerBeanFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;
import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;
import java.util.function.Supplier;

import static java.util.Optional.ofNullable;
import static org.hibernate.proxy.HibernateProxyHelper.getClassWithoutInitializingProxy;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class EntityDisassemblerFactory {
    private final Map<Class, Supplier<EntityDisassembler>> knownCreators = new HashMap<>();

    private final NodeComposerBeanFactory beanFactory;

    @Autowired
    public EntityDisassemblerFactory(NodeComposerBeanFactory beanFactory) {
        this.beanFactory = beanFactory;
    }

    @PostConstruct
    public void init() {
        knownCreators.put(Endpoint.class, () -> beanFactory.create(EndpointDisassembler.class));
        knownCreators.put(Volume.class, () -> beanFactory.create(VolumeDisassembler.class));
        knownCreators.put(Zone.class, () -> beanFactory.create(ZoneDisassembler.class));
        knownCreators.put(Processor.class, () -> beanFactory.create(FpgaDisassembler.class));
    }

    Builder createDisassemblerFor(DiscoverableEntity entity, ComposedNode node) {
        return new Builder(entity, node);
    }

    @SuppressWarnings("checkstyle:MethodCount")
    final class Builder {

        private EntityDisassembler disassembler;

        private Builder(DiscoverableEntity entity, ComposedNode node) {
            this.disassembler = ofNullable(knownCreators.get(getClassWithoutInitializingProxy(entity)))
                .orElseThrow(() -> new RuntimeException("Missing implementation, provided entity is not supported: " + entity))
                .get();

            disassembler.entity = entity;
            disassembler.node = node;
        }

        Builder use(WebClient webClient) {
            disassembler.webClient = webClient;
            return this;
        }

        Builder use(Consumer<DiscoverableEntity> entityRemover) {
            disassembler.entityRemover = entityRemover;
            return this;
        }

        @SuppressWarnings("checkstyle:IllegalCatch")
        void decompose() {
            try {
                disassembler.decompose();
            } catch (RuntimeException e) {
                disassembler.unlink();
            }
        }
    }
}
