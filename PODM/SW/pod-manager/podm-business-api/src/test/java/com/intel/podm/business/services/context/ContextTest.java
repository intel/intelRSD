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

package com.intel.podm.business.services.context;

import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.Context.hasParentOfTypeOnTopOf;
import static com.intel.podm.business.services.context.Context.isAcceptableChildOf;
import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.DRIVE;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.business.services.context.ContextType.SIMPLE_STORAGE;
import static com.intel.podm.common.types.Id.id;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertNotEquals;
import static org.testng.Assert.assertNotNull;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName", "checkstyle:MethodCount"})
public class ContextTest {
    @Test
    public void whenCreatingComputerSystemContext_ShouldNotReturnFalse() {
        assertNotNull(contextOf(id(12), COMPUTER_SYSTEM));
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenCreatingComputerSystemContextWithNullId_ShouldThrow() {
        contextOf(null, COMPUTER_SYSTEM);
    }

    @DataProvider
    public Object[][] theSameContexts() {
        return new Object[][] {
                {
                        contextOf(id(1), COMPUTER_SYSTEM),
                        contextOf(id(1), COMPUTER_SYSTEM)
                },
                {
                        contextOf(id(2), COMPUTER_SYSTEM).child(id(3), PROCESSOR),
                        contextOf(id(2), COMPUTER_SYSTEM).child(id(3), PROCESSOR)
                }
        };
    }

    @Test(dataProvider = "theSameContexts")
    public void WhenComparingTheSameContexts_ShouldReturnTrue(Context left, Context right) {
        assertEquals(left, right);
    }

    @DataProvider
    public Object[][] differentContexts() {
        return new Object[][] {
                {
                        contextOf(id(2), COMPUTER_SYSTEM).child(id(3), PROCESSOR),
                        null
                },
                {
                        contextOf(id(2), COMPUTER_SYSTEM),
                        contextOf(id(3), COMPUTER_SYSTEM)
                },
                {
                        contextOf(id(4), COMPUTER_SYSTEM).child(id(2), MEMORY),
                        contextOf(id(4), COMPUTER_SYSTEM).child(id(2), PROCESSOR)
                },
                {
                        contextOf(id(4), COMPUTER_SYSTEM).child(id(1), PROCESSOR),
                        contextOf(id(4), COMPUTER_SYSTEM).child(id(2), PROCESSOR)
                },
        };
    }

    @Test(dataProvider = "differentContexts")
    public void WhenComparingDifferentContexts_ShouldReturnFalse(Context left, Context right) {
        assertNotEquals(left, right);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenCreatingChildContextWithNullType_ShouldThrow() {
        contextOf(id(3), COMPUTER_SYSTEM).child(id(12), null);
    }

    @Test
    public void whenCreatingChildContext_ShouldNotReturnNull() {
        assertNotNull(contextOf(id(3), COMPUTER_SYSTEM).child(id(12), PROCESSOR));
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenCreatingComputerSystemChildFromComputerSystem_ShouldThrow() {
        contextOf(id(1), COMPUTER_SYSTEM).child(id(2), COMPUTER_SYSTEM);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenCheckingAcceptableChildForNullChildType_shouldThrow() {
        isAcceptableChildOf(null, contextOf(id(1), COMPUTER_SYSTEM));
    }


    @Test(dataProvider = "acceptableChildren")
    public void testAcceptableChild(ContextType childType, Context parent) {
        assertTrue(isAcceptableChildOf(childType, parent));
    }

    @DataProvider
    public Object[][] acceptableChildren() {
        return new Object[][] {
                {
                        PROCESSOR,
                        contextOf(id(1), COMPUTER_SYSTEM)
                },
                {
                        CHASSIS,
                        null
                },
                {
                        ETHERNET_SWITCH_PORT_VLAN,
                        contextOf(id(1), ETHERNET_SWITCH).child(id(2), ETHERNET_SWITCH_PORT)
                }
        };
    }

    @Test(dataProvider = "unacceptableChildren")
    public void testUnacceptableChild(ContextType childType, Context parent) {
        assertFalse(isAcceptableChildOf(childType, parent));
    }

    @DataProvider
    public Object[][] unacceptableChildren() {
        return new Object[][] {
                {
                        PROCESSOR,
                        contextOf(id(1), CHASSIS)
                },
                {
                        PROCESSOR,
                        contextOf(id(1), COMPUTER_SYSTEM).child(id(2), PROCESSOR)
                },
                {
                        PROCESSOR,
                        null
                }
        };
    }

    @Test(dataProvider = "acceptableParentOnTopOf")
    public void testAcceptableParentOnTopOf(Context context, ContextType possibleParentType) {
        assertTrue(hasParentOfTypeOnTopOf(context, possibleParentType));
    }

    @DataProvider
    public Object[][] acceptableParentOnTopOf() {
        return new Object[][] {
                {
                        contextOf(id(1), COMPUTER_SYSTEM).child(id(2), SIMPLE_STORAGE),
                        COMPUTER_SYSTEM
                },
                {
                        contextOf(id(1), CHASSIS),
                        null
                },
                {
                        contextOf(id(1), ETHERNET_SWITCH).child(id(2), ETHERNET_SWITCH_PORT).child(id(3), ETHERNET_SWITCH_PORT_VLAN),
                        ETHERNET_SWITCH_PORT
                }
        };
    }

    @Test(dataProvider = "unacceptableParentOnTopOf")
    public void testUnacceptableParentOnTopOf(Context context, ContextType possibleParentType) {
        assertFalse(hasParentOfTypeOnTopOf(context, possibleParentType));
    }

    @DataProvider
    public Object[][] unacceptableParentOnTopOf() {
        return new Object[][] {
                {
                        contextOf(id(1), CHASSIS),
                        COMPUTER_SYSTEM
                },
                {
                        contextOf(id(1), CHASSIS).child(id(2), DRIVE),
                        COMPUTER_SYSTEM
                },
                {
                        contextOf(id(1), ETHERNET_SWITCH).child(id(2), ETHERNET_SWITCH_PORT).child(id(3), ETHERNET_SWITCH_PORT_VLAN),
                        SIMPLE_STORAGE
                }
        };
    }
}
