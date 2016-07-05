/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.business.entities.redfish.properties;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class MemoryLocation extends DomainObject {
    public static final DomainObjectProperty<Integer> SOCKET = integerProperty("socket");
    public static final DomainObjectProperty<Integer> MEMORY_CONTROLLER = integerProperty("memoryController");
    public static final DomainObjectProperty<Integer> CHANNEL = integerProperty("channel");
    public static final DomainObjectProperty<Integer> SLOT = integerProperty("slot");

    public Integer getSocket() {
        return getProperty(SOCKET);
    }

    public void setSocket(Integer locationSocket) {
        setProperty(SOCKET, locationSocket);
    }

    public Integer getMemoryController() {
        return getProperty(MEMORY_CONTROLLER);
    }

    public void setMemoryController(Integer locationMemoryController) {
        setProperty(MEMORY_CONTROLLER, locationMemoryController);
    }

    public Integer getChannel() {
        return getProperty(CHANNEL);
    }

    public void setChannel(Integer locationChannel) {
        setProperty(CHANNEL, locationChannel);
    }

    public Integer getSlot() {
        return getProperty(SLOT);
    }

    public void setSlot(Integer locationSlot) {
        setProperty(SLOT, locationSlot);
    }
}
