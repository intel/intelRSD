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

package com.intel.podm.business.entities;

import com.intel.podm.common.types.redfish.ResourceNames;
import org.testng.annotations.Test;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import static com.google.common.collect.ImmutableSet.copyOf;
import static org.testng.Assert.assertEquals;

public class IdFromUriGeneratorTest {

    /**
     * Test checks if acronym generate method provides unique acronyms for all resources in ResourceNames.
     * Note: "Drives" resource can be used in different context and get the same acronym.
     *
     * @throws IllegalAccessException
     */
    @Test
    public void checkResourceAcronymsUniqness() throws IllegalAccessException {
        IdFromUriGenerator generator = new IdFromUriGenerator();
        Map<String, String> acronyms = new HashMap<>();
        ResourceNames.ALL_RESOURCE_NAMES.stream().forEach(resourceName -> acronyms.put(resourceName, generator.get(resourceName).apply(resourceName)));
        int acronymsSize = acronyms.size();
        int uniqueAcronymsSice = copyOf(acronyms.values()).size();

        if (acronyms.size() != uniqueAcronymsSice) {
            Iterator<Map.Entry<String, String>> iterator = acronyms.entrySet().iterator();
            while (iterator.hasNext()) {
                Map.Entry<String, String> acronymMapping = iterator.next();
                if (acronyms.values().stream().filter(a -> a.equals(acronymMapping.getValue())).count() == 1) {
                    iterator.remove();
                }
            }
            acronyms.forEach((from, acronym) -> System.err.println("NOT UNIQUE ACRONYM: " + from + " -> " + acronym));
            if (acronyms.size() > 0) {
                System.err.println("Add custom acronym to IRREGULAR_ACRONYMS map in IdFromUriGenerator.");
            }
        }
        assertEquals(acronymsSize, uniqueAcronymsSice);
    }
}
