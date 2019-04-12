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

package com.intel.rsd.nodecomposer.discovery.external.finalizers;

import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Identifier;
import com.intel.rsd.nodecomposer.types.DurableNameFormat;
import org.apache.commons.lang3.StringUtils;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.UUID;

import static com.intel.rsd.nodecomposer.types.DurableNameFormat.IQN;
import static com.intel.rsd.nodecomposer.types.DurableNameFormat.NQN;
import static java.util.Optional.empty;
import static java.util.Optional.of;

public final class IdentifierUtils {
    private static final List<DurableNameFormat> SUPPORTED_DURABLE_NAME_FORMATS = new ArrayList<>();

    static {
        SUPPORTED_DURABLE_NAME_FORMATS.add(IQN);
        SUPPORTED_DURABLE_NAME_FORMATS.add(NQN);
    }

    private IdentifierUtils() {
    }

    public static Optional<UUID> tryGetUuidFromIdentifier(Identifier identifier) {
        if (identifierFormatIsSupported(identifier)) {
            return tryConvertDurableNameToUuid(identifier);
        }

        return empty();
    }

    private static Optional<UUID> tryConvertDurableNameToUuid(Identifier identifier) {
        try {
            String uuidString = getLastSegmentOfDurableName(identifier.getDurableName());
            return of(UUID.fromString(uuidString));
        } catch (IllegalArgumentException e) {
            return empty();
        }
    }

    private static boolean identifierFormatIsSupported(Identifier identifier) {
        return SUPPORTED_DURABLE_NAME_FORMATS.contains(identifier.getDurableNameFormat());
    }

    private static String getLastSegmentOfDurableName(String durableName) {
        return StringUtils.substringAfterLast(durableName, ":");
    }
}
