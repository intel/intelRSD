/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.unifier.unification;

import com.intel.rsd.resourcemanager.layers.unifier.unification.dtos.Data;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.io.IOException;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static java.util.Optional.of;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

public class OdataIdMapperTest {

    @Test(dataProvider = "getUnifiedEntityIdTestCases")
    public void testGetUnifiedEntityIdTestCases(String sourceOdataId, String targetOdataId, String expectedUnifiedId) {
        UnifiedOdataIdResolver resolver = mock(UnifiedOdataIdResolver.class);
        when(resolver.toUnified(eq(sourceOdataId))).thenReturn(of(targetOdataId));
        assertEquals(OdataIdMapper.createUnificationMapper(resolver).mapEntityId(sourceOdataId).get(), expectedUnifiedId);
    }

    @DataProvider(name = "getUnifiedEntityIdTestCases")
    public Object[][] getUnifiedEntityIdTestCases() throws IOException {
        Data data = readJsonFromFile(this, "OdataIdMapperTest.json", Data.class);
        return data
            .getTestCases()
            .stream()
            .flatMap(tc -> tc.getAssertions().stream().map(a -> new Object[]{a.getFrom(), a.getTo(), a.getId()}))
            .toArray(Object[][]::new);
    }
}
