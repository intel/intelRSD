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
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.LineInputVoltageType;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.PowerInputType;
import com.intel.podm.common.types.PowerLimitException;
import com.intel.podm.common.types.PowerSupplyType;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;
import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount"})
public interface PowerResource extends ExternalServiceResource {
    Iterable<ResourceSupplier> getPowerControls() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getVoltages() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getRedundancy() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getPowerSupplies() throws ExternalServiceApiReaderException;

    interface PowerControl extends ExternalServiceResource, ReferenceableMember {
        @Override
        String getMemberId();
        Integer getPowerConsumedWatts();
        Status getStatus();
        Integer getPowerRequestedWatts();
        Integer getPowerAvailableWatts();
        Integer getPowerCapacityWatts();
        Integer getPowerAllocatedWatts();
        PowerMetrics getPowerMetrics();
        PowerLimit getPowerLimit();
        Iterable<ResourceSupplier> relatedItems() throws ExternalServiceApiReaderException;
        String getOem();

        interface PowerLimit {
            Integer getLimitInWatts();
            PowerLimitException getLimitException();
            Integer getCorrectionInMs();
        }

        interface PowerMetrics {
            Integer getIntervalInMin();
            Integer getMinConsumedWatts();
            Integer getMaxConsumedWatts();
            Integer getAverageConsumedWatts();
        }
    }

    interface Voltage extends ExternalServiceResource, ReferenceableMember {
        @Override
        String getMemberId();
        @Override
        String getName();
        Long getSensorNumber();
        Status getStatus();
        BigDecimal getReadingVolts();
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

    interface PowerSupply extends ExternalServiceResource, ReferenceableMember {
        @Override
        String getMemberId();
        @Override
        String getName();
        Status getStatus();
        String getOem();
        PowerSupplyType getPowerSupplyType();
        LineInputVoltageType getLineInputVoltageType();
        BigDecimal getLineInputVoltage();
        BigDecimal getPowerCapacityWatts();
        BigDecimal getLastPowerOutputWatts();
        String getModel();
        String getManufacturer();
        String getFirmwareVersion();
        String getSerialNumber();
        String getPartNumber();
        String getSparePartNumber();
        List<InputRange> getInputRanges();
        IndicatorLed getIndicatorLed();
        Iterable<ResourceSupplier> redundancy() throws ExternalServiceApiReaderException;
        Iterable<ResourceSupplier> relatedItems() throws ExternalServiceApiReaderException;

        interface InputRange {
            PowerInputType getInputType();
            BigDecimal getMinimumVoltage();
            BigDecimal getMaximumVoltage();
            BigDecimal getOutputWattage();
            BigDecimal getMinimumFrequencyHz();
            BigDecimal getMaximumFrequencyHz();
            String getOem();
        }
    }
}
