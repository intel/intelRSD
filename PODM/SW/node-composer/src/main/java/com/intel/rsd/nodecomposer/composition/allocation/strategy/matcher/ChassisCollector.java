/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationRequestProcessingException;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.Chassis;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.HashSet;
import java.util.Set;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class ChassisCollector {
    private final GenericDao genericDao;

    @Autowired
    public ChassisCollector(GenericDao genericDao) {
        this.genericDao = genericDao;
    }

    public Set<Chassis> getChassisFromODataIds(Set<ODataId> chassisODataIds) throws AllocationRequestProcessingException {
        Set<Chassis> chassisSet = new HashSet<>();
        Violations violations = new Violations();

        for (ODataId chassisODataId : chassisODataIds) {
            chassisSet.add(genericDao.tryFind(Chassis.class, chassisODataId).orElseThrow(() ->
                new AllocationRequestProcessingException(
                    violations.addViolation("Specified chassis (" + chassisODataId + ") does not exist.")
                )
            ));
        }

        return chassisSet;
    }
}
