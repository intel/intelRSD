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
 * limitations under the License.s
 */

const BaseAsset = require('../common/baseAsset').BaseAsset;

class Processor extends BaseAsset {
    constructor(mock, system, id) {
        super(mock, system, id);
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('Processors');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Systems/Members/1/Processors/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Processor.v1_0_0.Processor',
            'Name': 'Processor',
            'Id': this.Id,
            'Socket': this.Socket || null,
            'ProcessorType': this.ProcessorType || null,
            'ProcessorArchitecture': this.ProcessorArchitecture || null,
            'InstructionSet': this.InstructionSet || null,
            'Manufacturer': this.Manufacturer || null,
            'Model': this.Model || null,
            'ProcessorId': this.ProcessorId || null,
            'MaxSpeedMHz': 3700,
            'TotalCores': this.TotalCores || null,
            'TotalThreads': this.TotalThreads || null,
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'Oem': {
                'Intel_RackScale': {
                    '@odata.type': '#Intel.Oem.Processor',
                    'Brand': this.Brand || null,
                    'Capabilities': this.Capabilities || []
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

    set Brand(brand) {
        this.brand = brand;
    }

    get Brand() {
        return this.brand;
    }

    get Manufacturer() {
        return this.manufacturer;
    }

    set Manufacturer(manufacturer) {
        this.manufacturer = manufacturer;
    }

    get Socket() {
        return this.socket;
    }

    set Socket(socket) {
        this.socket = socket;
    }

    get ProcessorType() {
        return this.processorType;
    }

    set ProcessorType(processorType) {
        this.processorType = processorType;
    }

    get ProcessorArchitecture() {
        return this.processorArchitecture;
    }

    set ProcessorArchitecture(processorArchitecture) {
        this.processorArchitecture = processorArchitecture;
    }

    get InstructionSet() {
        return this.instructionSet;
    }

    set InstructionSet(instructionSet) {
        this.instructionSet = instructionSet;
    }

    get Capabilities() {
        return this.capabilities;
    }

    set Capabilities(capabilities) {
        this.capabilities = capabilities;
    }

    get ProcessorId() {
        return this.processorId;
    }

    set ProcessorId(processorId) {
        this.processorId = processorId;
    }

    get TotalCores() {
        return this.totalCores;
    }

    set TotalCores(totalCores) {
        this.totalCores = totalCores;
    }

    get TotalThreads() {
        return this.totalThreads;
    }

    set TotalThreads(totalThreads) {
        this.totalThreads = totalThreads;
    }

    get Model() {
        return this.model;
    }

    set Model(model) {
        this.model = model;
    }
}

exports.Processor = Processor;
