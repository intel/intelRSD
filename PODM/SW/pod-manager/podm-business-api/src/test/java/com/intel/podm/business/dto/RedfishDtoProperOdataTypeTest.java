/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.dto;

import net.sf.corn.cps.ClassFilter;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.regex.Pattern;

import static java.util.stream.Collectors.toSet;
import static net.sf.corn.cps.CPScanner.scanClasses;
import static org.testng.Assert.fail;
import static org.testng.AssertJUnit.assertFalse;
import static org.testng.AssertJUnit.assertTrue;

public class RedfishDtoProperOdataTypeTest {
    private static final String POD_MANAGER_DTO_PACKAGE = "com.intel.podm.business.*";
    private final Pattern regex = Pattern.compile("^#([a-zA-Z0-9]+)\\.v([0-9]+_){2}[0-9]+\\.([a-zA-Z0-9]+)$");

    @Test(dataProvider = "RedfishDtoOdataTypes")
    private void shouldPass(String odataType) {
        assertTrue(odataType.matches(regex.pattern()));
    }

    @DataProvider(name = "RedfishDtoOdataTypes")
    public Iterator<Object[]> odataTypeInRedfishDtoShouldHaveProperFormat() {
        List<Object[]> parameters = new ArrayList<>();
        for (Class<?> clazz : getRedfishDtoClasses()) {
            try {
                RedfishDto dto = (RedfishDto) clazz.newInstance();
                parameters.add(new Object[]{dto.getoDataType()});
            } catch (InstantiationException | IllegalAccessException e) {
                fail(e.getMessage());
            }
        }
        return parameters.iterator();
    }

    private Set<Class<?>> getRedfishDtoClasses() {
        return scanClasses(new ClassFilter()
            .packageName(POD_MANAGER_DTO_PACKAGE)
            .superClass(RedfishDto.class))
            .stream()
            .collect(toSet());
    }

    @Test(dataProvider = "InvalidOdataTypes")
    private void shouldFail(String odataType) {
        assertFalse(odataType.matches(regex.pattern()));
    }

    @DataProvider(name = "InvalidOdataTypes")
    public Object[][] invalidOdataTypes() {
        return new Object[][]{
            {"Object.v1_0_0.Object"},
            {"Object.v1.0.0.Object"},
            {"#Object.v1.0.0.Object"},
            {"asdasdasdasdx!@#45xc"}
        };
    }
}
