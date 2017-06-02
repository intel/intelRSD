/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.client.api.resources;

import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;

/**
 * Thermal zone details provided by RMM (if present).
 */
public interface ThermalZoneResource extends ExternalServiceResource {
    String getThermalZoneId();
    int getPresentTemperatureDegC();
    int getMaxFansSupported();
    int getNumberOfFansPresent();
    Integer getDesiredSpeedRpm();
    Integer getDesiredSpeedPwm();
    InRackLocationObject getInRackLocation();

    Iterable<ResourceSupplier> getFans() throws ExternalServiceApiReaderException;
}
