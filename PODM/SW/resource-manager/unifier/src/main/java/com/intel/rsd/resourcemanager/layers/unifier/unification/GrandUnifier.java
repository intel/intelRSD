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

package com.intel.rsd.resourcemanager.layers.unifier.unification;

import com.google.common.cache.CacheBuilder;
import com.google.common.cache.CacheLoader;
import com.google.common.cache.LoadingCache;
import com.intel.rsd.resourcemanager.layers.Response;
import lombok.NonNull;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import javax.annotation.ParametersAreNonnullByDefault;
import java.util.Collection;

import static java.util.Optional.ofNullable;

@Component
public class GrandUnifier {

    private final LoadingCache<Class<?>, Unifier> cachedUnifiers = CacheBuilder.newBuilder()
        .build(new CacheLoader<Class<?>, Unifier>() {
            @Override
            @ParametersAreNonnullByDefault
            public Unifier load(Class<?> classToRegister) {
                return findForSupertype(classToRegister, cachedUnifiers);
            }
        });

    private final LoadingCache<Class<?>, Deunifier> cachedDeunifiers = CacheBuilder.newBuilder()
        .build(new CacheLoader<Class<?>, Deunifier>() {
            @Override
            @ParametersAreNonnullByDefault
            public Deunifier load(Class<?> classToRegister) {
                return findForSupertype(classToRegister, cachedDeunifiers);
            }
        });

    @Autowired
    public GrandUnifier(Collection<Unifier<?>> unifiers, Collection<Deunifier<?>> deunifiers) {
        unifiers.forEach(unifier -> cachedUnifiers.put(unifier.getUnifiableClass(), unifier));
        deunifiers.forEach(deunifier -> cachedDeunifiers.put(deunifier.getDeunifiableClass(), deunifier));
    }

    public Response unify(@NonNull Response response, @NonNull UnifiedOdataIdResolver resolver) {
        return new Response(
            unify(response.getBody(), resolver),
            unify(response.getHttpHeaders(), resolver),
            response.getHttpStatus()
        );
    }

    public <T> T unify(T toBeUnified, @NonNull UnifiedOdataIdResolver resolver) {
        return ofNullable(toBeUnified)
            .map(this::findUnifier)
            .map(unifier -> unifier.unify(toBeUnified, resolver))
            .orElse(toBeUnified);
    }

    public <T> T deunify(T toBeDeunified, @NonNull UnifiedOdataIdResolver resolver) {
        return ofNullable(toBeDeunified)
            .map(this::findDeunifier)
            .map(deunifier -> deunifier.deunify(toBeDeunified, resolver))
            .orElse(toBeDeunified);
    }

    private <T> Unifier<T> findUnifier(@NonNull T toBeUnified) {
        return cachedUnifiers.getUnchecked(toBeUnified.getClass());
    }

    private <T> Deunifier<T> findDeunifier(@NonNull T toBeDeunified) {
        return cachedDeunifiers.getUnchecked(toBeDeunified.getClass());
    }

    private <T> T findForSupertype(Class<?> classToRegister, LoadingCache<Class<?>, T> cache) {
        for (Class<?> registeredClass : cache.asMap().keySet()) {
            if (registeredClass.isAssignableFrom(classToRegister)) {
                return cache.getUnchecked(registeredClass);
            }
        }
        throw new UnsupportedOperationException("Unifier/deunifier is not implemented for: " + classToRegister);
    }
}
