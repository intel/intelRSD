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

package com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers;

import com.fasterxml.jackson.databind.JsonNode;
import lombok.NonNull;
import lombok.val;
import org.springframework.stereotype.Component;

import java.util.AbstractMap.SimpleEntry;
import java.util.Comparator;

import static com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers.FindIdForEndpoint.DurableNameFormat.toDurableNameFormat;
import static java.util.Comparator.comparing;
import static java.util.stream.StreamSupport.stream;

@Component
public class FindIdForEndpoint implements TransformingFunction {

    private static final Comparator<SimpleEntry<DurableNameFormat, String>> DURABLE_NAME_FORMAT_PRIORITY_COMPARATOR = comparing(SimpleEntry::getKey);

    /**
     * Order of enumeration elements will be treat as an order of priorities
     */
    enum DurableNameFormat {
        UUID, NQN, IQN, UNKNOWN;

        static DurableNameFormat toDurableNameFormat(@NonNull String durableNameFormat) {
            try {
                return valueOf(durableNameFormat);
            } catch (IllegalArgumentException ex) {
                return UNKNOWN;
            }
        }
    }

    @Override
    public String functionId() {
        return "findIdForEndpoint";
    }

    @Override
    public String invoke(@NonNull JsonNode... nodes) {
        val identifiers = nodes[0];
        val selectedIdentifier = stream(identifiers.spliterator(), false)
            .map(this::toDurableNameByFormat)
            .min(DURABLE_NAME_FORMAT_PRIORITY_COMPARATOR);

        return selectedIdentifier.map(SimpleEntry::getValue).orElse(null);
    }

    private SimpleEntry<DurableNameFormat, String> toDurableNameByFormat(JsonNode identifier) {
        return new SimpleEntry<>(
            toDurableNameFormat(identifier.at("/DurableNameFormat").asText()),
            identifier.at("/DurableName").asText());
    }
}
