/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.redfish.json.templates.actions.constraints;

import com.intel.podm.common.types.redfish.RedfishEthernetSwitch;

import javax.validation.Constraint;
import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;
import javax.validation.Payload;
import java.lang.annotation.Retention;
import java.lang.annotation.Target;
import java.util.List;

import static java.lang.annotation.ElementType.PARAMETER;
import static java.lang.annotation.RetentionPolicy.RUNTIME;

import static com.intel.podm.common.types.redfish.RedfishEthernetSwitch.DcbxConfiguration;
import static com.intel.podm.common.types.redfish.RedfishEthernetSwitch.DcbxConfiguration.BandwidthMapping;
import static com.intel.podm.common.types.redfish.RedfishEthernetSwitch.DcbxConfiguration.PriorityClassMapping;
import static com.intel.podm.common.types.redfish.RedfishEthernetSwitch.DcbxConfiguration.ApplicationProtocolType;

@Constraint(validatedBy = EthernetSwitchConstraint.Validator.class)
@Target(PARAMETER)
@Retention(RUNTIME)
public @interface EthernetSwitchConstraint {

    String message() default "Cannot update EthernetSwitch";

    Class<?>[] groups() default {};

    Class<? extends Payload>[] payload() default {};

    @SuppressWarnings({"checkstyle:MethodCount"})
    class Validator implements ConstraintValidator<EthernetSwitchConstraint, RedfishEthernetSwitch> {

        private static final Long BANDWIDTH_PERCENT_MIN = 0L;
        private static final Long BANDWIDTH_PERCENT_MAX = 100L;

        private static final Long BANDWIDTH_PRIORITY_GROUP_MIN = 0L;
        private static final Long BANDWIDTH_PRIORITY_GROUP_MAX = 15L;

        private static final Long PRIORITY_GROUP_MIN = 0L;
        private static final Long PRIORITY_GROUP_MAX = 7L;

        private static final Long PORT_MIN = 0L;
        private static final Long PORT_MAX = 65635L;

        @Override
        public void initialize(EthernetSwitchConstraint constraintAnnotation) {
        }

        @Override
        public boolean isValid(RedfishEthernetSwitch ethernetSwitch, ConstraintValidatorContext context) {
            DcbxConfiguration dcbxSharedConfiguration = ethernetSwitch.getDcbxSharedConfiguration();
            return dcbxSharedConfiguration == null
                || areApplicationProtocolsValid(dcbxSharedConfiguration.getApplicationProtocolList())
                    && arePriorityToPriorityGroupMappingsValid(dcbxSharedConfiguration.getPriorityToPriorityGroupMappingList())
                    && areBandwidthAllocationsValid(dcbxSharedConfiguration.getBandwidthAllocationList());
        }

        private boolean areBandwidthAllocationsValid(List<? extends BandwidthMapping> bandwidthAllocations) {
            return bandwidthAllocations == null || bandwidthAllocations.stream().map(this::isValid).allMatch(r -> true);
        }

        private boolean isValid(BandwidthMapping bandwidthMapping) {
            return isBandwidthPercentValid(bandwidthMapping.getBandwidthPercent())
                && isBandwidthPriorityGroupValid(bandwidthMapping.getPriorityGroup());
        }

        private boolean arePriorityToPriorityGroupMappingsValid(List<? extends PriorityClassMapping> priorityToPriorityGroupMappings) {
            return priorityToPriorityGroupMappings == null || priorityToPriorityGroupMappings.stream().map(this::isValid).allMatch(r -> true);
        }

        private boolean areApplicationProtocolsValid(List<? extends ApplicationProtocolType> applicationProtocols) {
            return applicationProtocols == null || applicationProtocols.stream().map(this::isValid).allMatch(r -> true);
        }

        private boolean isValid(ApplicationProtocolType applicationProtocolType) {
            return isPriorityValid(applicationProtocolType.getPriority())
                && isPortValid(applicationProtocolType.getPort());
        }

        private boolean isValid(PriorityClassMapping priorityToPriorityGroupMapping) {
            return isPriorityGroupValid(priorityToPriorityGroupMapping.getPriorityGroup())
                && isPriorityValid(priorityToPriorityGroupMapping.getPriority());
        }

        private boolean isBandwidthPercentValid(Long bandwidthPercent) {
            return bandwidthPercent == null || bandwidthPercent >= BANDWIDTH_PERCENT_MIN && bandwidthPercent <= BANDWIDTH_PERCENT_MAX;
        }

        private boolean isBandwidthPriorityGroupValid(Long priorityGroup) {
            return priorityGroup == null || priorityGroup >= BANDWIDTH_PRIORITY_GROUP_MIN && priorityGroup <= BANDWIDTH_PRIORITY_GROUP_MAX;
        }

        private boolean isPriorityGroupValid(Long priorityGroup) {
            return priorityGroup == null || priorityGroup >= PRIORITY_GROUP_MIN && priorityGroup <= PRIORITY_GROUP_MAX;
        }

        private boolean isPortValid(Long port) {
            return port == null || port >= PORT_MIN && port <= PORT_MAX;
        }

        private boolean isPriorityValid(Long priority) {
            return priority == null || priority >= PRIORITY_GROUP_MIN && priority <= PRIORITY_GROUP_MAX;
        }
    }
}
