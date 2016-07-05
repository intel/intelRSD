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
import com.intel.podm.business.dto.redfish.DeviceDto;
import com.intel.podm.business.services.context.Context;

public interface DeviceService {
    /**
     * Returns collection of context elements which identify Devices available under specific System.
     * @param context
     * @return
     * @throws EntityNotFoundException if collection was not found in the context specified by the arguments
     */
    CollectionDto getDevices(Context context) throws EntityNotFoundException;

    /**
     * Returns representation of a specific Device.
     * @param context
     * @return
     * @throws EntityNotFoundException if Device was not found in the context specified by the arguments
     */
    DeviceDto getDevice(Context context) throws EntityNotFoundException;
}
