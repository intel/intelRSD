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

package com.intel.podm.business.entities.base;

import java.util.Objects;

import static com.google.common.base.Preconditions.checkNotNull;
import static java.util.Objects.hash;

public class DomainObjectsRelation {

    private final DomainObject source;
    private final DomainObject target;
    private final Link link;

    public DomainObjectsRelation(DomainObject source, DomainObject target, Link link) {
        this.source = checkNotNull(source, "Source domain object mustn't be null");
        this.target = checkNotNull(target, "Target domain object mustn't be null");
        this.link = checkNotNull(link, "Link between two domain objects mustn't be null");
    }

    public DomainObject getSource() {
        return source;
    }

    public DomainObject getTarget() {
        return target;
    }

    public Link getLink() {
        return link;
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof DomainObjectsRelation)) {
            return false;
        }

        DomainObjectsRelation that = (DomainObjectsRelation) o;
        return Objects.equals(getSource(), that.getSource())
                && Objects.equals(getTarget(), that.getTarget())
                && Objects.equals(getLink(), that.getLink());
    }

    @Override
    public int hashCode() {
        return hash(getSource(), getTarget(), getLink());
    }
}
