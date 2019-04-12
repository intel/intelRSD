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
import com.fasterxml.jackson.databind.node.NullNode;
import com.fasterxml.jackson.databind.node.TextNode;
import lombok.val;
import org.testng.annotations.Test;

import static com.intel.rsd.json.JsonUtils.newObjectNode;
import static java.util.UUID.randomUUID;
import static org.assertj.core.api.AssertionsForClassTypes.assertThat;
import static org.assertj.core.api.Java6Assertions.catchThrowable;

public class FirstNonEmptyFunctionTest {

    private static final JsonNode ANY_NODE_1 = TextNode.valueOf(randomUUID().toString());
    private static final JsonNode ANY_NODE_2 = TextNode.valueOf(randomUUID().toString());

    private FirstNonEmptyFunction sut = new FirstNonEmptyFunction();

    @Test
    public void whenTwoNonNullArgumentsAreProvided_thenFirstOnShouldBeReturned() {
        assertThat(sut.invoke(ANY_NODE_2, ANY_NODE_1)).isEqualTo(ANY_NODE_2.asText());
    }

    @Test
    public void whenFirstArgumentIsNullNodeAndSecondValidValueNode_thenSecondOneShouldBeReturned() {
        assertThat(sut.invoke(NullNode.getInstance(), ANY_NODE_1)).isEqualTo(ANY_NODE_1.asText());
    }

    @Test
    public void whenOnlyOneNonNullArgumentIsProvided_thenItShouldBeReturned() {
        assertThat(sut.invoke(ANY_NODE_1)).isEqualTo(ANY_NODE_1.asText());
    }

    @Test
    public void whenNoNonNullValueNodesAreProvided_thenExceptionShouldBeThrown() {
        val managerBody = newObjectNode();
        val thrown = catchThrowable(() -> sut.invoke(managerBody));

        assertThat(thrown).isInstanceOf(RuntimeException.class)
            .hasNoCause()
            .hasMessageContaining("all of provided values are null or empty");

    }
}
