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

package com.intel.podm.business.redfish.services.allocation.validation;

import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.Switch;
import com.intel.podm.common.types.Protocol;

import java.util.Objects;
import java.util.function.Predicate;

import static com.intel.podm.common.types.PciePortType.UPSTREAM_PORT;
import static com.intel.podm.common.types.Protocol.PCIE;
import static java.util.Objects.isNull;

public class Predicates {

    protected Predicates() {
    }

    public static Predicate<Port> byPortPredicate() {
        return port -> Objects.equals(port.getPortType(), UPSTREAM_PORT);
    }

    public static Predicate<Drive> byExactInterface(Protocol protocol) {
        return isNull(protocol) ? drive -> true
            : drive -> Objects.equals(drive.getProtocol(), protocol);
    }

    public static Predicate<Switch> byPcieSwitch() {
        return switchPredicate -> Objects.equals(switchPredicate.getSwitchType(), PCIE);
    }
}
