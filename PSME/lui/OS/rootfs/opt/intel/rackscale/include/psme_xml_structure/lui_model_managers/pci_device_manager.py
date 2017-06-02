"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

from include.psme_xml_structure.managers.pci_device_manager import PciDeviceManager as PciDeviceManager_abstract
from include.libs.lspci import lspci
from include.common.globals import *

class PciDeviceManager(PciDeviceManager_abstract):
    @classmethod
    def set_fields(cls, pci_device, data, context=None):

        pci_device.vendorId = data[DEV_VENDOR_ID]
        pci_device.deviceId = data[DEV_DEVICE_ID]
        return pci_device

    @classmethod
    def split_data(cls, data):
        # with removing duplicates
        return [dict(t) for t in set([tuple(d.items()) for d in lspci.get_lspci_info()])]

    @classmethod
    def prepare_data(cls, data):
        pass

