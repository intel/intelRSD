/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.mappers.processor;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.types.Connectivity;
import com.intel.rsd.nodecomposer.types.InstructionSet;
import com.intel.rsd.nodecomposer.types.ProcessorBrand;
import com.intel.rsd.nodecomposer.types.ProcessorType;

import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Predicate;

import static com.intel.rsd.nodecomposer.business.redfish.services.helpers.ProcessorHelper.getProtocolFromProcessor;
import static com.intel.rsd.nodecomposer.business.redfish.services.helpers.ProcessorHelper.isRemoteFpga;
import static com.intel.rsd.nodecomposer.types.Connectivity.LOCAL;
import static com.intel.rsd.nodecomposer.types.Connectivity.findByProtocol;
import static com.intel.rsd.nodecomposer.utils.Collections.nullOrEmpty;
import static java.lang.Boolean.TRUE;
import static java.util.Objects.isNull;
import static java.util.Optional.of;

class Predicates {
    protected Predicates() {
    }

    public static Predicate<? super Processor> byErasedFlagOrExactUri(ODataId oDataId) {
        return isNull(oDataId)
            ? processor -> TRUE.equals(processor.getProcessorErased())
            : processor -> Objects.equals(processor.getUri(), oDataId);
    }

    public static Predicate<? super Processor> byExactModel(String model) {
        return isNull(model) ? alwaysMatches() : processor -> Objects.equals(processor.getModel(), model);
    }

    public static Predicate<? super Processor> byExactBrand(ProcessorBrand brand) {
        return isNull(brand) ? alwaysMatches() : processor -> Objects.equals(processor.getBrand(), brand);
    }

    public static Predicate<? super Processor> byAtLeastTotalCores(Integer totalCores) {
        return isNull(totalCores) ? alwaysMatches() : processor -> {
            Integer availableTotalCores = processor.getTotalCores();
            return !isNull(availableTotalCores) && availableTotalCores >= totalCores;
        };
    }

    public static Predicate<? super Processor> byAtLeastAchievableSpeedMhz(Integer achievableSpeedMhz) {
        return isNull(achievableSpeedMhz) ? alwaysMatches() : processor -> {
            Integer availableMaxSpeedMhz = processor.getMaxSpeedMhz();
            return !isNull(availableMaxSpeedMhz) && availableMaxSpeedMhz >= achievableSpeedMhz;
        };
    }

    public static Predicate<? super Processor> byExactInstructionSet(InstructionSet instructionSet) {
        return isNull(instructionSet) ? alwaysMatches() : processor -> Objects.equals(processor.getInstructionSet(), instructionSet);
    }

    public static Predicate<? super Processor> byCapabilities(List<String> capabilities) {
        return isNull(capabilities) || capabilities.isEmpty()
            ?
            alwaysMatches()
            :
            processor -> processor.getCapabilities().containsAll(capabilities);
    }

    public static Predicate<? super Processor> byProcessorType(ProcessorType processorType) {
        return isNull(processorType) ? alwaysMatches() : processor -> Objects.equals(processor.getProcessorType(), processorType);
    }

    public static Predicate<? super Processor> byAllowableConnectivities(List<Connectivity> connectivities) {
        return nullOrEmpty(connectivities)
            ? alwaysMatches() : byAllowableConnectivity(connectivities);
    }

    private static Predicate<? super Processor> byAllowableConnectivity(List<Connectivity> requestedConnectivity) {
        return processor -> {
            Optional<Connectivity> supportedConnectivityMethod =
                isRemoteFpga(processor) ? findByProtocol(getProtocolFromProcessor(processor)) : of(LOCAL);
            return supportedConnectivityMethod.isPresent()
                && requestedConnectivity.contains(supportedConnectivityMethod.get());
        };
    }

    private static Predicate<? super Processor> alwaysMatches() {
        return processor -> true;
    }
}
