"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

import unittest
import mock

import include.libs.dmidecode.dmidecode

from include.libs.dmidecode.dmidecode import _parse_handle_section, _parse_dmi, _profile, get_memory_info, ShellCommand, TYPE

NO_ROOT_RESPONSE = """# dmidecode 3.0
/sys/firmware/dmi/tables/smbios_entry_point: Permission denied
Scanning /dev/mem for entry point.
/dev/mem: Permission denied
"""

NO_SENSE_RESPONSE = """@!$^^!XYZ



ABC122"""

PROPER_HANDLE_SECTION = """Handle 0x0050, DMI type 131, 64 bytes
OEM-specific Type
\tHeader and Data:
\t\t83 40 50 00 35 00 00 00 00 00 00 00 00 00 00 00
\t\tF8 00 00 00 00 00 00 00 01 00 00 00 01 00 09 00
\t\t60 04 00 00 00 00 00 00 C8 00 00 00 00 00 00 00
\t\t00 00 00 00 26 00 00 00 76 50 72 6F 00 00 00 00
"""

SPACE_INSTEAD_OF_TABS = """Handle 0x0042, DMI type 17, 23 bytes
Physical Memory Array
    Location: System Board Or Motherboard
    Use: System Memory
    Error Correction Type: None
    Maximum Capacity: 32 GB
    Error Information Handle: Not Provided
    Number Of Devices: 4
"""

MEMORY_NO_SERIAL_NO = """Handle 0x0044, DMI type 17, 34 bytes
Memory Device
\tArray Handle: 0x0042
\tError Information Handle: Not Provided
\tTotal Width: 64 bits
\tData Width: 64 bits
\tSize: 8192 MB
\tForm Factor: DIMM
\tSet: None
\tLocator: ChannelA-DIMM1
\tBank Locator: BANK 1
\tType: DDR3
\tType Detail: Synchronous
\tSpeed: 1600 MHz
\tManufacturer: Kingston
\tAsset Tag: 9876543210
\tPart Number: 99U5471-056.A00LF
\tRank: 2
\tConfigured Clock Speed: 1600 MHz
"""

MULTIPLE_MEMORIES = """Handle 0x0042, DMI type 16, 23 bytes
Physical Memory Array
\tLocation: System Board Or Motherboard
\tUse: System Memory
\tError Correction Type: None
\tMaximum Capacity: 32 GB
\tError Information Handle: Not Provided
\tNumber Of Devices: 4

Handle 0x0043, DMI type 17, 34 bytes
Memory Device
\tArray Handle: 0x0042
\tError Information Handle: Not Provided
\tTotal Width: Unknown
\tData Width: Unknown
\tSize: No Module Installed
\tForm Factor: DIMM
\tSet: None
\tLocator: ChannelA-DIMM0
\tBank Locator: BANK 0
\tType: Unknown
\tType Detail: None
\tSpeed: Unknown
\tManufacturer: [Empty]
\tSerial Number: [Empty]
\tAsset Tag: Not Specified
\tPart Number: [Empty]
\tRank: Unknown
\tConfigured Clock Speed: Unknown

Handle 0x0044, DMI type 17, 34 bytes
Memory Device
\tArray Handle: 0x0042
\tError Information Handle: Not Provided
\tTotal Width: 64 bits
\tData Width: 64 bits
\tSize: 8192 MB
\tForm Factor: DIMM
\tSet: None
\tLocator: ChannelA-DIMM1
\tBank Locator: BANK 1
\tType: DDR3
\tType Detail: Synchronous
\tSpeed: 1600 MHz
\tManufacturer: Kingston
\tSerial Number: 7730A787
\tAsset Tag: 9876543210
\tPart Number: 99U5471-056.A00LF
\tRank: 2
\tConfigured Clock Speed: 1600 MHz

Handle 0x0045, DMI type 17, 34 bytes
Memory Device
\tArray Handle: 0x0042
\tError Information Handle: Not Provided
\tTotal Width: Unknown
\tData Width: Unknown
\tSize: No Module Installed
\tForm Factor: DIMM
\tSet: None
\tLocator: ChannelB-DIMM0
\t Bank Locator: BANK 2
\tType: Unknown
\tType Detail: None
\tSpeed: Unknown
\tManufacturer: [Empty]
\tSerial Number: [Empty]
\tAsset Tag: Not Specified
\tPart Number: [Empty]
\tRank: Unknown
\tConfigured Clock Speed: Unknown

Handle 0x0046, DMI type 17, 34 bytes
Memory Device
\tArray Handle: 0x0042
\tError Information Handle: Not Provided
\tTotal Width: 64 bits
\tData Width: 64 bits
\tSize: 8192 MB
\tForm Factor: DIMM
\tSet: None
\tLocator: ChannelB-DIMM1
\tBank Locator: BANK 3
\tType: DDR3
\tType Detail: Synchronous
\tSpeed: 1600 MHz
\tManufacturer: Kingston
\tSerial Number: 73305687
\tAsset Tag: 9876543210
\tPart Number: 99U5471-056.A00LF
\tRank: 2
\tConfigured Clock Speed: 1600 MHz

Handle 0x0047, DMI type 19, 31 bytes
Memory Array Mapped Address
\tStarting Address: 0x00000000000
\tEnding Address: 0x003FFFFFFFF
\tRange Size: 16 GB
\tPhysical Array Handle: 0x0042
\tPartition Width: 4

Handle 0x0048, DMI type 20, 35 bytes
Memory Device Mapped Address
\tStarting Address: 0x00000000000
\tEnding Address: 0x001FFFFFFFF
\tRange Size: 8 GB
\tPhysical Device Handle: 0x0046
\tMemory Array Mapped Address Handle: 0x0047
\tPartition Row Position: Unknown
\tInterleave Position: 1
\tInterleaved Data Depth: 2
"""

MEM1 = {'Manufacturer': 'Kingston',
       'Set': 'None',
       'Rank': '2',
       'Data Width': '64 bits',
       'Part Number': '99U5471-056.A00LF',
       'Type': 'DDR3',
       'Bank Locator': 'BANK 1',
       'Speed': '1600 MHz',
       'Error Information Handle': 'Not Provided',
       'Configured Clock Speed': '1600 MHz',
       'Locator': 'ChannelA-DIMM1',
       'Serial Number': '7730A787',
       'Total Width': '64 bits',
       'Asset Tag': '9876543210',
       'Type Detail': 'Synchronous',
       'Array Handle': '0x0042',
       'Form Factor': 'DIMM',
       '_title': 'Memory Device',
       'Size': '8192 MB'}

MEM2 = {'Manufacturer': 'Kingston',
       'Set': 'None',
       'Rank': '2',
       'Data Width': '64 bits',
       'Part Number': '99U5471-056.A00LF',
       'Type': 'DDR3',
       'Bank Locator': 'BANK 3',
       'Speed': '1600 MHz',
       'Error Information Handle': 'Not Provided',
       'Configured Clock Speed': '1600 MHz',
       'Locator': 'ChannelB-DIMM1',
       'Serial Number': '73305687',
       'Total Width': '64 bits',
       'Asset Tag': '9876543210',
       'Type Detail': 'Synchronous',
       'Array Handle': '0x0042',
       'Form Factor': 'DIMM',
       '_title': 'Memory Device',
       'Size': '8192 MB'}

MEM_TO_OBJ = [('memory_device',
              {'Manufacturer': '[Empty]',
               'Set': 'None',
               'Rank': 'Unknown',
               'Data Width': 'Unknown',
               'Part Number': '[Empty]',
               'Type': 'Unknown',
               'Bank Locator': 'BANK 0',
               'Speed': 'Unknown',
               'Error Information Handle': 'Not Provided',
               'Configured Clock Speed': 'Unknown',
               'Locator': 'ChannelA-DIMM0',
               'Serial Number': '[Empty]',
               'Total Width': 'Unknown',
               'Asset Tag': 'Not Specified',
               'Type Detail': 'None',
               'Array Handle': '0x0042',
               'Form Factor': 'DIMM',
               '_title': 'Memory Device',
               'Size': 'No Module Installed'}),
              ('memory_device', MEM1),
              ('memory_device',
              {'Manufacturer': '[Empty]',
               'Set': 'None',
               'Rank': 'Unknown',
               'Data Width': 'Unknown',
               'Part Number': '[Empty]',
               'Type': 'Unknown',
               'Bank Locator': 'BANK 2',
               'Speed': 'Unknown',
               'Error Information Handle': 'Not Provided',
               'Configured Clock Speed': 'Unknown',
               'Locator': 'ChannelB-DIMM0',
               'Serial Number': '[Empty]',
               'Total Width': 'Unknown',
               'Asset Tag': 'Not Specified',
               'Type Detail': 'None',
               'Array Handle': '0x0042',
               'Form Factor': 'DIMM',
               '_title': 'Memory Device',
               'Size': 'No Module Installed'}),
              ('memory_device', MEM2)
]

class DmiDecodeUnitTest(unittest.TestCase):
    def setUp(self):
        pass

    def test_no_root_response(self):
        with mock.patch('include.common.shell_command.ShellCommand.get_stdout', return_value=NO_SENSE_RESPONSE) as something:
            self.assertEqual(get_memory_info("whatever"), [])

        self.assertEqual(_parse_handle_section(iter(NO_SENSE_RESPONSE.splitlines())),
                                    {'_title': 'Handle 0x0050, DMI type 131, 64 bytes'})
        self.assertEqual(_parse_dmi(NO_SENSE_RESPONSE, 5), [])

    def test_no_root_response(self):
        with mock.patch('include.common.shell_command.ShellCommand.get_stdout', return_value=NO_ROOT_RESPONSE) as something:
            self.assertEqual(get_memory_info("whatever"), [])

        self.assertEqual(_parse_dmi(NO_ROOT_RESPONSE, 5), [])

    def test_no_memory_response(self):
        self.assertEqual(_parse_handle_section(iter(PROPER_HANDLE_SECTION.splitlines())),
                                    {'_title': 'Handle 0x0050, DMI type 131, 64 bytes'})

        with mock.patch('include.common.shell_command.ShellCommand.get_stdout', return_value=PROPER_HANDLE_SECTION) as something:
            self.assertEqual(get_memory_info("whatever"), [])

        self.assertEqual(_parse_dmi(PROPER_HANDLE_SECTION, 5), [])


    def test_multiple_memories_response(self):
        self.assertEqual(_parse_handle_section(iter(MULTIPLE_MEMORIES.splitlines())),
                                    {'_title': 'Handle 0x0042, DMI type 16, 23 bytes'})

        with mock.patch('include.common.shell_command.ShellCommand.get_stdout', return_value=MULTIPLE_MEMORIES) as something:
            self.assertEqual(get_memory_info("whatever"), MEM_TO_OBJ)
            self.assertEqual(_parse_dmi(MULTIPLE_MEMORIES, TYPE.keys()[0]), MEM_TO_OBJ)
            self.assertEqual(_profile(TYPE.keys()[0]), MEM_TO_OBJ)
            self.assertEqual(get_memory_info('7730A787'), MEM1)
            self.assertEqual(get_memory_info('73305687'), MEM2)


    def test_corrupted_data(self):
        with mock.patch('include.common.shell_command.ShellCommand.get_stdout', return_value=SPACE_INSTEAD_OF_TABS) as something:
            with self.assertRaises(KeyError):
                get_memory_info("whatever")
        with mock.patch('include.common.shell_command.ShellCommand.get_stdout', return_value=MEMORY_NO_SERIAL_NO) as something:
            with self.assertRaises(KeyError):
                get_memory_info("whatever")