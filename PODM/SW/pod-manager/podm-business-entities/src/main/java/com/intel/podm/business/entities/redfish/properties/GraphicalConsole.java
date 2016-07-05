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
import com.intel.podm.common.types.GraphicalConnectType;
import com.intel.podm.common.types.helpers.EnumeratedTypeListHolder;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.entities.base.DomainObjectProperties.booleanProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumeratedTypeListProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.common.types.helpers.EnumeratedTypeListHolder.enumeratedTypeListHolder;
import static com.intel.podm.common.types.helpers.EnumeratedTypeListHolder.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class GraphicalConsole extends DomainObject {
    public static final DomainObjectProperty<Boolean> SERVICE_ENABLED = booleanProperty("serviceEnabled");
    public static final DomainObjectProperty<Integer> MAX_CONCURRENT_SESSIONS = integerProperty("maxConcurrentSessions");
    public static final DomainObjectProperty<EnumeratedTypeListHolder<GraphicalConnectType>> CONNECT_TYPES_SUPPORTED =
            enumeratedTypeListProperty("connectTypesSupported", GraphicalConnectType.class);

    public Boolean isServiceEnabled() {
        return getProperty(SERVICE_ENABLED);
    }

    public void setServiceEnabled(Boolean serviceEnabled) {
        setProperty(SERVICE_ENABLED, serviceEnabled);
    }

    public Integer getMaxConcurrentSessions() {
        return getProperty(MAX_CONCURRENT_SESSIONS);
    }

    public void setMaxConcurrentSessions(Integer maxConcurrentSessions) {
        setProperty(MAX_CONCURRENT_SESSIONS, maxConcurrentSessions);
    }

    public List<GraphicalConnectType> getConnectTypesSupported() {
        return toList(GraphicalConnectType.class, getProperty(CONNECT_TYPES_SUPPORTED));
    }

    public void setConnectTypesSupported(List<GraphicalConnectType> connectTypesSupported) {
        setProperty(CONNECT_TYPES_SUPPORTED, enumeratedTypeListHolder(GraphicalConnectType.class, connectTypesSupported));
    }
}
