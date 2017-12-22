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

package com.intel.podm.redfish.json.templates.actions;

import com.intel.podm.redfish.json.templates.actions.constraints.ComposedNodeConstraint.ComposedNodeConstraintValidator;
import org.testng.annotations.Test;

import static com.intel.podm.common.types.BootSourceState.CONTINUOUS;
import static com.intel.podm.common.types.BootSourceType.BIOS_SETUP;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName"})
public class ComposedNodeConstraintValidatorTest {

    @Test
    public void whenEmptyJsonProvided_shouldReturnFalse() throws Exception {
        ComposedNodeConstraintValidator composedNodeConstraintValidator = new ComposedNodeConstraintValidator();
        ComposedNodePartialRepresentation json = new ComposedNodePartialRepresentation();
        assertFalse(composedNodeConstraintValidator.isValid(json, null));
    }

    @Test
    public void whenJsonWithEmptyBootProvided_shouldReturnFalse() throws Exception {
        ComposedNodeConstraintValidator composedNodeConstraintValidator = new ComposedNodeConstraintValidator();
        ComposedNodePartialRepresentation json = new ComposedNodePartialRepresentation();
        json.boot = new ComposedNodePartialRepresentation.Boot();
        assertFalse(composedNodeConstraintValidator.isValid(json, null));
    }

    @Test
    public void whenJsonWithBootSourceTypeProvided_shouldReturnTrue() throws Exception {
        ComposedNodeConstraintValidator composedNodeConstraintValidator = new ComposedNodeConstraintValidator();
        ComposedNodePartialRepresentation json = new ComposedNodePartialRepresentation();
        json.boot = new ComposedNodePartialRepresentation.Boot();
        json.boot.bootSourceOverrideTarget = BIOS_SETUP;
        assertTrue(composedNodeConstraintValidator.isValid(json, null));
    }

    @Test
    public void whenJsonWithBootSourceStateProvided_shouldReturnTrue() throws Exception {
        ComposedNodeConstraintValidator composedNodeConstraintValidator = new ComposedNodeConstraintValidator();
        ComposedNodePartialRepresentation json = new ComposedNodePartialRepresentation();
        json.boot = new ComposedNodePartialRepresentation.Boot();
        json.boot.bootSourceOverrideEnabled = CONTINUOUS;
        assertTrue(composedNodeConstraintValidator.isValid(json, null));
    }

    @Test
    public void whenValidJsonProvided_shouldReturnTrue() throws Exception {
        ComposedNodePartialRepresentation json = new ComposedNodePartialRepresentation();
        json.boot = new ComposedNodePartialRepresentation.Boot();
        json.boot.bootSourceOverrideEnabled = CONTINUOUS;
        json.boot.bootSourceOverrideTarget = BIOS_SETUP;
        ComposedNodeConstraintValidator composedNodeConstraintValidator = new ComposedNodeConstraintValidator();
        assertTrue(composedNodeConstraintValidator.isValid(json, null));
    }
}
