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

import com.intel.podm.common.types.redfish.RedfishComputerSystem;
import com.intel.podm.common.types.redfish.RedfishComputerSystem.Boot;
import com.intel.podm.redfish.json.templates.actions.constraints.ComputerSystemConstraint.ComputerSystemConstraintValidator;
import org.testng.annotations.Test;

import static com.intel.podm.common.types.BootSourceState.CONTINUOUS;
import static com.intel.podm.common.types.BootSourceType.BIOS_SETUP;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName"})
public class ComputerSystemConstraintValidatorTest {
    private static final String DUMMY_DATA = "DUMMY_DATA";

    @Test
    public void whenEmptyJsonProvided_shouldReturnFalse() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();

        RedfishComputerSystem json = mock(RedfishComputerSystem.class);

        assertFalse(computerSystemConstraintValidator.isValid(json, null));
    }

    @Test
    public void whenJsonWithEmptyBootProvided_shouldReturnFalse() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();

        RedfishComputerSystem json = prepareRedfishComputerSystemWithBoot();

        assertFalse(computerSystemConstraintValidator.isValid(json, null));
    }

    @Test
    public void whenJsonWithEmptyAssetTag_shouldReturnFalse() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();

        RedfishComputerSystem json = mock(RedfishComputerSystem.class);
        when(json.getAssetTag()).thenReturn(null);

        assertFalse(computerSystemConstraintValidator.isValid(json, null));
    }

    @Test
    public void whenJsonWithBootSourceTypeProvided_shouldReturnTrue() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();

        RedfishComputerSystem json = prepareRedfishComputerSystemWithBoot();
        when(json.getBoot().getBootSourceOverrideTarget()).thenReturn(BIOS_SETUP);

        assertTrue(computerSystemConstraintValidator.isValid(json, null));
    }

    @Test
    public void whenJsonWithBootSourceStateProvided_shouldReturnTrue() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();

        RedfishComputerSystem json = prepareRedfishComputerSystemWithBoot();
        when(json.getBoot().getBootSourceOverrideEnabled()).thenReturn(CONTINUOUS);

        assertTrue(computerSystemConstraintValidator.isValid(json, null));
    }

    @Test
    public void whenJsonWithAssetTagProvided_shouldReturnTrue() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();

        RedfishComputerSystem json = mock(RedfishComputerSystem.class);
        when(json.getAssetTag()).thenReturn(DUMMY_DATA);

        assertTrue(computerSystemConstraintValidator.isValid(json, null));
    }

    @Test
    public void whenValidJsonProvided_shouldReturnTrue() throws Exception {
        RedfishComputerSystem json = prepareRedfishComputerSystemWithBoot();
        when(json.getAssetTag()).thenReturn(DUMMY_DATA);
        when(json.getBoot().getBootSourceOverrideEnabled()).thenReturn(CONTINUOUS);
        when(json.getBoot().getBootSourceOverrideTarget()).thenReturn(BIOS_SETUP);

        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();

        assertTrue(computerSystemConstraintValidator.isValid(json, null));
    }

    private RedfishComputerSystem prepareRedfishComputerSystemWithBoot() {
        RedfishComputerSystem json = mock(RedfishComputerSystem.class);
        Boot boot = mock(Boot.class);

        when(json.getBoot()).thenReturn(boot);

        return json;
    }
}
