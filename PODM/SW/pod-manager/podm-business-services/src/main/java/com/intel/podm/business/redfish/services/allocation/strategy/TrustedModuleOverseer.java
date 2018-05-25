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

package com.intel.podm.business.redfish.services.allocation.strategy;

import com.intel.podm.business.entities.redfish.embeddables.TrustedModule;
import com.intel.podm.business.services.redfish.requests.RequestedNode.Security;
import com.intel.podm.common.types.InterfaceType;
import com.intel.podm.common.types.actions.ChangeTpmStatusUpdateDefinition;

import javax.enterprise.context.ApplicationScoped;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.types.EnumeratedType.byPriority;
import static java.lang.Boolean.TRUE;
import static java.util.Comparator.comparing;

@ApplicationScoped
public class TrustedModuleOverseer {
    ChangeTpmStatusUpdateDefinition prepareChangeTpmStateUpdateDefinition(Security security, List<TrustedModule> trustedModules) {
        return createProperUpdateDefinition(security, trustedModules);
    }

    private ChangeTpmStatusUpdateDefinition createProperUpdateDefinition(Security security, List<TrustedModule> trustedModules) {
        InterfaceType tpmInterfaceType = security.getTpmInterfaceType();

        if (tpmInterfaceType != null) {
            return createUpdateDefinitionWhenInterfaceTypePresent(trustedModules, tpmInterfaceType);
        } else {
            return createUpdateDefinitionWhenTpmPresent(trustedModules);
        }
    }

    private ChangeTpmStatusUpdateDefinition createUpdateDefinitionWhenTpmPresent(List<TrustedModule> trustedModules) {
        List<TrustedModule> sortedTrustedModules = getSortedCopyOfTrustedModules(trustedModules);
        return createUpdateDefinition(sortedTrustedModules.remove(0), TRUE);
    }

    private ChangeTpmStatusUpdateDefinition createUpdateDefinition(TrustedModule trustedModule, Boolean deviceEnabled) {
        return new ChangeTpmStatusUpdateDefinition(
            deviceEnabled, trustedModule.getInterfaceType(), null);
    }

    private List<TrustedModule> getSortedCopyOfTrustedModules(List<TrustedModule> trustedModules) {
        List<TrustedModule> sortedTrustedModules = new ArrayList<>(trustedModules);
        sortedTrustedModules.sort(comparing(TrustedModule::getInterfaceType, byPriority()));
        return sortedTrustedModules;
    }

    private ChangeTpmStatusUpdateDefinition createUpdateDefinitionWhenInterfaceTypePresent(List<TrustedModule> trustedModules,
                                                                                           InterfaceType tpmInterfaceType) {
        TrustedModule trustedModuleOfRequestedType = trustedModules.stream()
            .filter(trustedModule -> tpmInterfaceType.equals(trustedModule.getInterfaceType()))
            .findFirst()
            .orElseThrow(() -> new RuntimeException("ChangeTpmTask could not be prepared."
               + "Allocation algorithm returned computer system that does not meet criteria"));

        return new ChangeTpmStatusUpdateDefinition(TRUE, trustedModuleOfRequestedType.getInterfaceType(), null);
    }
}
