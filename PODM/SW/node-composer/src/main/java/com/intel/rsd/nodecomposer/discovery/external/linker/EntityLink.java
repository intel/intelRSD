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

package com.intel.rsd.nodecomposer.discovery.external.linker;

import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;

import java.util.Objects;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;

public final class EntityLink {
    private final Entity source;
    private final Entity target;
    private final String name;

    public EntityLink(Entity source, Entity target, String name) {
        this.source = requiresNonNull(source, "source");
        this.target = requiresNonNull(target, "target");
        this.name = requiresNonNull(name, "name");
    }

    public Entity getSource() {
        return source;
    }

    public Entity getTarget() {
        return target;
    }

    public String getName() {
        return name;
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof EntityLink)) {
            return false;
        }

        EntityLink that = (EntityLink) o;
        return Objects.equals(getSource(), that.getSource())
            && Objects.equals(getTarget(), that.getTarget())
            && Objects.equals(getName(), that.getName());
    }

    @Override
    public int hashCode() {
        return Objects.hash(source, target, name);
    }
}
