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

from include.psme_xml_structure.managers.usb_device_manager import UsbDeviceManager as UsbDeviceManager_abstract
from include.libs.lsusb import lsusb
from include.common.globals import *

class UsbDeviceManager(UsbDeviceManager_abstract):
    @classmethod
    def set_fields(cls, usb_device, data, context=None):

        usb_device.vendorId = data[DEV_VENDOR_ID]
        usb_device.deviceId = data[DEV_DEVICE_ID]
        return usb_device

    @classmethod
    def split_data(cls, data):
        # with removing duplicates
        return [dict(t) for t in set([tuple(d.items()) for d in lsusb.get_lsusb_info()])]

    @classmethod
    def prepare_data(cls, data):
        pass

