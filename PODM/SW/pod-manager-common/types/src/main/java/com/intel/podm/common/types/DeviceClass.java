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

package com.intel.podm.common.types;

public enum DeviceClass implements EnumeratedType {
    UNCLASSIFIED_DEVICE("UnclassifiedDevice"),
    MASS_STORAGE_CONTROLLER("MassStorageController"),
    NETWORK_CONTROLLER("NetworkController"),
    DISPLAY_CONTROLLER("DisplayController"),
    MULTIMEDIA_CONTROLLER("MultimediaController"),
    MEMORY_CONTROLLER("MemoryController"),
    BRIDGE("Bridge"),
    COMMUNICATION_CONTROLLER("CommunicationController"),
    GENERIC_SYSTEM_PERIPHERAL("GenericSystemPeripheral"),
    INPUT_DEVICE_CONTROLLER("InputDeviceController"),
    DOCKING_STATION("DockingStation"),
    PROCESSOR("Processor"),
    SERIAL_BUS_CONTROLLER("SerialBusController"),
    WIRELESS_CONTROLLER("WirelessController"),
    INTELLIGENT_CONTROLLER("IntelligentController"),
    SATELLITE_COMMUNICATIONS_CONTROLLER("SatelliteCommunicationsController"),
    ENCRYPTION_CONTROLLER("EncryptionController"),
    SIGNAL_PROCESSING_CONTROLLER("SignalProcessingController"),
    PROCESSING_ACCELERATORS("ProcessingAccelerators"),
    NON_ESSENTIAL_INSTRUMENTATION("NonEssentialInstrumentation"),
    COPROCESSOR("Coprocessor"),
    UNASSIGNED_CLASS("UnassignedClass"),
    OTHER("Other");

    private final String value;

    DeviceClass(String value) {
        this.value = value;
    }

    public String getValue() {
        return value;
    }

    @Override
    public String toString() {
        return getValue();
    }
}
