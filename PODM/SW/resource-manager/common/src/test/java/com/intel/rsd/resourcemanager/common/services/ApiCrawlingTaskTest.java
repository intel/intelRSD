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

package com.intel.rsd.resourcemanager.common.services;

import com.intel.rsd.crawler.Resource;
import com.intel.rsd.resourcemanager.common.services.ApiCrawlingTask.ResourceCollectionConsumer;
import lombok.val;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import static java.util.Arrays.asList;
import static java.util.Collections.singletonList;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.verify;

public class ApiCrawlingTaskTest {
    private ApiCrawlingTask sut;

    @BeforeMethod
    public void setUp() {
        this.sut = new ApiCrawlingTask() {
        };
    }

    @Test
    public void givenResultShouldBeDistributedToAllGivenConsumers() {
        val consumer1 = spy(ResourceCollectionConsumer.class);
        val consumer2 = spy(ResourceCollectionConsumer.class);

        val resultToBeDistributed = singletonList(mock(Resource.class));

        sut.distribute(asList(consumer1, consumer2), resultToBeDistributed);
        verify(consumer1).consume(resultToBeDistributed);
        verify(consumer2).consume(resultToBeDistributed);
    }

    @Test
    public void brokenConsumerDoesntStopResultDistribution() {
        val consumer1 = spy(ApiCrawlingTask.ResourceCollectionConsumer.class);
        doThrow(new RuntimeException()).when(consumer1).consume(any());

        val consumer2 = spy(ResourceCollectionConsumer.class);

        val resultToBeDistributed = singletonList(mock(Resource.class));

        sut.distribute(asList(consumer1, consumer2), resultToBeDistributed);
        verify(consumer1).consume(resultToBeDistributed);
        verify(consumer2).consume(resultToBeDistributed);
    }
}
