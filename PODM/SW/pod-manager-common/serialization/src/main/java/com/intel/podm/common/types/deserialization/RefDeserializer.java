/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.common.types.deserialization;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.BeanProperty;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JavaType;
import com.fasterxml.jackson.databind.JsonDeserializer;
import com.fasterxml.jackson.databind.JsonMappingException;
import com.fasterxml.jackson.databind.deser.ContextualDeserializer;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.annotations.AsUnassigned;

import java.io.IOException;
import java.util.Arrays;
import java.util.Collection;

import static java.util.Collections.emptyList;

public class RefDeserializer extends JsonDeserializer<Ref<?>> implements ContextualDeserializer {
    private BeanProperty beanProperty;
    private Collection<AsUnassigned.Strategy> unassignedStrategies = emptyList();

    public RefDeserializer() {
    }

    private RefDeserializer(Collection<AsUnassigned.Strategy> unassignedStrategies, BeanProperty property) {
        super();
        this.unassignedStrategies = unassignedStrategies;
        this.beanProperty = property;
    }

    @Override
    public Ref<?> deserialize(JsonParser p, DeserializationContext ctxt) throws IOException {
        JavaType targetType = beanProperty.getType().containedType(0);
        JsonDeserializer<Object> contextualValueDeserializer = ctxt.findContextualValueDeserializer(targetType, beanProperty);
        Object containedObject = contextualValueDeserializer.deserialize(p, ctxt);

        return unassignedStrategies.stream().anyMatch(s -> s.isUnassigned(containedObject))
                ?
                Ref.unassigned()
                :
                Ref.of(containedObject);
    }

    @Override
    public JsonDeserializer<?> createContextual(DeserializationContext ctxt, BeanProperty property) throws JsonMappingException {
        Collection<AsUnassigned.Strategy> strategies = getStrategies(property.getAnnotation(AsUnassigned.class));
        return new RefDeserializer(strategies, property);
    }

    private Collection<AsUnassigned.Strategy> getStrategies(AsUnassigned annotation) {
        if (annotation == null) {
            return emptyList();
        }

        return Arrays.asList(annotation.value());
    }

    @Override
    public Ref<?> getNullValue() {
        return unassignedStrategies.stream().anyMatch(s -> s.isUnassigned(null)) ? Ref.unassigned() : Ref.of(null);
    }
}
