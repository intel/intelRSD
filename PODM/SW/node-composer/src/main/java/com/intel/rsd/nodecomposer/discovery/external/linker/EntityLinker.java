/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external.linker;

import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Collection;

import static java.lang.String.format;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

/**
 * Allows to associate two entities based on name of relationship
 */
@Component
@Scope(SCOPE_PROTOTYPE)
public class EntityLinker {
    @Autowired
    private Collection<Linker> linkers;

    public void link(Entity source, Entity target, String linkName) {
        for (Linker linker : linkers) {
            if (linker.tryLink(source, target, linkName)) {
                return;
            }
        }

        String msg = format("Cannot create link with name '%s' from %s to %s", linkName, source, target);
        throw new IllegalArgumentException(msg);
    }
}
