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

package com.intel.podm.business.entities.interceptors;

import com.intel.podm.business.entities.EventRedirectionSource;
import com.intel.podm.business.entities.EventRedirectionTarget;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.base.Entity;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.function.Consumer;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyZeroInteractions;

public class EventRedirectionsTest {

    private EventRedirections eventRedirections = new EventRedirections();

    @DataProvider
    public Object[][] redirectedInvocations() {

        ComputerSystem computerSystem = new ComputerSystem();
        ComposedNode composedNode = new ComposedNode();
        computerSystem.setComposedNode(composedNode);

        Target target1 = new Target(1);
        Target target2 = new Target(2);
        Source source = new Source(target1, target2);

        return new Object[][]{
            {source, "property1", target1},
            {source, "property2", target2},
            {computerSystem, "powerState", composedNode},
            {computerSystem, "processorSummary", composedNode},
            {computerSystem, "boot", composedNode},
        };
    }

    @DataProvider
    public Object[][] notRedirectedInvocations() {
        return new Object[][]{
            {new ComputerSystem(), "powerState"},
            {new ComputerSystem(), "processorSummary"},
            {new ComputerSystem(), "boot"},
        };
    }

    @Test(dataProvider = "redirectedInvocations")
    public void invocationsShoudlBeRedirected(Entity sourceEntity, String changedProperty, Entity expectedTargetEntity) throws Exception {
        Consumer<Entity> redirector = mock(Consumer.class);
        eventRedirections.redirectIfRequired(sourceEntity, propertyName -> changedProperty.equals(propertyName), redirector);
        verify(redirector, times(1)).accept(expectedTargetEntity);
    }

    @Test(dataProvider = "notRedirectedInvocations")
    public void invocationsShouldntBeRedirected(Entity sourceEntity, String changedProperty) throws Exception {
        Consumer<Entity> redirector = mock(Consumer.class);
        eventRedirections.redirectIfRequired(sourceEntity, propertyName -> changedProperty.equals(propertyName), redirector);
        verifyZeroInteractions(redirector);
    }

    static class Source extends Entity {

        public static final String PROPERTY1_TO_TARGET1_REDIRECTION = "P1ToT1";
        public static final String PROPERTY2_TO_TARGET2_REDIRECTION = "P2ToT2";

        @EventRedirectionSource(PROPERTY1_TO_TARGET1_REDIRECTION)
        private Object property1;

        @EventRedirectionSource(PROPERTY2_TO_TARGET2_REDIRECTION)
        private Object property2;

        private Target target1;

        private Target target2;

        Source(Target target1, Target target2) {
            this.target1 = target1;
            this.target2 = target2;
        }

        @EventRedirectionTarget(PROPERTY1_TO_TARGET1_REDIRECTION)
        public Target getTarget1() {
            return target1;
        }

        @EventRedirectionTarget(PROPERTY2_TO_TARGET2_REDIRECTION)
        public Target getTarget2() {
            return target2;
        }

        @Override
        public void preRemove() {
        }

        @Override
        public boolean containedBy(Entity possibleParent) {
            return false;
        }
    }

    static class Target extends Entity {

        private long primarykey;

        Target(long primarykey) {
            this.primarykey = primarykey;
        }

        @Override
        public long getPrimaryKey() {
            return primarykey;
        }

        @Override
        public void preRemove() {
        }

        @Override
        public boolean containedBy(Entity possibleParent) {
            return false;
        }
    }
}
