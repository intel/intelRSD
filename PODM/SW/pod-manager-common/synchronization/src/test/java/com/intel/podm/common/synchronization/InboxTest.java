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

package com.intel.podm.common.synchronization;

import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import static org.testng.Assert.assertNotNull;
import static org.testng.Assert.assertNull;

public class InboxTest {

    private InBox inbox;

    @BeforeMethod
    public void setUp() throws Exception {
        this.inbox = new InBox("IDENTITY");
    }

    @Test
    public void addToEmptyInBox() throws Exception {
        assertNull(inbox.poll(), "InBox should be empty");
        inbox.add(() -> {
        });
        assertNotNull(inbox.poll(), "Added element is missing");
        assertNull(inbox.poll(), "InBox should be empty");
    }

    @Test
    public void pauseAndResumeTest() {
        inbox.pause();
        inbox.add(() -> {
        });
        assertNull(inbox.poll(), "Paused inBox should not report any elements");
        inbox.resume();
        assertNotNull(inbox.poll(), "Previously added element should be reported on resumed inbox");
    }

}
