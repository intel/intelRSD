/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation;

import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.ApplicationArguments;
import org.springframework.boot.ApplicationRunner;
import org.springframework.context.annotation.Scope;
import org.springframework.core.Ordered;
import org.springframework.stereotype.Component;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class AllocationStartup implements ApplicationRunner, Ordered {
    private static final int ORDER = 5;

    private final ComposedNodeSanitizer composedNodeSanitizer;

    @Autowired
    public AllocationStartup(ComposedNodeSanitizer composedNodeSanitizer) {
        this.composedNodeSanitizer = composedNodeSanitizer;
    }

    @Override
    public void run(ApplicationArguments args) throws Exception {
        log.debug("Failing all composed nodes that were not assembled before restart");
        composedNodeSanitizer.sanitizeComputerSystemsUsedInComposedNodes();
    }

    @Override
    public int getOrder() {
        return ORDER;
    }
}
