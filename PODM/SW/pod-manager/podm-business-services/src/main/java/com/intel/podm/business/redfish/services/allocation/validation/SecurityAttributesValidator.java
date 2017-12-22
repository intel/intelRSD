/*
 * Copyright (c) 2017 Intel Corporation
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

import com.intel.podm.business.Violations;
import com.intel.podm.business.services.redfish.requests.RequestedNode;

import javax.enterprise.context.Dependent;

import static java.lang.Boolean.FALSE;
import static java.lang.String.format;

@Dependent
public class SecurityAttributesValidator {

    public Violations validate(RequestedNode.Security security) {
        Violations violations = new Violations();

        if (security == null) {
            return violations;
        }

        if (isSystemWithoutTpmRequested(security) && isDesiredTpmInterfaceTypeSpecified(security)) {
            violations.addViolation(format("Inconsistent request: request contains both: without TPM (TpmPresent: false) "
                + "and with specific TPM Interface Type (TpmInterfaceType : %s)", security.getTpmInterfaceType()));
        }
        return violations;
    }

    private boolean isDesiredTpmInterfaceTypeSpecified(RequestedNode.Security security) {
        return security.getTpmInterfaceType() != null;
    }

    private boolean isSystemWithoutTpmRequested(RequestedNode.Security security) {
        return FALSE.equals(security.getTpmPresent());
    }
}
