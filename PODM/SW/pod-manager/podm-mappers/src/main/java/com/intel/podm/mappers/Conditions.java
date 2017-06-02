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

package com.intel.podm.mappers;

import com.intel.podm.common.types.Ref;
import org.modelmapper.Condition;
import org.modelmapper.spi.PropertyInfo;

import java.util.Arrays;
import java.util.Objects;

import static com.intel.podm.mappers.Converters.refConverter;
import static org.apache.commons.lang.BooleanUtils.negate;

public final class Conditions {
    private Conditions() {
    }

    public static Condition<Ref<?>, ?> refMappingCondition() {
        return context -> {
            if (!context.getSourceType().isAssignableFrom(Ref.class)) {
                return true;
            }

            // TODO: can we avoid this hack? it should probably be moved to mapper API
            if (context.getParent() != null) {
                context.getParent().getTypeMap().setPropertyConverter(refConverter());
            }

            return context.getSource() != null && context.getSource().isAssigned();
        };
    }

    public static Condition<?, ?> ignoredPropertiesCondition(String... propertiesNames) {
        return context -> negate(
                context.getMapping().getDestinationProperties().stream()
                .map(PropertyInfo::getName)
                .filter(Objects::nonNull)
                .filter(Arrays.asList(propertiesNames)::contains)
                .findFirst()
                .isPresent()
        );
    }
}
