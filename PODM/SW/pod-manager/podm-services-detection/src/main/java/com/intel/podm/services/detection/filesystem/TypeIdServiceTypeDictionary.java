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

package com.intel.podm.services.detection.filesystem;

import com.intel.podm.common.types.ServiceType;

import java.util.Optional;

import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.types.ServiceType.RMM;
import static com.intel.podm.common.types.ServiceType.RSS;
import static java.util.Optional.ofNullable;


public final class TypeIdServiceTypeDictionary {

    private TypeIdServiceTypeDictionary() {
    }

    public static Optional<ServiceType> fromTypeId(String typeId) {
        ServiceType serviceType = null;
        switch (typeId) {
            case "psme": serviceType = PSME; break;
            case "storage": serviceType = RSS; break;
            case "rmm": serviceType = RMM;  break;
            case "lui": serviceType = LUI; break;
            default: break;
        }
        return ofNullable(serviceType);
    }

}
