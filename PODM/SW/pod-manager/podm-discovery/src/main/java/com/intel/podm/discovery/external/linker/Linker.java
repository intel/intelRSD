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

package com.intel.podm.discovery.external.linker;

import com.intel.podm.business.entities.base.DomainObject;

import java.util.Objects;

final class Linker<S extends DomainObject, T extends DomainObject> {
    private final Class<S> sourceClass;
    private final Class<T> targetClass;
    private final String linkName;
    private final LinkMethod<S, T> method;

    Linker(Class<S> sourceClass, Class<T> targetClass, String linkName, LinkMethod<S, T> method) {
        this.sourceClass = sourceClass;
        this.targetClass = targetClass;
        this.linkName = linkName;
        this.method = method;
    }

    public boolean tryLink(DomainObject source, DomainObject target, String linkName)  {
        if (!canLink(source, target, linkName)) {
            return false;
        }

        method.link((S) source, (T) target);
        return  true;
    }

    boolean canLink(DomainObject source, DomainObject target, String linkName) {
        return sourceClass.isInstance(source)
                && targetClass.isInstance(target)
                && Objects.equals(this.linkName, linkName);
    }

    interface LinkMethod<S extends DomainObject, T extends DomainObject> {
        void link(S source, T target);
    }
}
