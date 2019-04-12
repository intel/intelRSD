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

package com.intel.rsd.nodecomposer.composition.allocation.strategy;

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.Security;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.TrustedModule;
import com.intel.rsd.nodecomposer.types.InterfaceType;
import com.intel.rsd.nodecomposer.types.actions.ChangeTpmStatusUpdateDefinition;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.List;

import static com.intel.rsd.nodecomposer.types.EnumeratedType.byPriority;
import static java.lang.Boolean.TRUE;
import static java.util.Comparator.comparing;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
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
