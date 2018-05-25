/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.common.types.Ref;
import com.intel.podm.redfish.json.templates.actions.constraints.ComputerSystemConstraintValidator;
import org.mockito.Mockito;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import javax.validation.ConstraintValidatorContext;
import javax.validation.ConstraintValidatorContext.ConstraintViolationBuilder;
import java.util.HashSet;
import java.util.Set;

import static com.intel.podm.common.types.BootSourceState.CONTINUOUS;
import static com.intel.podm.common.types.BootSourceType.BIOS_SETUP;
import static com.intel.podm.common.types.InterfaceType.TPM2_0;
import static com.intel.podm.common.types.redfish.RedfishComputerSystem.TrustedModule;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName"})
public class ComputerSystemConstraintValidatorTest {
    private static final String DUMMY_DATA = "DUMMY_DATA";

    private ConstraintValidatorContext context;
    private ConstraintViolationBuilder constraintViolationBuilder;

    @BeforeClass
    private void prepareContext() {
        context = mock(ConstraintValidatorContext.class);
        constraintViolationBuilder = mock(ConstraintViolationBuilder.class);
        when(context.buildConstraintViolationWithTemplate(Mockito.anyString())).thenReturn(constraintViolationBuilder);
    }

    @Test
    public void whenEmptyJsonProvided_shouldReturnFalse() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();
        ComputerSystemPartialRepresentation json = new ComputerSystemPartialRepresentation();
        assertFalse(computerSystemConstraintValidator.isValid(json, context));
    }

    @Test
    public void whenJsonWithEmptyAssetTag_shouldReturnFalse() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();
        ComputerSystemPartialRepresentation json = new ComputerSystemPartialRepresentation();
        json.assetTag = null;
        assertFalse(computerSystemConstraintValidator.isValid(json, context));
    }

    @Test
    public void whenJsonWithAssetTagProvided_shouldReturnTrue() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();
        ComputerSystemPartialRepresentation json = new ComputerSystemPartialRepresentation();
        json.assetTag = DUMMY_DATA;
        assertTrue(computerSystemConstraintValidator.isValid(json, context));
    }

    @Test
    public void whenJsonWithEmptyBootProvided_shouldReturnFalse() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();
        ComputerSystemPartialRepresentation json = new ComputerSystemPartialRepresentation();
        json.boot = new ComputerSystemPartialRepresentation.Boot();

        assertFalse(computerSystemConstraintValidator.isValid(json, context));
    }

    @Test
    public void whenJsonWithBootSourceTypeProvided_shouldReturnTrue() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();
        ComputerSystemPartialRepresentation json = new ComputerSystemPartialRepresentation();
        json.boot = new ComputerSystemPartialRepresentation.Boot();
        json.boot.bootSourceOverrideTarget = BIOS_SETUP;
        assertTrue(computerSystemConstraintValidator.isValid(json, context));
    }

    @Test
    public void whenJsonWithBootSourceStateProvided_shouldReturnTrue() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();
        ComputerSystemPartialRepresentation json = new ComputerSystemPartialRepresentation();
        json.boot = new ComputerSystemPartialRepresentation.Boot();
        json.boot.bootSourceOverrideEnabled = CONTINUOUS;
        assertTrue(computerSystemConstraintValidator.isValid(json, context));
    }

    @Test
    public void whenJsonWithNullTrustedModulesListProvided_shouldReturnFalse() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();
        ComputerSystemPartialRepresentation json = new ComputerSystemPartialRepresentation();
        json.trustedModules = null;
        assertFalse(computerSystemConstraintValidator.isValid(json, context));
    }

    @Test
    public void whenJsonWithEmptyTrustedModulesListProvided_shouldReturnFalse() throws Exception {
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();
        ComputerSystemPartialRepresentation json = new ComputerSystemPartialRepresentation();
        json.trustedModules = new HashSet<>();
        assertFalse(computerSystemConstraintValidator.isValid(json, context));
    }

    @Test
    public void whenJsonWithTrustedModuleProvided_shouldReturnTrue() throws Exception {
        ComputerSystemPartialRepresentation json = mock(ComputerSystemPartialRepresentation.class);
        TrustedModule trustedModule = mock(TrustedModule.class);
        Set<TrustedModule> trustedModules = new HashSet<TrustedModule>() {{
            add(trustedModule);
        }};

        when(json.getTrustedModules()).thenReturn(trustedModules);
        when(trustedModule.getInterfaceType()).thenReturn(Ref.of(TPM2_0));

        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();
        assertTrue(computerSystemConstraintValidator.isValid(json, context));
    }

    @Test
    public void whenValidJsonProvided_shouldReturnTrue() throws Exception {
        ComputerSystemPartialRepresentation json = new ComputerSystemPartialRepresentation();
        json.assetTag = DUMMY_DATA;
        json.boot = new ComputerSystemPartialRepresentation.Boot();
        json.boot.bootSourceOverrideEnabled = CONTINUOUS;
        json.boot.bootSourceOverrideTarget = BIOS_SETUP;
        ComputerSystemConstraintValidator computerSystemConstraintValidator = new ComputerSystemConstraintValidator();
        assertTrue(computerSystemConstraintValidator.isValid(json, context));
    }
}
