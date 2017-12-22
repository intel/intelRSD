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

package com.intel.podm.business.redfish.services.assembly.tasks;

import org.mockito.InOrder;
import org.mockito.InjectMocks;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.util.List;
import java.util.function.Consumer;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.common.types.Id.id;
import static java.util.stream.Collectors.toList;
import static java.util.stream.IntStream.range;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

public class NodeAssemblyTaskRunnerTest {

    private static final int ANY_NUMBER_OF_TASKS = 5;

    @InjectMocks
    private NodeAssemblyTaskRunner runner = new NodeAssemblyTaskRunner();

    @BeforeMethod
    public void setUp() throws Exception {
        MockitoAnnotations.initMocks(this);
    }

    @Test
    public void testTaskChainInvocationOrder() throws Exception {

        List<NodeTask> tasks = createNodeAssemblyTasks(ANY_NUMBER_OF_TASKS);

        NodeAssemblyTaskChainBuilder.instance()
            .prepareTaskChain(tasks)
            .forComposedNode(id(1))
            .executeWith(nodeAssemblyTaskChainElementWithNext -> nodeAssemblyTaskChainElementWithNext.run());

        InOrder inOrder = Mockito.inOrder(tasks.toArray());
        tasks.forEach(t -> inOrder.verify(t, times(1)).run());
    }


    @Test(expectedExceptions = RuntimeException.class)
    public void testRethrowingExceptionHandler() {
        NodeTask exceptionThrowingTask = createExceptionThrowingNodeAssemblyTask();

        NodeAssemblyTaskChainBuilder.instance()
            .prepareTaskChain(newArrayList(exceptionThrowingTask))
            .forComposedNode(id(1))
            .useExceptionHandler(e -> {
                throw new RuntimeException(e);
            })
            .executeWith(next -> next.run());
    }

    @Test
    public void testIfTaskChainInvocationIsStoppedByException() {
        NodeTask task1 = createNodeAssemblyTask();
        NodeTask task2 = createExceptionThrowingNodeAssemblyTask();
        NodeTask task3 = createNodeAssemblyTask();

        Consumer<RuntimeException> exceptionHandler = mock(Consumer.class);

        NodeAssemblyTaskChainBuilder.instance()
            .prepareTaskChain(newArrayList(task1, task2, task3))
            .forComposedNode(id(1))
            .useExceptionHandler(exceptionHandler)
            .executeWith(next -> next.run());

        verify(exceptionHandler, times(1)).accept(any());

        InOrder inOrder = Mockito.inOrder(task1, task2, exceptionHandler, task3);
        inOrder.verify(task1, times(1)).run();
        inOrder.verify(task2, times(1)).run();
        inOrder.verify(exceptionHandler, times(1)).accept(any());
        inOrder.verify(task3, never()).run();

    }

    private List<NodeTask> createNodeAssemblyTasks(int tasksNumber) {
        return range(0, tasksNumber).mapToObj(i -> createNodeAssemblyTask()).collect(toList());
    }

    private NodeTask createExceptionThrowingNodeAssemblyTask() {
        NodeTask exceptionThrowingTask = createNodeAssemblyTask();
        doThrow(new RuntimeException("")).when(exceptionThrowingTask).run();
        return exceptionThrowingTask;
    }

    private NodeTask createNodeAssemblyTask() {
        return mock(NodeTask.class);
    }
}
