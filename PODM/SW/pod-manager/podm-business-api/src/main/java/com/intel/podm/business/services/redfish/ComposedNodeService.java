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

package com.intel.podm.business.services.redfish;

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.ComposedNodeDto;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.actions.ResetType;

public interface ComposedNodeService {
    CollectionDto getComposedNodeCollection();
    ComposedNodeDto getComposedNode(Context composedNodeContext) throws EntityNotFoundException;
    Context createComposedNode(RequestedNode requestedNode) throws EntityOperationException;
    void assembleComposedNode(Context composedNodeContext) throws EntityNotFoundException, EntityOperationException;
    void reset(Context composedNodeContext, ResetType resetType)
            throws EntityNotFoundException, EntityOperationException, IllegalStateException, IllegalArgumentException;
    void overrideBootSource(Context composedNodeContext, BootSourceType type, BootSourceState state)
            throws EntityNotFoundException, EntityOperationException, IllegalStateException;
    void deleteComposedNode(Context composedNodeContext) throws EntityNotFoundException, EntityOperationException;
    void terminateComposedNodeTasks(Context composedNodeContext);
}
