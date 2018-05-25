/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.redfish.RedfishResource;

public interface ReaderService<T extends RedfishResource> {
    Context SERVICE_ROOT_CONTEXT = null;

    /**
     * Returns collection of context elements which identify resources available in specific context.
     * @param context
     * @return
     * @throws ContextResolvingException if collection was not found in specified context
     */
    CollectionDto getCollection(Context context) throws ContextResolvingException;

    /**
     * Returns representation of a specific Redfish Resource.
     * @param context
     * @return
     * @throws ContextResolvingException if this resource was not found in the specified context
     */
    T getResource(Context context) throws ContextResolvingException;
}
