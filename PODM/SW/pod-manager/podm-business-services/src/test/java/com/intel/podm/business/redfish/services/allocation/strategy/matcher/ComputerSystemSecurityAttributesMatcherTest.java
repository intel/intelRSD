/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.strategy.matcher;

import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import static com.intel.podm.business.redfish.services.allocation.templates.assets.security.AllocationRequestWithSecurityBuilder.requestedNodeSecurityBuilder;
import static com.intel.podm.business.redfish.services.allocation.templates.assets.security.ComputerSystemBuilder.systemInTestBuilder;
import static com.intel.podm.common.types.InterfaceType.TCM1_0;
import static com.intel.podm.common.types.InterfaceType.TPM1_2;
import static com.intel.podm.common.types.InterfaceType.TPM2_0;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName", "checkstyle:MethodCount", "checkstyle:LineLength"})
public class ComputerSystemSecurityAttributesMatcherTest {

    private ComputerSystemSecurityAttributesMatcher computerSystemSecurityAttributesMatcher;

    @BeforeClass
    public void setup() {
        this.computerSystemSecurityAttributesMatcher = new ComputerSystemSecurityAttributesMatcher();
    }

    @Test
    public void whenRequestingComputerSystemWithTxtEnabled_ShouldMatch() {
        assertTrue(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().txtEnabled(true).build(),
            systemInTestBuilder().txtEnabled(true).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithSpecificTpmInterfaceType_ShouldMatch() {
        assertTrue(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().interfaceType(TCM1_0).build(),
            systemInTestBuilder().interfaceTypes(TCM1_0).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithAnyTrustedModulePresent_ShouldMatch() {
        assertTrue(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().tpmPresent(true).build(),
            systemInTestBuilder().interfaceTypes(TCM1_0).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithAnyTrustedModulePresentAndWithSpecificTpmInterfaceType_ShouldMatch() {
        assertTrue(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().interfaceType(TCM1_0).tpmPresent(true).build(),
            systemInTestBuilder().interfaceTypes(TCM1_0).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithTrustedModulePresentAndWithSpecificTpmInterfaceType_ShouldMatch() {
        assertTrue(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().interfaceType(TCM1_0).txtEnabled(true).build(),
            systemInTestBuilder().interfaceTypes(TCM1_0).txtEnabled(true).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithTxtEnabledAndWithAnyTrustedModulePresentAndWithSpecificTpmInterfaceType_ShouldMatch() {
        assertTrue(computerSystemSecurityAttributesMatcher.matches(
            requestedNodeSecurityBuilder().txtEnabled(true).interfaceType(TCM1_0).tpmPresent(true).build(),
            systemInTestBuilder().txtEnabled(true).interfaceTypes(TCM1_0).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithAnyTrustedModuleWhileComputerSystemHasMoreTrustedModule_ShouldMatch() {
        assertTrue(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().tpmPresent(true).build(),
            systemInTestBuilder().interfaceTypes(TPM2_0, TPM1_2).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithSpecificTpmInterfaceTypeWhileComputerSystemContainsMoreThenOneTrustedModule_ShouldMatch() {
        assertTrue(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().interfaceType(TPM1_2).build(),
            systemInTestBuilder().interfaceTypes(TPM2_0, TPM1_2).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithoutTrustedModules_ShouldMatch() {
        assertTrue(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().tpmPresent(false).build(),
            systemInTestBuilder().build()));
    }

    @Test
    public void whenRequestingComputerSystemWithTxtDisabledAndWithSpecificTpmInterfaceType_ShouldMatch() {
        assertTrue(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().txtEnabled(false).interfaceType(TPM1_2).build(),
            systemInTestBuilder().txtEnabled(false).interfaceTypes(TPM2_0, TPM1_2).build()));
    }


    @Test
    public void whenRequestingComputerSystemWithTxtDisabledWhileComputerSystemDoesNotMeetCriteria_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().txtEnabled(false).build(),
            systemInTestBuilder().txtEnabled(true).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithTxtDisabledAndWithAnyTrustedModuleWhileComputerSystemDoesNotMeetCriteria_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().txtEnabled(false).tpmPresent(true).build(),
            systemInTestBuilder().txtEnabled(true).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithAnyTrustedModuleWhileComputerSystemDoesNotHaveAnyTrustedModule_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().tpmPresent(true).build(),
            systemInTestBuilder().build()));
    }

    @Test
    public void whenRequestingComputerSystemWithSpecificTpmInterfaceTypeWhileComputerSystemDoesNotHaveAnyTrustedModule_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().interfaceType(TCM1_0).tpmPresent(true).build(),
            systemInTestBuilder().build()));
    }

    @Test
    public void whenRequestingComputerSystemWithTxtEnabledAndWithSpecificTpmInterfaceTypeWhileComputerSystemDoesNotMeetCriteria_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().txtEnabled(true).interfaceType(TCM1_0).build(),
            systemInTestBuilder().txtEnabled(false).interfaceTypes(TCM1_0).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithTxtEnabledAndWithAnyTrustedModulePresentAndWithSpecificTpmInterfaceTypeWhileComputerSystemDoesNotMeetCriteria_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(
            requestedNodeSecurityBuilder().txtEnabled(true).interfaceType(TCM1_0).tpmPresent(true).build(),
            systemInTestBuilder().txtEnabled(false).interfaceTypes(TCM1_0).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithTxtEnabledAndWithoutTrustedModulesPresentAndWithSpecificTpmInterfaceType_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(
            requestedNodeSecurityBuilder().txtEnabled(true).interfaceType(TCM1_0).tpmPresent(false).build(),
            systemInTestBuilder().txtEnabled(true).interfaceTypes(TPM2_0).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithSpecifiedTpmInterfaceTypeDifferentThenOneComputerSystemContains_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().interfaceType(TCM1_0).build(),
            systemInTestBuilder().interfaceTypes(TPM2_0).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithSpecifiedTpmInterfaceTypeDifferentThenOneComputerSystemContainsAndAnyTrustedModule_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().interfaceType(TCM1_0).tpmPresent(true).build(),
            systemInTestBuilder().interfaceTypes(TPM2_0).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithSpecifiedTpmInterfaceTypeWhileComputerSystemDoeNotHaveTrustedModules_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().interfaceType(TCM1_0).build(),
            systemInTestBuilder().build()));
    }

    @Test
    public void whenRequestingComputerSystemWithTxtEnabledAndAnyTrustedModuleWhileComputerSystemDoesNotMeetCriteria_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().txtEnabled(true).tpmPresent(true).build(),
            systemInTestBuilder().txtEnabled(false).interfaceTypes(TPM2_0).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithTxtEnabledAndAnyTrustedModuleWhileComputerSystemDoesNotHaveAnyTrustedModule_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().txtEnabled(true).tpmPresent(true).build(),
            systemInTestBuilder().txtEnabled(true).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithSpecifiedTpmInterfaceTypeDifferentFromThoseSystemContains_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().interfaceType(TCM1_0).build(),
            systemInTestBuilder().interfaceTypes(TPM2_0, TPM1_2).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithTxtEnabledAndSpecifiedInterfaceTypeWhileComputerSystemDoesNotMeetCriteria_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().txtEnabled(true).interfaceType(TPM1_2).build(),
            systemInTestBuilder().txtEnabled(false).interfaceTypes(TPM2_0, TPM1_2).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithTxtEnabledAndWithoutTrustedModulesWhileComputerSystemDoesNotMeetCriteria_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().txtEnabled(true).tpmPresent(false).build(),
            systemInTestBuilder().txtEnabled(true).interfaceTypes(TPM2_0).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithoutTrustedModulesAndSpecifiedTpmInterfaceTypeWhileComputerSystemDoesNotMeetCriteria_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(requestedNodeSecurityBuilder().interfaceType(TCM1_0).tpmPresent(false).build(),
            systemInTestBuilder().interfaceTypes(TCM1_0).build()));
    }

    @Test
    public void whenRequestingComputerSystemWithTxtEnabledSpecifiedTpmInterfaceTypeAndWithoutTrustedModulesWhileComputerSystemDoesNotMeetCriteria_ShouldFail() {
        assertFalse(computerSystemSecurityAttributesMatcher.matches(
            requestedNodeSecurityBuilder().txtEnabled(true).interfaceType(TCM1_0).tpmPresent(false).build(),
            systemInTestBuilder().txtEnabled(true).interfaceTypes(TCM1_0).build()));
    }
}
