/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.mappers;

import com.intel.podm.common.types.Ref;
import org.modelmapper.Condition;
import org.modelmapper.spi.MappingContext;

import java.lang.reflect.Field;
import java.util.Collection;

import static com.intel.podm.mappers.Converters.refConverter;

public final class Conditions {
    private Conditions() {
    }

    public static Condition<Ref<?>, ?> refMappingCondition() {
        return context -> {
            if (!context.getSourceType().isAssignableFrom(Ref.class)) {
                return true;
            }

            if (context.getParent() != null) {
                context.getParent().getTypeMap().setPropertyConverter(refConverter());
            }

            return context.getSource() != null && context.getSource().isAssigned();
        };
    }

    public static Condition<?, ?> aggregateCondition(boolean isSourceFavored, boolean useNullOverwrite) {
        return context -> {
            Object value = tryGetDestinationValue(context);
            if (value == null || isAnEmptyCollection(value)) {
                return true;
            }

            if (isSourceFavored) {
                if (context.getSource() != null) {
                    return true;
                } else {
                    return useNullOverwrite;
                }
            }
            return false;
        };
    }

    private static boolean isAnEmptyCollection(Object value) {
        return value instanceof Collection<?> && ((Collection) value).isEmpty();
    }

    private static Object tryGetDestinationValue(MappingContext<Object, Object> context) {
        try {
            String fieldName = context.getMapping().getLastDestinationProperty().getName();
            Field declaredField = context.getParent().getDestination().getClass().getDeclaredField(fieldName);
            Object value;
            if (!declaredField.isAccessible()) {
                declaredField.setAccessible(true);
                value = declaredField.get(context.getParent().getDestination());
                declaredField.setAccessible(false);
            } else {
                value = declaredField.get(context.getParent().getDestination());
            }
            return value;
        } catch (NoSuchFieldException | IllegalAccessException e) {
            return null;
        }
    }
}
