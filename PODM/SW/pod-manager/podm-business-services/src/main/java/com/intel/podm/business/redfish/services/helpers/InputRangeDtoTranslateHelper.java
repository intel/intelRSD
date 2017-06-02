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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.business.dto.redfish.attributes.InputRangeDto;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.embeddables.InputRange;

import javax.inject.Inject;
import java.util.Collection;
import java.util.List;

import static java.util.stream.Collectors.toList;

public class InputRangeDtoTranslateHelper {
    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    public List<InputRangeDto> translateInputRangesToDto(ExternalService externalService, Collection<InputRange> inputRanges) {
        return inputRanges.stream()
            .map(inputRange -> buildInputRangeDto(externalService, inputRange))
            .collect(toList());
    }

    private InputRangeDto buildInputRangeDto(ExternalService externalService, InputRange inputRange) {
        return InputRangeDto.newPowerSupplyDto()
            .inputType(inputRange.getInputType())
            .minimumVoltage(inputRange.getMinimumVoltage())
            .maximumVoltage(inputRange.getMaximumVoltage())
            .minimumFrequencyHz(inputRange.getMinimumFrequencyHz())
            .maximumFrequencyHz(inputRange.getMaximumFrequencyHz())
            .outputWattage(inputRange.getOutputWattage())
            .oem(unknownOemTranslator.translateStringOemToDto(externalService, inputRange.getOem()))
            .build();
    }
}
