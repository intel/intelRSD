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

package com.intel.podm.redfish.json.templates.actions.constraints;

import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import javax.validation.ConstraintValidatorContext;
import javax.validation.ConstraintValidatorContext.ConstraintViolationBuilder;
import java.util.function.Predicate;

import static org.mockito.Matchers.any;
import static org.mockito.Matchers.anyString;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class ValidatorTest {

    private ConstraintValidatorContext context;
    private ConstraintViolationBuilder builder;

    @BeforeMethod
    public void setUp() throws Exception {
        this.context = mock(ConstraintValidatorContext.class);
        this.builder = mock(ConstraintViolationBuilder.class);
        when(context.buildConstraintViolationWithTemplate(anyString())).thenReturn(builder);
    }

    @Test
    public void validateWithNoRule() {
        assertTrue(Validator.instance(context).isValid(1));
    }

    @Test
    public void validateWithViolatedRule() {
        Predicate alwaysBroken = value -> false;
        Validator validator = Validator.instance(context).addRule(alwaysBroken, null);
        assertFalse(validator.isValid(null), "Should return false when rule is violated");
    }

    @Test
    public void validateWithNonViolatedRule() {
        Predicate alwaysPassed = value -> true;
        Validator validator = Validator.instance(context).addRule(alwaysPassed, null);
        assertTrue(validator.isValid(null), "Should return true when rule is not violated");
    }

    @Test
    public void validateWithMultipleRulesOneIsViolatedAndSecondIsNot() {
        Predicate violatedRule = value -> false;
        Predicate notViolatedRule = value -> true;

        Validator validator = Validator.instance(context)
            .addRule(violatedRule, null)
            .addRule(notViolatedRule, null);

        assertFalse(validator.isValid(null), "Configuration should not be valid when at least one rule is violated");
    }

    @Test
    public void validateWithMultipleViolatedRulesFirstOneShouldBeReported() throws Exception {
        Rule violatedRule1 = spy(new Rule(false));
        Rule violatedRule2 = spy(new Rule(false));

        Validator validator = Validator.instance(context)
            .addRule(violatedRule1, null)
            .addRule(violatedRule2, null);

        assertFalse(validator.isValid(null), "Configuration should be not valid when at all rule are violated");
        verify(violatedRule1, times(1)).test(any());
        verify(violatedRule2, never()).test(any());
    }

    static class Rule implements Predicate<Object> {
        boolean testResult;

        Rule(boolean testResult) {
            this.testResult = testResult;
        }

        @Override
        public boolean test(Object o) {
            return testResult;
        }
    }
}
