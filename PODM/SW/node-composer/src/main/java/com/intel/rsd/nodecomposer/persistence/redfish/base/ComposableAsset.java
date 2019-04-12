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

package com.intel.rsd.nodecomposer.persistence.redfish.base;

import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;

public interface ComposableAsset {
    /**
     * Resource is available when it's state is Enabled and health is Ok.
     *
     * @return true when resource is available.
     */
    boolean isAvailable();
    /**
     * Resource is degraded when it's state is missing or state is not Enabled or health is Critical.
     *
     * @return true when resource is degraded.
     */
    boolean isDegraded();
    /**
     * Resource is achievable when connection between PODM and underlying service is alive.
     *
     * @return true when resource is achievable.
     */
    boolean isAchievable();
    /**
     * @param achievable describes whether resource is achievable or not.
     */
    void setAchievable(boolean achievable);
    /**
     * @return ComposedNode assigned to this resource.
     */
    ComposedNode getComposedNode();
}
