/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.common.types.DurableNameFormat;

import java.util.UUID;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;

public final class DurableNameProducer {
    private DurableNameProducer() {
    }

    public static String createDurableName(DurableNameFormat format, UUID uuid) {
        requiresNonNull(format, "DurableNameFormat");
        requiresNonNull(uuid, "uuid");

        String durableName;
        switch (format) {
            case IQN: durableName = createIqn(uuid);
                      break;
            case NQN: durableName = createNqn(uuid);
                      break;
            default: throw new IllegalStateException(format("Could not produce durable name for %s", format));
        }

        return durableName;
    }

    public static String createIqn(UUID uuid) {
        return format("%s:%s", "iqn.1986-03.com.intel", uuid);
    }

    public static String createNqn(UUID uuid) {
        return format("%s:%s", "nqn.2014-08.org.nvmexpress:uuid", uuid);
    }
}
