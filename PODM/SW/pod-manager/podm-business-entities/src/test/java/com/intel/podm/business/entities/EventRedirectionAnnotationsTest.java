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

package com.intel.podm.business.entities;

import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.lang.reflect.Method;
import java.util.List;
import java.util.Map;
import java.util.Set;

import static com.intel.podm.common.utils.ClassGatherer.getAllClassesByPackageAndAnnotation;
import static java.util.stream.Collectors.counting;
import static java.util.stream.Collectors.groupingBy;
import static java.util.stream.Collectors.toList;
import static org.apache.commons.lang3.reflect.FieldUtils.getFieldsListWithAnnotation;
import static org.apache.commons.lang3.reflect.MethodUtils.getMethodsListWithAnnotation;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.lessThanOrEqualTo;
import static org.hamcrest.collection.IsMapContaining.hasValue;
import static org.testng.AssertJUnit.assertTrue;

public class EventRedirectionAnnotationsTest {
    private static final String ENTITIES_PACKAGE = "com.intel.podm.business.entities.*";

    private Set<Class<?>> eventableEntityClasses;

    @BeforeClass
    public void setUp() throws Exception {
        this.eventableEntityClasses = getAllClassesByPackageAndAnnotation(ENTITIES_PACKAGE, javax.persistence.Entity.class);
    }

    @Test
    public void checkDuplicatedTargets() throws Exception {
        for (Class eventableEntityClass : eventableEntityClasses) {
            List<Method> declaredTargets = getMethodsListWithAnnotation(eventableEntityClass, EventRedirectionTarget.class);
            if (declaredTargets.isEmpty()) {
                continue;
            }

            Map<String, Long> countedTargets = declaredTargets.stream()
                .map(method -> method.getAnnotation(EventRedirectionTarget.class))
                .collect(groupingBy(EventRedirectionTarget::value, counting()));

            assertThat(
                "Multiple @EventRedirectionTarget with same redirection key are not supported. Verify targets declared on " + eventableEntityClass,
                countedTargets,
                hasValue(lessThanOrEqualTo(1L))
            );
        }
    }

    @Test
    @SuppressWarnings("unchecked")
    public void checkWhetherEventRedirectionTargetExistsForEventRedirectionSource() throws NoSuchMethodException {

        for (Class eventableEntityClass : eventableEntityClasses) {
            List<String> sources = getFieldsListWithAnnotation(eventableEntityClass, EventRedirectionSource.class).stream()
                .map(field -> field.getAnnotation(EventRedirectionSource.class))
                .map(EventRedirectionSource::value)
                .distinct()
                .collect(toList());

            List<String> targets = getMethodsListWithAnnotation(eventableEntityClass, EventRedirectionTarget.class)
                .stream()
                .map(method -> method.getAnnotation(EventRedirectionTarget.class))
                .map(EventRedirectionTarget::value)
                .collect(toList());

            sources.forEach(source -> assertTrue(
                "@EventRedirectionTarget(\"" + source + "\") should be defined for class: " + eventableEntityClass, targets.contains(source))
            );
        }
    }
}
