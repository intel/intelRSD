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

package com.intel.podm.redfish.json.templates.actions.constraints;

import org.apache.commons.validator.routines.IntegerValidator;

public final class VlanIdValidationHelper {
    private static final int MAX_PRIMARY_VLAN_ID_VALUE = 4094;

    private VlanIdValidationHelper() {
    }

    static boolean isVlanIdValid(Integer vlanId) {
        return vlanId == null || IntegerValidator.getInstance().isInRange(vlanId, 0, MAX_PRIMARY_VLAN_ID_VALUE);
    }
}
