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

package com.intel.podm.business.entities.redfish.properties;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.helpers.EnumeratedTypeListHolder;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumeratedTypeListProperty;
import static com.intel.podm.common.types.helpers.EnumeratedTypeListHolder.enumeratedTypeListHolder;
import static com.intel.podm.common.types.helpers.EnumeratedTypeListHolder.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class Boot extends DomainObject {
    public static final DomainObjectProperty<BootSourceState> BOOT_SOURCE_OVERRIDE_ENABLED = enumProperty("bootSourceOverrideEnabled", BootSourceState.class);
    public static final DomainObjectProperty<BootSourceType> BOOT_SOURCE_OVERRIDE_TARGET = enumProperty("bootSourceOverrideTarget", BootSourceType.class);
    public static final DomainObjectProperty<EnumeratedTypeListHolder<BootSourceType>>
            BOOT_SOURCE_TYPES = enumeratedTypeListProperty("bootSourceTypes", BootSourceType.class);

    public BootSourceState getBootSourceOverrideEnabled() {
        return getProperty(BOOT_SOURCE_OVERRIDE_ENABLED);
    }

    public void setBootSourceOverrideEnabled(BootSourceState bootSourceOverrideEnabled) {
        setProperty(BOOT_SOURCE_OVERRIDE_ENABLED, bootSourceOverrideEnabled);
    }

    public BootSourceType getBootSourceOverrideTarget() {
        return getProperty(BOOT_SOURCE_OVERRIDE_TARGET);
    }

    public void setBootSourceOverrideTarget(BootSourceType bootSourceOverrideTarget) {
        setProperty(BOOT_SOURCE_OVERRIDE_TARGET, bootSourceOverrideTarget);
    }

    public List<BootSourceType> getBootSourceOverrideTargetAllowableValues() {
        return toList(BootSourceType.class, getProperty(BOOT_SOURCE_TYPES));
    }

    public void setBootSourceOverrideTargetAllowableValues(List<BootSourceType> bootSourcetypes) {
        setProperty(BOOT_SOURCE_TYPES, enumeratedTypeListHolder(BootSourceType.class, bootSourcetypes));
    }
}
