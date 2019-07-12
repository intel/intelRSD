/*
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const BaseAsset = require('../common/baseAsset').BaseAsset;

class Memory extends BaseAsset {
    constructor(mock, system, id) {
        super(mock, system, id);
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('Memory');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Systems/Members/1/Memory/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Memory.v1_0_0.Memory',
            'Name': 'DIMM',
            'Id': this.Id,
            'Description' : 'description',
            'MemoryType': this.MemoryType || null,
            'MemoryDeviceType': this.MemoryDeviceType || null,
            'BaseModuleType': this.BaseModuleType || null,
            'MemoryMedia': this.MemoryMedia || [],
            'CapacityMiB': 8192,
            'DataWidthBits': this.DataWidthBits || null,
            'BusWidthBits': this.BusWidthBits || null,
            'Manufacturer': this.Manufacturer || null,
            'SerialNumber': this.SerialNumber || null,
            'PartNumber': this.PartNumber || null,
            'AllowedSpeedsMHz': this.AllowedSpeedsMhz || [],
            'FirmwareRevision': this.FirmwareRevision || null,
            'FirmwareApiVersion': this.FirmwareApiVersion || null,
            'FunctionClasses': this.FunctionClasses || [],
            'VendorID': this.VendorId || null,
            'DeviceID': this.DeviceId || null,
            'RankCount': this.RankCount || null,
            'DeviceLocator': this.DeviceLocator || null,
            'MemoryLocation': this.MemoryLocation || null,
            'ErrorCorrection': this.ErrorCorrection || null,
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'OperatingSpeedMhz': this.OperatingSpeedsMhz || 2400,
            'Regions': this.Regions || [],
            'OperatingMemoryModes': this.OperatingMemoryModes || [],
            'Oem': {
                'Intel_RackScale': {
                    '@odata.type': '#Intel.Oem.Memory',
                    'VoltageVolt': this.VoltageVolt || null
                }
            }
        };
    }

    set Status(val) {
        this.status = val;
    }

    get Status() {
        return this.status;
    }

    get VoltageVolt() {
        return this.voltageVolt;
    }

    set VoltageVolt(voltageVolt) {
        this.voltageVolt = voltageVolt;
    }

    get MemoryType() {
        return this.memoryType;
    }

    set MemoryType(memoryType) {
        this.memoryType = memoryType;
    }

    get MemoryDeviceType() {
        return this.memoryDeviceType;
    }

    set MemoryDeviceType(memoryDeviceType) {
        this.memoryDeviceType = memoryDeviceType;
    }

    get BaseModuleType() {
        return this.baseModuleType;
    }

    set BaseModuleType(baseModuleType) {
        this.baseModuleType = baseModuleType;
    }

    get MemoryMedia() {
        return this.memoryMedia;
    }

    set MemoryMedia(memoryMedia) {
        this.memoryMedia = memoryMedia;
    }

    get DataWidthBits() {
        return this.dataWidthBits;
    }

    set DataWidthBits(dataWidthBits) {
        this.dataWidthBits = dataWidthBits;
    }

    get BusWidthBits() {
        return this.busWidthBits;
    }

    set BusWidthBits(busWidthBits) {
        this.busWidthBits = busWidthBits;
    }

    get Manufacturer() {
        return this.manufacturer;
    }

    set Manufacturer(manufacturer) {
        this.manufacturer = manufacturer;
    }

    get SerialNumber() {
        return this.serialNumber;
    }

    set SerialNumber(serialNumber) {
        this.serialNumber = serialNumber;
    }

    get PartNumber() {
        return this.partNumber;
    }

    set PartNumber(partNumber) {
        this.partNumber = partNumber;
    }

    get AllowedSpeedsMhz() {
        return this.allowedSpeedsMhz;
    }

    set AllowedSpeedsMhz(allowedSpeedsMhz) {
        this.allowedSpeedsMhz = allowedSpeedsMhz;
    }

    set OperatingSpeedsMhz(operatingSpeedsMhz) {
        this.operatingSpeedsMhz = operatingSpeedsMhz;
    }

    get OperatingSpeedsMhz() {
        return this.operatingSpeedsMhz;
    }

    get FirmwareRevision() {
        return this.firmwareRevision;
    }

    set FirmwareRevision(firmwareRevision) {
        this.firmwareRevision = firmwareRevision;
    }

    get FirmwareApiVersion() {
        return this.firmwareApiVersion;
    }

    set FirmwareApiVersion(firmwareApiVersion) {
        this.firmwareApiVersion = firmwareApiVersion;
    }

    get FunctionClasses() {
        return this.functionClasses;
    }

    set FunctionClasses(functionClasses) {
        this.functionClasses = functionClasses;
    }

    get VendorId() {
        return this.vendorId;
    }

    set VendorId(vendorId) {
        this.vendorId = vendorId;
    }

    get DeviceId() {
        return this.deviceId;
    }

    set DeviceId(deviceId) {
        this.deviceId = deviceId;
    }

    get RankCount() {
        return this.rankCount;
    }

    set RankCount(rankCount) {
        this.rankCount = rankCount;
    }

    get DeviceLocator() {
        return this.deviceLocator;
    }

    set DeviceLocator(deviceLocator) {
        this.deviceLocator = deviceLocator;
    }

    get MemoryLocation() {
        return this.memoryLocation;
    }

    set MemoryLocation(memoryLocation) {
        this.memoryLocation = memoryLocation;
    }

    get ErrorCorrection() {
        return this.errorCorrection;
    }

    set ErrorCorrection(errorCorrection) {
        this.errorCorrection = errorCorrection;
    }

    get Regions() {
        return this.regions;
    }

    set Regions(regions) {
        this.regions = regions;
    }

    get OperatingMemoryModes() {
        return this.operatingMemoryModes;
    }

    set OperatingMemoryModes(operatingMemoryModes) {
        this.operatingMemoryModes = operatingMemoryModes;
    }
}

exports.Memory = Memory;
