/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.discovery.external.linker;

import com.intel.podm.business.entities.redfish.base.Entity;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;

import static java.lang.String.format;

/**
 * Allows to associate two entities based on name of relationship
 */
@Dependent
public class EntityLinker {
    @Inject
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
