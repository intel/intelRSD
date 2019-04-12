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

package com.intel.rsd.nodecomposer.persistence.redfish.base;

import javax.persistence.Column;
import javax.persistence.GeneratedValue;
import javax.persistence.MappedSuperclass;
import javax.persistence.PreRemove;
import javax.persistence.Version;
import java.util.Collection;
import java.util.Iterator;
import java.util.Objects;
import java.util.function.Consumer;
import java.util.function.Predicate;

import static java.lang.String.format;
import static java.util.Objects.hash;
import static javax.persistence.GenerationType.IDENTITY;

@MappedSuperclass
public abstract class Entity {
    protected static final String ENTITY_ID_NUMERIC_COLUMN_DEFINITION = "bigserial";

    @javax.persistence.Id
    @GeneratedValue(strategy = IDENTITY)
    @Column(name = "id")
    private long id;

    @Version
    @Column(name = "version", columnDefinition = "integer DEFAULT 0", nullable = false)
    private long version;

    protected long getPrimaryKey() {
        return id;
    }

    public abstract void preRemove();

    @PreRemove
    public void unlinkRelations() {
        preRemove();
    }

    protected <T extends Entity> void unlinkCollection(Collection<T> entities, Consumer<T> unlinkConsumer, Predicate<T> predicate) {
        // Iterator prevents ConcurrentModification exception, update method carefully. Checked by unit test.
        Iterator<T> iterator = entities.iterator();
        while (iterator.hasNext()) {
            T entity = iterator.next();
            if (predicate.test(entity)) {
                unlinkConsumer.accept(entity);
                iterator = entities.iterator();
            }
        }
    }

    protected <T extends Entity> void unlinkCollection(Collection<T> entities, Consumer<T> unlinkConsumer) {
        unlinkCollection(entities, unlinkConsumer, x -> true);
    }

    @Override
    public int hashCode() {
        return hash(id);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || (!(o instanceof Entity))) {
            return false;
        }

        Entity that = (Entity) o;
        return Objects.equals(getPrimaryKey(), that.getPrimaryKey());
    }

    @Override
    public String toString() {
        return format("Entity {clazz=%s, primaryKey=%d}", getClass().getSimpleName(), getPrimaryKey());
    }
}
