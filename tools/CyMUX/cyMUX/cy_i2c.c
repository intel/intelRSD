/*!
 * \copyright
 * Copyright (c) 2015, 2016 Intel Corporation. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \file receiver_thread.c
 *
 * \brief The main thread that recieves and handles all inbound messages on the i2c
 *
 */


#include "cy_i2c.h"
#include "configs.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>


I2cInfo_t gCy_bus[CY_BUS_MAX];
bool      gCy_initialized = false;


/*!
 * Initialize the driver/library for the threads that will talk to the Cypress part
 *
 * \author bjkurlex (11/5/15)
 *
 */
void init_i2c(void)
{
    I2cInfo_t*       bus         = gCy_bus;
    CY_DEVICE_INFO   cy_devinfo;
    CY_RETURN_STATUS cy_status;
    CY_HANDLE        cy_handle;
    UINT8            num_devices = 0;
    UINT8            device;
    int              interface;
    int              bus_count   = 0;

    if (gCy_initialized) return;

    debug(LOG_INFO, "Detecting CY7C65215 Hardware\n");

    cy_status = CyLibraryInit();
    if (CY_SUCCESS != cy_status) {
        fprintf(stderr, "CylibraryInit Error: %s\n", cy_strerror(cy_status));
        exit(1);
    }

    cy_status = CyGetListofDevices(&num_devices);
    if (cy_status != CY_SUCCESS) {
        fprintf(stderr, "CyGetListofDevices Error: %s\n", cy_strerror(cy_status));
        exit(1);
    }

    debug(LOG_INFO, "%d devices detected\n", num_devices);

    for (device = 0; device < num_devices; device++) {

        //probe each device to see if it is made by Cypress
        cy_status = CyGetDeviceInfo(device, &cy_devinfo);
        if (cy_status != CY_SUCCESS) continue;

        //It is a Cypress part if CyOpen returns success
        cy_status = CyOpen(device, 0, &cy_handle);
        if (cy_status != CY_SUCCESS) continue;

        debug(LOG_INFO, "Found CY at device %d\n", device);
        debug(LOG_INFO, "Number of interfaces: %d\n", cy_devinfo.numInterfaces);

        for (interface = 0; interface < cy_devinfo.numInterfaces; interface++) {
            if (CY_CLASS_VENDOR != cy_devinfo.deviceClass[interface]) continue;

            if (CY_TYPE_I2C == cy_devinfo.deviceType[interface]) {
                debug(LOG_INFO, "I2C Interface at %d\n", interface);

                bus->device = device;
                bus->interface = interface;

                bus++;
                bus_count++;
            }
        }

        cy_status = CyClose(cy_handle);
        break;  // Only find the first one for this implementation
    }

    if (bus_count < 2) {
        fprintf(stderr, "Error: CY7C65215: HW detection failed (or already bound by another process)\n");
        exit(1);
    }

    gCy_initialized = true;
}


void cleanup_i2c(void)
{
    CyLibraryExit();
}


/*!
 * Provide a string translation for the Cypress error codes
 *
 * \author bjkurlex (11/10/15)
 *
 * \param errnum Cypress error status code
 *
 * \return const char const*
 */
const char const* cy_strerror(CY_RETURN_STATUS errnum)
{
    switch (errnum) {
    case CY_SUCCESS:
        return "API returned successfully without any errors.";
    case CY_ERROR_ACCESS_DENIED:
        return "Access of the API is denied for the application ";
    case CY_ERROR_DRIVER_INIT_FAILED:
        return "Driver initialisation failed";
    case CY_ERROR_DEVICE_INFO_FETCH_FAILED:
        return "Device information fetch failed ";
    case CY_ERROR_DRIVER_OPEN_FAILED:
        return "Failed to open a device in the library ";
    case CY_ERROR_INVALID_PARAMETER:
        return "One or more parameters sent to the API was invalid";
    case CY_ERROR_REQUEST_FAILED:
        return "Request sent to USB Serial device failed ";
    case CY_ERROR_DOWNLOAD_FAILED:
        return "Firmware download to the device failed ";
    case CY_ERROR_FIRMWARE_INVALID_SIGNATURE:
        return "Invalid Firmware signature in firmware file";
    case CY_ERROR_INVALID_FIRMWARE:
        return "Invalid firmware ";
    case CY_ERROR_DEVICE_NOT_FOUND:
        return "Device disconnected ";
    case CY_ERROR_IO_TIMEOUT:
        return "Timed out while processing a user request";
    case CY_ERROR_PIPE_HALTED:
        return "Pipe halted while trying to transfer data";
    case CY_ERROR_BUFFER_OVERFLOW:
        return "OverFlow of buffer while trying to read/write data ";
    case CY_ERROR_INVALID_HANDLE:
        return "Device handle is invalid ";
    case CY_ERROR_ALLOCATION_FAILED:
        return "Error in Allocation of the resource inside the library";
    case CY_ERROR_I2C_DEVICE_BUSY:
        return "I2C device busy";
    case CY_ERROR_I2C_NAK_ERROR:
        return "I2C device NAK";
    case CY_ERROR_I2C_ARBITRATION_ERROR:
        return "I2C bus arbitration error";
    case CY_ERROR_I2C_BUS_ERROR:
        return "I2C bus error";
    case CY_ERROR_I2C_BUS_BUSY:
        return "I2C bus is busy";
    case CY_ERROR_I2C_STOP_BIT_SET:
        return "I2C master has sent a stop bit during a transaction";
    case CY_ERROR_STATUS_MONITOR_EXIST:
        return "API Failed because the SPI/UART status monitor thread already exists";
    }
    return "Invalid CY Error number";
}
