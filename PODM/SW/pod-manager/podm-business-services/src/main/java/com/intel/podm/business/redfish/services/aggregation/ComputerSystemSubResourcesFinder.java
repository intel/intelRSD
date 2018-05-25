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

package com.intel.podm.business.redfish.services.aggregation;

import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.MultiSourceResource;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.function.Function;

import static java.util.Collections.emptyMap;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class ComputerSystemSubResourcesFinder {
    @Inject
    private ComputerSystemDao computerSystemDao;

    @Transactional(MANDATORY)
    public Collection<? extends DiscoverableEntity> getUniqueSubResourcesOfClass(ComputerSystem baseEntity,
                                                                                 Class<? extends DiscoverableEntity> subResourceClass) {
        List<ComputerSystem> complementarySystems = computerSystemDao.findComplementarySystems(baseEntity);
        Map<String, ? extends DiscoverableEntity> subResources;
        if (Objects.equals(subResourceClass, Storage.class)) {
            subResources = getStringMap(baseEntity, complementarySystems, ComputerSystem::getStorages);
        } else if (Objects.equals(subResourceClass, SimpleStorage.class)) {
            subResources = getStringMap(baseEntity, complementarySystems, ComputerSystem::getSimpleStorages);
        } else if (Objects.equals(subResourceClass, EthernetInterface.class)) {
            subResources = getStringMap(baseEntity, complementarySystems, ComputerSystem::getEthernetInterfaces);
        } else if (Objects.equals(subResourceClass, Memory.class)) {
            subResources = getStringMap(baseEntity, complementarySystems, ComputerSystem::getMemoryModules);
        } else if (Objects.equals(subResourceClass, Processor.class)) {
            subResources = getStringMap(baseEntity, complementarySystems, ComputerSystem::getProcessors);
        } else {
            subResources = emptyMap();
        }
        return subResources.values();
    }

    private <T extends MultiSourceResource> Map<String, T> getStringMap(ComputerSystem system, List<ComputerSystem> complementarySystems,
                                                                        Function<ComputerSystem, Set<T>> func) {
        Map<String, T> map = new HashMap<>();
        for (T item : func.apply(system)) {
            map.put(item.getMultiSourceDiscriminator(), item);
        }

        for (ComputerSystem complementarySystem : complementarySystems) {
            for (T complementaryItem : func.apply(complementarySystem)) {
                map.putIfAbsent(complementaryItem.getMultiSourceDiscriminator(), complementaryItem);
            }
        }
        return map;
    }
}
