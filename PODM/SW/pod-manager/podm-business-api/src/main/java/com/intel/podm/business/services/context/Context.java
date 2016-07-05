/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.business.services.context;

import com.intel.podm.common.types.Id;

import java.util.Objects;

import static java.lang.String.format;
import static java.util.Objects.hash;

/**
 * Represents path to Entity located in Entity Tree.
 */
public final class Context {
    private final Id id;
    private final ContextType type;
    private final Context parent;

    private Context(Id id, ContextType type, Context parent) {
        if (id == null) {
            throw new IllegalArgumentException("id should not be null");
        }

        if (type == null) {
            throw new IllegalArgumentException("type should not be null");
        }

        this.id = id;
        this.type = type;
        this.parent = parent;
    }

    public static Context contextOf(Id entityId, ContextType type) {
        return new Context(entityId, type, null);
    }

    public Id getId() {
        return id;
    }

    public ContextType getType() {
        return type;
    }

    public Context getParent() {
        return parent;
    }

    public Context child(Id id, ContextType type) {
        if (id == null) {
            throw new IllegalArgumentException("id should not be null");
        }

        if (type == null) {
            throw new IllegalArgumentException("type should not be null");
        }

        if (!type.getPossibleParents().contains(this.type)) {
            throw new IllegalArgumentException();
        }

        return new Context(id, type, this);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (!(o instanceof Context)) {
            return false;
        }

        Context rhs = (Context) o;
        return Objects.equals(id, rhs.id)
                && Objects.equals(type, rhs.type)
                && Objects.equals(parent, rhs.parent);
    }

    @Override
    public int hashCode() {
        return hash(id, type);
    }

    @Override
    public String toString() {
        return parent == null
                ? format("%s(%s)", type, id)
                : format("%s-%s(%s)", parent, type, id);
    }

    public static boolean isAcceptableChildOf(ContextType childType, Context parent) {
        if (childType == null) {
            throw new IllegalArgumentException("childType must not be null");
        }

        if (parent == null) {
            return childType.getPossibleParents().isEmpty();
        }

        return childType.getPossibleParents().contains(parent.getType());
    }
}
