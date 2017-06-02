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

package com.intel.podm.services.detection.dhcp.filesystem;

import org.testng.annotations.Test;

import java.util.Collection;
import java.util.List;

import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.types.ServiceType.RMM;
import static com.intel.podm.common.types.ServiceType.RSS;
import static java.util.Arrays.asList;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.contains;
import static org.hamcrest.Matchers.empty;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName"})
public class ServiceListFileRecordsParserTest {

    @Test
    public void whenValidInputWithNoType_shouldNotReturnValidPsmeCandidate() {
        ServiceListFileRecordsParser parser = new ServiceListFileRecordsParser();
        List<String> inputRecords = asList(TestData.VALID_URI_STRING_NO_TYPE);
        Collection<ServiceListRecord> parsed = parser.toServiceListRecords(inputRecords);
        assertThat(parsed, empty());
    }

    @Test
    public void whenValidInputPsmeType_shouldReturnValidPsmeCandidate() {
        ServiceListFileRecordsParser parser = new ServiceListFileRecordsParser();
        List<String> inputRecords = asList(TestData.VALID_URI_STRING_PSME_TYPE);
        Collection<ServiceListRecord> parsed = parser.toServiceListRecords(inputRecords);
        assertThat(parsed, contains(TestData.VALID_PSME_CANDIDATE));
    }

    @Test
    public void whenValidInputRssType_shouldReturnValidPsmeCandidate() {
        ServiceListFileRecordsParser parser = new ServiceListFileRecordsParser();
        List<String> inputRecords = asList(TestData.VALID_URI_STRING_RSS_TYPE);
        Collection<ServiceListRecord> parsed = parser.toServiceListRecords(inputRecords);
        assertThat(parsed, contains(TestData.VALID_RSS_CANDIDATE));
    }

    @Test
    public void whenValidInputRmmType_shouldReturnValidPsmeCandidate() {
        ServiceListFileRecordsParser parser = new ServiceListFileRecordsParser();
        List<String> inputRecords = asList(TestData.VALID_URI_STRING_RMM_TYPE);
        Collection<ServiceListRecord> parsed = parser.toServiceListRecords(inputRecords);
        assertThat(parsed, contains(TestData.VALID_RMM_CANDIDATE));
    }

    @Test
    public void whenValidInputLuiType_shouldReturnValidPsmeCandidate() {
        ServiceListFileRecordsParser parser = new ServiceListFileRecordsParser();
        List<String> inputRecords = asList(TestData.VALID_URI_STRING_LUI_TYPE);
        Collection<ServiceListRecord> parsed = parser.toServiceListRecords(inputRecords);
        assertThat(parsed, contains(TestData.VALID_LUI_CANDIDATE));
    }

    @Test
    public void whenInvalidUri_shouldReturnEmptyList() {
        ServiceListFileRecordsParser parser = new ServiceListFileRecordsParser();
        List<String> inputRecords = asList(TestData.JUNK_LINE);
        Collection<ServiceListRecord> parsed = parser.toServiceListRecords(inputRecords);
        assertTrue(parsed.isEmpty());
    }

    @Test
    public void whenInvalidServiceType_shouldReturnEmptyList() {
        ServiceListFileRecordsParser parser = new ServiceListFileRecordsParser();
        List<String> inputRecords = asList(TestData.VALID_URI_STRING_INVALID_TYPE);
        Collection<ServiceListRecord> parsed = parser.toServiceListRecords(inputRecords);
        assertTrue(parsed.isEmpty());
    }

    @Test
    public void whenMixedInput_shouldHandleOnlyValidLines() {
        ServiceListFileRecordsParser parser = new ServiceListFileRecordsParser();
        List<String> inputRecords = asList(
                TestData.VALID_URI_STRING_INVALID_TYPE,
                TestData.VALID_URI_STRING_NO_TYPE,
                TestData.VALID_URI_STRING_RSS_TYPE,
                TestData.VALID_URI_STRING_LUI_TYPE,
                TestData.VALID_URI_STRING_RMM_TYPE,
                TestData.VALID_URI_STRING_PSME_TYPE,
                TestData.JUNK_LINE);

        Collection<ServiceListRecord> parsed = parser.toServiceListRecords(inputRecords);
        assertThat(parsed, contains(
                TestData.VALID_RSS_CANDIDATE,
                TestData.VALID_LUI_CANDIDATE,
                TestData.VALID_RMM_CANDIDATE,
                TestData.VALID_PSME_CANDIDATE));
    }

    static class TestData {
        static final String VALID_URI_STRING_NO_TYPE = "http://10.0.0.1:8888/rest/v1";
        static final String VALID_URI_STRING_PSME_TYPE = "http://10.0.0.1:8888/rest/v1 psme";
        static final String VALID_URI_STRING_RSS_TYPE = "http://10.0.0.1:8888/rest/v1 rss";
        static final String VALID_URI_STRING_RMM_TYPE = "http://10.0.0.1:8888/rest/v1 rmm";
        static final String VALID_URI_STRING_LUI_TYPE = "http://10.0.0.1:8888/rest/v1 lui";
        static final String VALID_URI_STRING_INVALID_TYPE = "http://10.0.0.1:8888/rest/v1 zergrush";
        static final String JUNK_LINE = "@$%@$TERFVSDFG";
        static final ServiceListRecord VALID_PSME_CANDIDATE = new ServiceListRecord(PSME, VALID_URI_STRING_NO_TYPE);
        static final ServiceListRecord VALID_RSS_CANDIDATE = new ServiceListRecord(RSS, VALID_URI_STRING_NO_TYPE);
        static final ServiceListRecord VALID_RMM_CANDIDATE = new ServiceListRecord(RMM, VALID_URI_STRING_NO_TYPE);
        static final ServiceListRecord VALID_LUI_CANDIDATE = new ServiceListRecord(LUI, VALID_URI_STRING_NO_TYPE);
    }
}
