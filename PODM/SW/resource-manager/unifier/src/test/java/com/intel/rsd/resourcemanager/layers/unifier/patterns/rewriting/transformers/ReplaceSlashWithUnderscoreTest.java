/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.TextNode;
import lombok.val;
import org.testng.annotations.Test;

import static org.assertj.core.api.Assertions.assertThat;

public class ReplaceSlashWithUnderscoreTest {

    ReplaceSlashWithUnderscore sut = new ReplaceSlashWithUnderscore();

    @Test
    public void testFunctionId() {
        assertThat(sut.functionId()).isEqualTo("replaceSlashWithUnderscore");
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenNullIsGiven_exceptionShouldBeThrown() {
        sut.invoke((JsonNode[]) null);
    }

    @Test
    public void whenJsonNodeContainingSlashIsGiven_slashShouldBeReplacedWithUnderscore() {
        val jsonNode = new TextNode("something/somewhere");
        assertThat(sut.invoke(jsonNode)).isEqualTo("something_somewhere");
    }
}
