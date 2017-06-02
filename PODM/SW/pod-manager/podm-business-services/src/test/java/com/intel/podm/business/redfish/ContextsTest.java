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

package com.intel.podm.business.redfish;

import com.intel.podm.business.entities.EventOriginProvider;
import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Power;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.services.context.Context;
import org.testng.annotations.Test;

import java.util.List;
import java.util.Set;
import java.util.function.Function;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.business.services.context.ContextType.COMPOSED_NODE;
import static com.intel.podm.business.services.context.ContextType.POWER;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.utils.ClassGatherer.getAllClassesByPackageAndAnnotation;
import static java.lang.reflect.Modifier.isStatic;
import static java.util.Arrays.stream;
import static java.util.stream.Collectors.joining;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.is;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class ContextsTest {

    private static final String ENTITIES_PACKAGE = "com.intel.podm.business.entities.*";
    private static final String TO_CONTEXT = "toContext";

    @Test
    @SuppressWarnings("unchecked")
    public void checkWhetherAllEntitiesHaveToContextMethod() throws NoSuchMethodException {
        Set<Class> eventableEntityClasses = getAllClassesByPackageAndAnnotation(ENTITIES_PACKAGE, javax.persistence.Entity.class).stream()
            .map(clazz -> clazz)
            .filter(clazz -> clazz.getAnnotation(Eventable.class) != null)
            .map(mapToEventSourceClass())
            .collect(toSet());

        List<Class> parameterClasses = getToContextParameterClasses();
        eventableEntityClasses.removeAll(parameterClasses);

        if (!eventableEntityClasses.isEmpty()) {
            throw new NoSuchMethodException("Failed to find toContext methods for " + eventableEntityClasses.stream()
                .map(Class::getSimpleName)
                .collect(joining(", ")));
        }
    }

    private Function<Class, Class> mapToEventSourceClass() {
        return clazz -> stream(clazz.getMethods())
            .filter(method -> method.isAnnotationPresent(EventOriginProvider.class))
            .map(method -> method.getReturnType())
            .filter(type -> !DiscoverableEntity.class.equals(type))
            .findFirst()
            .orElse(clazz);
    }

    private List<Class> getToContextParameterClasses() {
        return stream(Contexts.class.getDeclaredMethods())
            .filter(method -> method.getName().startsWith(TO_CONTEXT))
            .filter(method -> isStatic(method.getModifiers()))
            .filter(method -> method.getParameters().length == 1)
            .filter(method -> method.getReturnType().equals(Context.class))
            .map(method -> method.getParameters()[0].getType())
            .filter(clazz -> clazz.isAnnotationPresent(Eventable.class))
            .collect(toList());
    }

    @Test
    public void toContextForDiscoverableEntity() throws Exception {
        Chassis chassis = mock(Chassis.class);
        Power power = new Power();
        when(chassis.getId()).thenReturn(id("chassisId"));
        when(chassis.getPower()).thenReturn(power);
        power.setId(id("powerId"));
        power.setChassis(chassis);
        assertThat(toContext(power).getType(), is(POWER));
    }

    @Test
    public void toContextForComposedNode() throws Exception {
        ComposedNode node = mock(ComposedNode.class);
        when(node.getId()).thenReturn(id(1));
        assertThat(toContext(node).getType(), is(COMPOSED_NODE));
    }

    @Test(expectedExceptions = UnsupportedOperationException.class)
    public void toContextForUnknownEntity() throws Exception {
        DiscoverableEntity fakeEntity = mock(DiscoverableEntity.class);
        toContext(fakeEntity);
    }

    @Test(expectedExceptions = RuntimeException.class)
    public void toContextForBrokenEntity() throws Exception {
        DiscoverableEntity brokenEntity = new Power();
        toContext(brokenEntity);
    }
}
