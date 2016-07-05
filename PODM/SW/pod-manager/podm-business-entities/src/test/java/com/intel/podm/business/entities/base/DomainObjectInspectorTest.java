/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.business.entities.base;

import com.google.common.collect.ImmutableMap;
import org.testng.annotations.Test;

import java.util.Date;
import java.util.Map;

import static org.testng.Assert.assertEquals;

@Test(groups = "DomainObjectInspector")
public class DomainObjectInspectorTest {

    @Test
    public void whenInspectingClassWithoutPropertiesShouldReturnMapWithPropertiesFromDomainObject() {
            final DomainObject domainObject = new ClassWithoutProperties();

            Map<String, Class> properties = DomainObjectInspector
                    .inspect(domainObject.getClass())
                    .getProperties();

            assertEquals(properties, ImmutableMap.of("wasLastUpdatedByDiscovery", Boolean.class));
        }

    @Test
    public void whenInspectingClassWithSinglePropertyShouldReturnValidMapOfProperties() {
        final DomainObject domainObject = new ClassWithOneProperty();

        Map<String, Class> properties = DomainObjectInspector
                .inspect(domainObject.getClass())
                .getProperties();

        assertEquals(properties, ImmutableMap.of("name", String.class, "wasLastUpdatedByDiscovery", Boolean.class));
    }

    @Test
    public void whenInspectingClassWithTwoPropertiesShouldReturnValidMapOfProperties() {
        final DomainObject domainObject = new ClassWithTwoProperties();

        Map<String, Class> properties = DomainObjectInspector
                .inspect(domainObject.getClass())
                .getProperties();

        assertEquals(properties, ImmutableMap.of(
                "size", Integer.class,
                "wasLastUpdatedByDiscovery", Boolean.class,
                "description", String.class));
    }

    @Test
    public void whenInspectingClassWithThreePropertiesShouldReturnValidMapOfProperties() {
        final DomainObject domainObject = new ClassWithThreeProperties();

        Map<String, Class> properties = DomainObjectInspector
                .inspect(domainObject.getClass())
                .getProperties();

        assertEquals(properties, ImmutableMap.of(
                "size", Integer.class,
                "wasLastUpdatedByDiscovery", Boolean.class,
                "description", String.class,
                "date", Date.class));
    }

}

