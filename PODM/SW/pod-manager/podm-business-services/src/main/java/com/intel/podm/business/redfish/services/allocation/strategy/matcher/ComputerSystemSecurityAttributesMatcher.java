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

package com.intel.podm.business.redfish.services.allocation.strategy.matcher;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.embeddables.TrustedModule;
import com.intel.podm.business.services.redfish.requests.RequestedNode.Security;
import com.intel.podm.common.types.InterfaceType;

import javax.enterprise.context.Dependent;
import java.util.List;
import java.util.Objects;

import static java.lang.Boolean.FALSE;

@Dependent
public class ComputerSystemSecurityAttributesMatcher {
    public boolean matches(Security requestedSecurity, ComputerSystem computerSystem) {
        if (requestedSecurity == null) {
            return true;
        }

        return checkTpmRequirements(requestedSecurity, computerSystem.getTrustedModules())
            && isTrustedExecutionTechnologyRequirementFulfilled(requestedSecurity.getTxtEnabled(), computerSystem.getTrustedExecutionTechnologyEnabled());
    }

    private boolean checkTpmRequirements(Security security, List<TrustedModule> trustedModules) {
        if (security.getTpmPresent() == null) {
            return isRequestedSpecificTpmInterfaceType(security, trustedModules);
        } else {
            return areTpmRequirementsFulfilled(security, trustedModules);
        }
    }

    private boolean areTpmRequirementsFulfilled(Security security, List<TrustedModule> trustedModules) {
        if (FALSE.equals(security.getTpmPresent())) {
            return trustedModules.isEmpty();
        } else {
            return isRequestedTpmModuleToBePresent(security, trustedModules);
        }
    }

    private boolean isRequestedTpmModuleToBePresent(Security security, List<TrustedModule> trustedModules) {
        if (trustedModules.isEmpty()) {
            return false;
        } else {
            return isRequestedSpecificTpmInterfaceType(security, trustedModules);
        }
    }

    private boolean isRequestedSpecificTpmInterfaceType(Security security, List<TrustedModule> trustedModules) {
        InterfaceType requestedTpmInterfaceType = security.getTpmInterfaceType();
        if (requestedTpmInterfaceType == null) {
            return true;
        }
        return anyTrustedModuleMatchesRequestedInterfaceType(trustedModules, requestedTpmInterfaceType);
    }

    private boolean anyTrustedModuleMatchesRequestedInterfaceType(List<TrustedModule> trustedModules, InterfaceType requestedTpmInterfaceType) {
        return trustedModules.stream()
            .map(TrustedModule::getInterfaceType)
            .anyMatch(requestedTpmInterfaceType::equals);
    }

    private boolean isTrustedExecutionTechnologyRequirementFulfilled(Boolean requestedNodeTxtEnabled, Boolean computerSystemTxtEnabled) {
        if (requestedNodeTxtEnabled == null) {
            return true;
        }

        return Objects.equals(computerSystemTxtEnabled, requestedNodeTxtEnabled)
            || (FALSE.equals(requestedNodeTxtEnabled) && computerSystemTxtEnabled == null);
    }
}
