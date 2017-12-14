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

package com.intel.podm.business.redfish.services.allocation.mappers.processor;

import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;

import java.util.List;
import java.util.Objects;
import java.util.function.Predicate;

import static java.util.Objects.isNull;

class Predicates {
    protected Predicates() {
    }

    public static Predicate<? super Processor> byExactId(Context context) {
        return isNull(context) ? alwaysMatches() : processor -> Objects.equals(processor.getId(), context.getId());
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

    public static Predicate<? super Processor> byProcessorType(ProcessorType proccesorType) {
        return isNull(proccesorType) ? alwaysMatches() : processor -> Objects.equals(processor.getProcessorType(), proccesorType);
    }

    private static Predicate<? super Processor> alwaysMatches() {
        return processor -> true;
    }
}
