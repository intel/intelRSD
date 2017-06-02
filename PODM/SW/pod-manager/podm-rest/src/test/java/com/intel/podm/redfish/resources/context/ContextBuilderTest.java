/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.redfish.resources.context;

import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Id;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.Collection;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.common.types.Id.id;
import static java.lang.String.format;
import static java.util.Arrays.asList;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName"})
public class ContextBuilderTest {
    @Test(dataProvider = "correctPathParamsForContextBuilding")
    public void whenBuildingPodContext_shouldReturnCorrectContext(Collection<Param> params, Context expectedContext) {
        ContextBuilder sut = new ContextBuilder();

        for (Param param : params) {
            sut.add(param.name, param.value);
        }
        Context actualContext = sut.build();

        assertEquals(actualContext, expectedContext);
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*ssdasdsadsadas.*")
    public void whenAddingUnknownParam_shouldThrow() {
        ContextBuilder sut = new ContextBuilder();
        sut.add("ssdasdsadsadas", id(1));
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenAddingNullParamValue_shouldThrow() {
        ContextBuilder sut = new ContextBuilder();
        sut.add("{podId}", null);
    }

    @Test(expectedExceptions = ContextBuilderException.class)
    public void whenBuildingIncorrectContext_shouldThrow() {
        ContextBuilder sut = new ContextBuilder();
        sut.add("{processorId}", id(1));
        sut.add("{computerSystemId}", id(2));
        sut.build();
    }

    @Test(expectedExceptions = ContextBuilderException.class)
    public void whenBuildingEmptyContext_shouldThrow() {
        ContextBuilder sut = new ContextBuilder();
        sut.build();
    }

    @DataProvider
    public Object[][] correctPathParamsForContextBuilding() {
        return new Object[][] {
                {
                        asList(param("{computerSystemId}", id(1))),
                        contextOf(id(1), COMPUTER_SYSTEM)
                },
                {
                        asList(param("{computerSystemId}", id(1)), param("{processorId}", id(2))),
                        contextOf(id(1), COMPUTER_SYSTEM).child(id(2), PROCESSOR)
                },
                {
                        asList(
                                param("{ethernetSwitchId}", id(2)),
                                param("{ethernetSwitchPortId}", id(3)),
                                param("{ethernetSwitchPortVlanId}", id(4))
                        ),
                        contextOf(id(2), ETHERNET_SWITCH).child(id(3), ETHERNET_SWITCH_PORT).child(id(4), ETHERNET_SWITCH_PORT_VLAN)
                }
        };
    }

    private static Param param(String name, Id value) {
        return new Param(name, value);
    }

    private static final class Param {
        final String name;
        final Id value;

        private Param(String name, Id value) {
            this.name = name;
            this.value = value;
        }

        @Override
        public String toString() {
            return format("%s=%s", name, value);
        }
    }
}
