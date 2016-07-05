/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.services.detection.filesystem;

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
import static org.testng.Assert.assertTrue;

public class TmpLeasesFileRecordsParserTest {

    @Test
    public void whenValidPsmeLine_shouldReturnValidRecord() {
        TmpLeasesFileRecordsParser parser = new TmpLeasesFileRecordsParser();
        List<String> inputRecords = asList(TestData.VALID_PSME_LINE);
        Collection<TmpLeasesRecord> parsed = parser.toTmpLeasesRecordCollection(inputRecords);
        assertThat(parsed, contains(TestData.VALID_PSME_CANDIDATE));
    }

    @Test
    public void whenValidStorageLine_shouldReturnValidRecord() {
        TmpLeasesFileRecordsParser parser = new TmpLeasesFileRecordsParser();
        List<String> inputRecords = asList(TestData.VALID_STORAGE_LINE);
        Collection<TmpLeasesRecord> parsed = parser.toTmpLeasesRecordCollection(inputRecords);
        assertThat(parsed, contains(TestData.VALID_RSS_CANDIDATE));
    }

    @Test
    public void whenValidPsmeAndStorageLine_shouldReturnValidRecords() {
        TmpLeasesFileRecordsParser parser = new TmpLeasesFileRecordsParser();
        List<String> inputRecords = asList(
                TestData.VALID_PSME_LINE,
                TestData.VALID_STORAGE_LINE,
                TestData.VALID_RMM_LINE,
                TestData.VALID_LUI_LINE
        );
        Collection<TmpLeasesRecord> parsed = parser.toTmpLeasesRecordCollection(inputRecords);
        assertThat(parsed, contains(
                TestData.VALID_PSME_CANDIDATE,
                TestData.VALID_RSS_CANDIDATE,
                TestData.VALID_RMM_CANDIDATE,
                TestData.VALID_LUI_CANDIDATE)
        );
    }

    @Test
    public void whenJunk_shouldReturnEmptyList() {
        TmpLeasesFileRecordsParser parser = new TmpLeasesFileRecordsParser();
        List<String> inputRecords = asList(TestData.JUNK_LINE);
        Collection<TmpLeasesRecord> parsed = parser.toTmpLeasesRecordCollection(inputRecords);
        assertTrue(parsed.isEmpty());
    }

    @Test
    public void whenInvalidType_shouldReturnEmptyList() {
        TmpLeasesFileRecordsParser parser = new TmpLeasesFileRecordsParser();
        List<String> inputRecords = asList(TestData.INVALID_TYPE_LINE);
        Collection<TmpLeasesRecord> parsed = parser.toTmpLeasesRecordCollection(inputRecords);
        assertTrue(parsed.isEmpty());
    }

    @Test
    public void whenInvalidIp_shouldReturnEmptyList() {
        TmpLeasesFileRecordsParser parser = new TmpLeasesFileRecordsParser();
        List<String> inputRecords = asList(TestData.INVALID_IP_LINE);
        Collection<TmpLeasesRecord> parsed = parser.toTmpLeasesRecordCollection(inputRecords);
        assertTrue(parsed.isEmpty());
    }

    @Test
    public void whenMissingDate_shouldReturnEmptyList() {
        TmpLeasesFileRecordsParser parser = new TmpLeasesFileRecordsParser();
        List<String> inputRecords = asList(TestData.MISSING_DATE_LINE);
        Collection<TmpLeasesRecord> parsed = parser.toTmpLeasesRecordCollection(inputRecords);
        assertTrue(parsed.isEmpty());
    }

    @Test
    public void whenMixedInput_shouldHandleOnlyValidLines() {
        TmpLeasesFileRecordsParser parser = new TmpLeasesFileRecordsParser();
        List<String> inputRecords = asList(
                TestData.VALID_PSME_LINE,
                TestData.JUNK_LINE,
                TestData.VALID_STORAGE_LINE,
                TestData.INVALID_IP_LINE,
                TestData.VALID_RMM_LINE,
                TestData.INVALID_TYPE_LINE,
                TestData.VALID_LUI_LINE);

        Collection<TmpLeasesRecord> actual = parser.toTmpLeasesRecordCollection(inputRecords);
        assertThat(actual, contains(
                TestData.VALID_PSME_CANDIDATE,
                TestData.VALID_RSS_CANDIDATE,
                TestData.VALID_RMM_CANDIDATE,
                TestData.VALID_LUI_CANDIDATE)
        );
    }

    static class TestData {
        static final TmpLeasesRecord VALID_PSME_CANDIDATE = new TmpLeasesRecord(PSME, "10.0.0.1", "00:00:00:00:00:00", "2015-11-01T00:01:00.123456");
        static final TmpLeasesRecord VALID_RSS_CANDIDATE = new TmpLeasesRecord(RSS, "10.0.0.2", "00:00:00:00:00:00", "2015-11-01T00:02:00.123456");
        static final TmpLeasesRecord VALID_RMM_CANDIDATE = new TmpLeasesRecord(RMM, "10.0.0.3", "00:00:00:00:00:00", "2015-11-01T00:03:00.123456");
        static final TmpLeasesRecord VALID_LUI_CANDIDATE = new TmpLeasesRecord(LUI, "10.0.0.4", "00:00:00:00:00:00", "2015-11-01T00:04:00.123456");

        static final String VALID_PSME_LINE = "00:00:00:00:00:00 10.0.0.1 psme 2015-11-01T00:01:00.123456";
        static final String VALID_STORAGE_LINE = "00:00:00:00:00:00 10.0.0.2 storage 2015-11-01T00:02:00.123456";
        static final String VALID_RMM_LINE = "00:00:00:00:00:00 10.0.0.3 rmm 2015-11-01T00:03:00.123456";
        static final String VALID_LUI_LINE = "00:00:00:00:00:00 10.0.0.4 lui 2015-11-01T00:04:00.123456";

        static final String INVALID_IP_LINE = "00:00:00:00:00:00 999.999.999.999 psme 2015-11-01T00:01:00.123456";
        static final String INVALID_TYPE_LINE = "00:00:00:00:00:00 10.0.0.1 blade 2015-11-01T00:01:00.123456";
        static final String MISSING_DATE_LINE = "00:00:00:00:00:00 10.0.0.1 psme";

        static final String JUNK_LINE = "@$%@$TERFVSDFG";
    }
}
