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

package com.intel.podm.business.services.redfish;

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.PowerZoneDto;
import com.intel.podm.business.services.context.Context;

public interface PowerZoneService {

    /**
     * Returns collection of context elements which identify PowerZones available under specific Rack.
     * @param chassisContext
     * @return
     * @throws EntityNotFoundException if Power Zone collection is not found in the context specified by the argument
     */
    CollectionDto getPowerZonesCollection(Context chassisContext) throws EntityNotFoundException;

    /**
     * Returns representation of a specific Power Zone.
     * @param powerZoneContext
     * @return
     * @throws EntityNotFoundException if Power Zone is not found in the context specified by the argument
     */
    PowerZoneDto getPowerZone(Context powerZoneContext) throws EntityNotFoundException;
}
