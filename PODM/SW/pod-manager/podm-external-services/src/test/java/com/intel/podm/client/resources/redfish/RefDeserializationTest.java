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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.module.SimpleModule;
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule;
import com.intel.podm.client.SerializersProvider;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.EnumeratedType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.annotations.AsUnassigned;
import com.intel.podm.common.types.deserialization.EnumeratedTypeDeserializer;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.io.IOException;
import java.util.List;

import static com.fasterxml.jackson.databind.DeserializationFeature.FAIL_ON_NULL_FOR_PRIMITIVES;
import static com.fasterxml.jackson.databind.DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES;
import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.client.resources.redfish.RefDeserializationTest.TestEnum.FIRST;
import static com.intel.podm.client.resources.redfish.RefDeserializationTest.TestEnum.SECOND;
import static com.intel.podm.client.typeidresolver.OdataTypeMatcher.simpleOdataTypeMatcher;
import static com.intel.podm.client.typeidresolver.ResourceResolver.register;
import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static java.lang.Integer.valueOf;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertNull;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName", "checkstyle:MagicNumber"})
public class RefDeserializationTest {

    private final String testResourceJson = "{"
        + "\"@odata.type\": \"#TestResource.v1_0_0.TestResource\","
        + "\"SimpleTypeCollection\": [1, 2, 4],"
        + "\"EnumTypeCollection\": [\"FIRST\", \"SECOND\"],"
        + "\"Integer\": 4,"
        + "\"String\": \"someValue\","
        + "\"EnumType\": \"FIRST\","
        + "\"WillBeNull\": null,"
        + "\"SomeEmptyCollection\": [],"
        + "\"SomeNullCollection\": null,"
        + "\"CustomObject\": {"
        + "\"EnumType\": \"SECOND\","
        + "\"AssignedToNull\": \"null\""
        + "}"
        + "}";
    private ObjectMapper mapper;

    @BeforeClass
    public void setUp() {
        register(simpleOdataTypeMatcher("#TestResource.v1_0_0.TestResource", TestResource.class));

        SimpleModule serializersModule = new SerializersProvider().getSerializersModule();
        serializersModule.addDeserializer(TestEnum.class, new EnumeratedTypeDeserializer<>(TestEnum.class));

        mapper = new ObjectMapper()
            .registerModule(new JavaTimeModule())
            .registerModule(serializersModule)
            .disable(FAIL_ON_UNKNOWN_PROPERTIES)
            .enable(FAIL_ON_NULL_FOR_PRIMITIVES);
    }

    @Test
    public void whenTestResourceDeserialized_simpleTypesShouldBeAssigned() throws IOException {
        TestResource testResource = mapper.readValue(testResourceJson, TestResource.class);

        assertTrue(testResource.integer.isAssigned());
        assertEquals(testResource.integer.get(), valueOf(4));
        assertTrue(testResource.string.isAssigned());
        assertEquals(testResource.string.get(), "someValue");
    }

    @Test
    public void whenTestResourceDeserialized_absentSimpleTypeShouldBeUnassigned() throws IOException {
        TestResource testResource = mapper.readValue(testResourceJson, TestResource.class);
        assertFalse(testResource.someAbsentField.isAssigned());
    }

    @Test
    public void whenTestResourceDeserialized_absentCollectionShouldBeUnassigned() throws IOException {
        TestResource testResource = mapper.readValue(testResourceJson, TestResource.class);
        assertFalse(testResource.someAbsentCollection.isAssigned());
    }

    @Test
    public void whenTestResourceDeserialized_emptyCollectionShouldBeUnassigned() throws IOException {
        TestResource testResource = mapper.readValue(testResourceJson, TestResource.class);
        assertFalse(testResource.someEmptyCollection.isAssigned());
    }

    @Test
    public void whenTestResourceDeserialized_nullCollectionShouldBeUnassigned() throws IOException {
        TestResource testResource = mapper.readValue(testResourceJson, TestResource.class);
        assertFalse(testResource.someNullCollection.isAssigned());
    }

    @Test
    public void whenTestResourceDeserialized_nullSimpleTypeShouldBeUnassigned() throws IOException {
        TestResource testResource = mapper.readValue(testResourceJson, TestResource.class);
        assertFalse(testResource.willBeNull.isAssigned());
    }

    @Test
    public void whenTestResourceDeserialized_absentEnumTypeShouldBeAssigned() throws IOException {
        TestResource testResource = mapper.readValue(testResourceJson, TestResource.class);
        assertTrue(testResource.enumType.isAssigned());
        assertEquals(testResource.enumType.get(), FIRST);
    }

    @Test
    public void whenTestResourceDeserialized_enumTypeCollectionShouldBeAssigned() throws IOException {
        TestResource testResource = mapper.readValue(testResourceJson, TestResource.class);
        assertTrue(testResource.enumTypeCollection.isAssigned());
        assertTrue(testResource.enumTypeCollection.get().containsAll(newArrayList(FIRST, SECOND)));
    }

    @Test
    public void whenTestResourceDeserialized_simpleTypeCollectionShouldBeAssigned() throws IOException {
        TestResource testResource = mapper.readValue(testResourceJson, TestResource.class);
        assertTrue(testResource.simpleTypeCollection.isAssigned());
        assertTrue(testResource.simpleTypeCollection.get().containsAll(newArrayList(1, 2, 4)));
    }

    @Test
    public void whenTestResourceDeserialized_customObjectShouldBeAssigned() throws IOException {
        TestResource testResource = mapper.readValue(testResourceJson, TestResource.class);
        assertTrue(testResource.customObject.isAssigned());
        assertTrue(testResource.customObject.get().enumType.isAssigned());
        assertEquals(testResource.customObject.get().enumType.get(), SECOND);
    }

    @Test
    public void whenTestResourceDeserialized_fieldWithoutAsUnassignedShouldBeAssignedWithNullValue() throws IOException {
        TestResource testResource = mapper.readValue(testResourceJson, TestResource.class);
        assertTrue(testResource.customObject.isAssigned());
        assertTrue(testResource.customObject.get().assignedToNull.isAssigned());
        assertNull(testResource.customObject.get().assignedToNull.get());
    }

    enum TestEnum implements EnumeratedType {
        FIRST("FIRST"),
        SECOND("SECOND");

        private final String value;

        TestEnum(String value) {
            this.value = value;
        }

        @Override
        public String getValue() {
            return value;
        }
    }

    static class TestResource extends ExternalServiceResourceImpl {
        @JsonProperty("SimpleTypeCollection")
        @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
        Ref<List<Integer>> simpleTypeCollection = unassigned();

        @JsonProperty("EnumTypeCollection")
        @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
        Ref<List<TestEnum>> enumTypeCollection = unassigned();

        @JsonProperty("SomeEmptyCollection")
        @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
        Ref<List<String>> someEmptyCollection = unassigned();

        @JsonProperty("SomeAbsentCollection")
        @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
        Ref<List<Object>> someAbsentCollection = unassigned();

        @JsonProperty("SomeNullCollection")
        @AsUnassigned(WHEN_NULL)
        Ref<List<Integer>> someNullCollection = unassigned();

        @JsonProperty("EnumType")
        @AsUnassigned(WHEN_NULL)
        Ref<TestEnum> enumType = unassigned();

        @JsonProperty("Integer")
        @AsUnassigned(WHEN_NULL)
        Ref<Integer> integer = unassigned();

        @JsonProperty("String")
        @AsUnassigned(WHEN_NULL)
        Ref<String> string = unassigned();

        @JsonProperty("WillBeNull")
        @AsUnassigned(WHEN_NULL)
        Ref<String> willBeNull = unassigned();

        @JsonProperty("SomeAbsentField")
        @AsUnassigned(WHEN_NULL)
        Ref<String> someAbsentField = unassigned();

        @JsonProperty("CustomObject")
        @AsUnassigned(WHEN_NULL)
        Ref<CustomObject> customObject = unassigned();

        static class CustomObject {
            @JsonProperty("EnumType")
            @AsUnassigned(WHEN_NULL)
            Ref<TestEnum> enumType = unassigned();

            @JsonProperty("AssignedToNull")
            Ref<Integer> assignedToNull = unassigned();
        }
    }
}
