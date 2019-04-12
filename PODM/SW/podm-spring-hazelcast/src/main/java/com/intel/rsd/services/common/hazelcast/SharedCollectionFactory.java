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

package com.intel.rsd.services.common.hazelcast;

import com.hazelcast.core.HazelcastInstance;
import com.hazelcast.core.IAtomicReference;
import com.intel.rsd.services.common.Shared;
import org.springframework.beans.factory.InjectionPoint;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Scope;

import java.util.Map;
import java.util.Set;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Configuration
public class SharedCollectionFactory {
    @Bean
    @Scope(SCOPE_PROTOTYPE)
    @Qualifier("shared")
    public Map<?, ?> createSharedMap(InjectionPoint injectionPoint, HazelcastInstance hazelcastInstance) {
        Shared shared = injectionPoint.getAnnotation(Shared.class);
        return hazelcastInstance.getMap(shared.value());
    }

    @Bean
    @Scope(SCOPE_PROTOTYPE)
    @Qualifier("shared")
    public Set<?> createSharedSet(InjectionPoint injectionPoint, HazelcastInstance hazelcastInstance) {
        Shared shared = injectionPoint.getAnnotation(Shared.class);
        return hazelcastInstance.getSet(shared.value());
    }

    @Bean
    @Scope(SCOPE_PROTOTYPE)
    @Qualifier("shared")
    public IAtomicReference<?> createAtomicReference(InjectionPoint injectionPoint, HazelcastInstance hazelcastInstance) {
        Shared shared = injectionPoint.getAnnotation(Shared.class);
        return hazelcastInstance.getAtomicReference(shared.value());
    }

}
