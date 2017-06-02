/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.client.api.resources.redfish;

import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.ReferenceableMember;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;

public interface ThermalResource extends ExternalServiceResource {

    Iterable<ResourceSupplier> getTemperatures() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getFans() throws ExternalServiceApiReaderException;

    @SuppressWarnings({"checkstyle:MethodCount"})
    interface Fan extends ExternalServiceResource, ReferenceableMember {
        String getName();
        PhysicalContext getPhysicalContext();
        Status getStatus();
        Integer getReading();
        String getReadingUnits();
        Integer getUpperThresholdNonCritical();
        Integer getUpperThresholdCritical();
        Integer getUpperThresholdFatal();
        Integer getLowerThresholdNonCritical();
        Integer getLowerThresholdCritical();
        Integer getLowerThresholdFatal();
        Integer getMinReadingRange();
        Integer getMaxReadingRange();

        Iterable<ResourceSupplier> relatedItems() throws ExternalServiceApiReaderException;
        Iterable<ResourceSupplier> redundancy() throws ExternalServiceApiReaderException;
    }

    @SuppressWarnings({"checkstyle:MethodCount"})
    interface Temperature  extends ExternalServiceResource, ReferenceableMember {
        Integer getSensorNumber();
        Status getStatus();
        BigDecimal getReadingCelsius();
        BigDecimal getUpperThresholdNonCritical();
        BigDecimal getUpperThresholdCritical();
        BigDecimal getUpperThresholdFatal();
        BigDecimal getLowerThresholdNonCritical();
        BigDecimal getLowerThresholdCritical();
        BigDecimal getLowerThresholdFatal();
        BigDecimal getMinReadingRange();
        BigDecimal getMaxReadingRange();
        PhysicalContext getPhysicalContext();

        Iterable<ResourceSupplier> relatedItems() throws ExternalServiceApiReaderException;
    }
}
