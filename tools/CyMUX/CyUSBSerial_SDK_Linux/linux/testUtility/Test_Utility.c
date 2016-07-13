/*
 * Test utility
 * Copyright (C) 2013 Cypress Semiconductor
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <ctype.h>

#include "../../common/header/CyUSBSerial.h"
#define CY_MAX_DEVICES 30
#define CY_MAX_INTERFACES 4

typedef struct _CY_DEVICE_STRUCT {
    int deviceNumber;
    int interfaceFunctionality[CY_MAX_INTERFACES];
    bool isI2c;
    bool isSpi;
    int numInterface; 
}CY_DEVICE_STRUCT;

CY_DEVICE_STRUCT *glDevice;
int i2cDeviceIndex[CY_MAX_DEVICES][CY_MAX_INTERFACES];
unsigned char *deviceNumber = NULL;
int cyDevices, i2cDevices = 0, numDevices = 0;
int selectedDeviceNum = -1, selectedInterfaceNum = -1;
bool exitApp = false;
unsigned short pageAddress = -1;
short readWriteLength = -1;
bool deviceAddedRemoved = false;
int getUserInput()
{
    char userInput[6], x;
    int output,i = 0;
    bool isDigit = true;
    x = getchar();
    while (x != '\n'){
        if (i < 5){
            userInput[i] = x;
            i++;
        }
        if (!isdigit(x))
            isDigit = false;

        x = getchar();
    }
    userInput[i] = '\0';
    if (isDigit == false)
        return -1;
    output = atoi(userInput);
    return output;
}
void deviceHotPlug () {
    
    CY_RETURN_STATUS rStatus;
    deviceAddedRemoved = true;
    selectedDeviceNum = -1;
    selectedInterfaceNum = -1;
    printf ("Device of interest Removed/Added \n");
    rStatus = CyGetListofDevices (&numDevices);
    if (rStatus != CY_SUCCESS) {
        printf ("CY:Error in Getting List of Devices: Error NO:<%d> \n", rStatus);
        return rStatus;
    }
    printListOfDevices (false);
}

int main (int argc, char **agrv)
{
    int index = 0, i, j, userInput;
    int tempSelectedDeviceNum, tempSelectedInterfaceNum, tempPageAddress, tempLength;
    CY_RETURN_STATUS rStatus;
    signal (SIGUSR1, deviceHotPlug);
    glDevice = (CY_DEVICE_STRUCT *)malloc (CY_MAX_DEVICES *sizeof (CY_DEVICE_STRUCT));
    if (glDevice == NULL){
        printf ("Memory allocation failed ...!! \n");
        return -1;
    }
    rStatus = CyLibraryInit ();
    if (rStatus != CY_SUCCESS) {
        printf ("CY:Error in Doing library init Error NO:<%d> \n", rStatus);
        return rStatus;
    }
    rStatus = CyGetListofDevices (&numDevices);
    if (rStatus != CY_SUCCESS) {
        printf ("CY:Error in Getting List of Devices: Error NO:<%d> \n", rStatus);
        return rStatus;
    }
    printListOfDevices(true);
    do {

        printf ("-------------------------------------------------------------------\n");
        printf ("1: Print list of devices \n");
        if (selectedDeviceNum != -1 && selectedInterfaceNum != -1){
            printf ("2: Change device selection--selected device: [Device number %d] : [Interface No %d]",\
                    selectedDeviceNum, selectedInterfaceNum);
            if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == CY_TYPE_I2C)
                printf (" : I2C\n");
            else if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == CY_TYPE_SPI)
                printf (" : SPI\n");
            else
                printf (" : NA\n");

        }
        else    
            printf ("2: Select device...No device selected !!\n");

        if (readWriteLength != -1 && pageAddress != -1){
            printf ("3: Change Flash page address and length ...Entered is page address %d and length %d\n", pageAddress, readWriteLength);
            printf ("4: Verify data\n5: Exit\n");
        }
        else
            printf ("3: Enter I2C/SPI Flash page address and length to write/read.\n4: Verify data\n5: Exit\n");
        printf ("-------------------------------------------------------------------\n");

        userInput = getUserInput();
        if (userInput < 1 || userInput > 5){
            printf ("Wrong selection code ... Enter again \n");
            continue;
        }

        switch (userInput){

            case 1:
                printListOfDevices(true);
                break;
            case 2:
                if (cyDevices == 0) {
                    printf ("No device of interest connected ...!!\n");
                    continue;
                }
                printf ("Enter Device number to be selected.. \n");
                tempSelectedDeviceNum = getUserInput();
                //printf ("Selected device number is %d \n",tempSelectedDeviceNum);
                if (tempSelectedDeviceNum >= cyDevices || tempSelectedDeviceNum == -1){
                    printf ("Wrong device selection \n");
                    continue;
                }
                printf ("Enter interface number..\n");
                tempSelectedInterfaceNum = getUserInput();
                //printf ("Selected device number is %d %d\n",tempSelectedInterfaceNum, glDevice[tempSelectedDeviceNum].numInterface);

                if (tempSelectedInterfaceNum >= glDevice[tempSelectedDeviceNum].numInterface || 
                        tempSelectedInterfaceNum == -1) {
                    printf ("Wrong interface Selection selection \n");
                    continue;
                }
                if ((glDevice[tempSelectedDeviceNum].interfaceFunctionality[tempSelectedInterfaceNum] == -1)){
                    printf ("Selected device does not have I2C or SPI !!! \n");
                    continue;
                }
                if (deviceAddedRemoved == true) {
                    printf ("Device of interest was added/removed .... Print and select from new list\n");
                    continue;
                }
                selectedDeviceNum = tempSelectedDeviceNum;
                selectedInterfaceNum = tempSelectedInterfaceNum;
                //pageAddress = -1;
                //readWriteLength = -1;    
                break;
            case 3:
                if (selectedDeviceNum == -1) {
                    printf ("Select proper device!!! \n");
                    continue;
                }
                if (selectedDeviceNum >= cyDevices){
                    printf ("Select proper device!!! \n");
                    continue;
                }
                if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == CY_TYPE_I2C){
                    printf ("Enter Page address... (less than 256)\n");
                    tempPageAddress = getUserInput();
                    printf ("Enter length to read/write...(less than 64)\n");
                    tempLength = getUserInput();
                    if (deviceAddedRemoved == true) {
                        printf ("Device of interest was added/removed .... Print and select from new list\n");
                        continue;
                    }
                    if (tempPageAddress > 256){
                        printf ("Invalid page address ..Enter page address less than 256 (1 bytes len)\n");
                        continue;
                    }
                    if (tempLength < 0 || tempLength > 64){
                        printf ("Invalid Length ..Enter length less than 64 bytes\n");
                        continue;
                    }
                    pageAddress = tempPageAddress;
                    readWriteLength = tempLength;
                    break;
                }
                if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == CY_TYPE_SPI){
                    printf ("Enter Page address... (less than 65536)\n");
                    tempPageAddress = getUserInput();
                    printf ("Enter length to read/write...(less than 256)\n");
                    tempLength = getUserInput();
                    if (deviceAddedRemoved == true) {
                        printf ("Device of interest was added/removed .... Print and select from new list\n");
                        continue;
                    }
                    if (tempPageAddress > 65536){
                        printf ("Invalid page address ..Enter page address less than 65536 (1 bytes len)\n");
                        continue;
                    }
                    if (tempLength < 0 || tempLength > 256){
                        printf ("Invalid Length ..Enter length less than 256 bytes\n");
                        continue;
                    }
                    pageAddress = tempPageAddress;
                    readWriteLength = tempLength;
                    break;
                }
            case 4:
                if (selectedDeviceNum == -1) {
                    printf ("Select proper device!!! \n");
                    continue;
                }
                if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == -1){
                    printf ("Selected device does not have I2C or SPI !!! \n");
                    continue;
                }
                if ((pageAddress == -1) || (readWriteLength == -1)){
                    printf ("Select proper page address and length !!! \n");
                    continue;
                }
                if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == CY_TYPE_I2C){
                    if (pageAddress > 256){
                        printf ("Invalid Page address for I2C .. need to be less than 256\n");
                        continue;
                    }
                    if (readWriteLength > 64){
                        printf ("Invalid read write length for I2C .. need to be less than 64\n");
                        continue;
                    }
                    printf ("Calling I2C \n");
                    i2cVerifyData(glDevice[selectedDeviceNum].deviceNumber, selectedInterfaceNum);
                }
                if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == CY_TYPE_SPI){
                    spiVerifyData(glDevice[selectedDeviceNum].deviceNumber, selectedInterfaceNum);
                }
                break;

            case 5:
                exitApp = true;
                CyLibraryExit ();
                break;    
        }
    }while (exitApp == false);
    free (glDevice);
}
int spiWriteEnable (CY_HANDLE handle)
{
    unsigned char wr_data,rd_data;
    CY_RETURN_STATUS status = CY_SUCCESS;
    CY_DATA_BUFFER writeBuf;
    CY_DATA_BUFFER readBuf;

    writeBuf.buffer = &wr_data;
    writeBuf.length = 1;

    readBuf.buffer = &rd_data;
    readBuf.length = 1;

    wr_data = 0x06; /* Write enable */
    status = CySpiReadWrite (handle, &readBuf, &writeBuf, 5000);
    if (status != CY_SUCCESS)
    {
        return status;
    }
    return status;
}
//Helper functions for doing data transfer to/from SPI flash
int spiWaitForIdle (CY_HANDLE handle)
{
    char rd_data[2], wr_data[2];
    CY_DATA_BUFFER writeBuf, readBuf;
    writeBuf.length = 2;
    writeBuf.buffer = (unsigned char *)wr_data;
    int timeout = 0xFFFF;
    CY_RETURN_STATUS status;

    readBuf.length = 2;
    readBuf.buffer = (unsigned char *)rd_data;
    do
    {
        wr_data[0] = 0x05; /* Status */
        status = CySpiReadWrite (handle, &readBuf, &writeBuf, 5000);
        if (status != CY_SUCCESS)
        {
            break;
        }
        timeout--;
        if (timeout == 0)
        {
            status = CY_ERROR_IO_TIMEOUT;
            return status;
        }
    } while (rd_data[1] & 0x01);
    return status;
}

int spiVerifyData (int deviceNumber, int interfaceNum)
{
    CY_DATA_BUFFER dataBufferWrite,dataBufferRead;
    CY_HANDLE handle;
    bool isVerify = true;
    unsigned char wbuffer[256 + 4], rbuffer[256 + 4];
    int rStatus, length;

    memset (rbuffer, 0x00, 256);
    memset (wbuffer, 0x00, 256);

    rStatus = CyOpen (deviceNumber, interfaceNum, &handle);
    if (rStatus != CY_SUCCESS){
        printf ("CY_SPI: Open failed \n");
        return rStatus;
    }
    dataBufferWrite.buffer = wbuffer;
    dataBufferRead.buffer = rbuffer;

    rStatus = spiWaitForIdle (handle); 
    if (rStatus){
        printf("Error in Waiting for EEPOM active state %d \n", rStatus);
        CyClose (handle);

        return CY_ERROR_REQUEST_FAILED;
    }
    printf ("Calling spi write enable \n");
    rStatus = spiWriteEnable (handle);
    if (rStatus){
        printf("Error in setting Write Enable %d \n", rStatus);
        CyClose (handle);
        return CY_ERROR_REQUEST_FAILED;
    }
    //Write SPI write command
    wbuffer[0] = 0x02;
    //SPI flash address
    wbuffer[1] = (pageAddress >> 8);
    wbuffer[2] = (pageAddress) & 0x00FF;
    wbuffer[3] = 0;

    printf ("The Data written is ...\n");
    printf ("\n--------------------------------------------------------------------\n");
    for (rStatus = 4; rStatus < (readWriteLength + 4); rStatus++){
        wbuffer[rStatus] = rand() % 256;
        printf ("%x ",wbuffer[rStatus]);
    }
    printf ("\n--------------------------------------------------------------------\n");

    dataBufferRead.length = (4 + readWriteLength);
    dataBufferWrite.length = (4 + readWriteLength);

    rStatus = CySpiReadWrite (handle , &dataBufferRead, &dataBufferWrite, 5000);
    if (rStatus != CY_SUCCESS){
        CyClose (handle);
        printf ("Error in doing SPI data write data Write is %d data read is %d\n" , dataBufferWrite.transferCount,dataBufferRead.transferCount);
        return CY_ERROR_REQUEST_FAILED;    
    }

    spiWaitForIdle (handle);
    //Write SPI read command
    wbuffer[0] = 0x03;
    dataBufferRead.length =  (4 + readWriteLength);
    dataBufferWrite.length = (4 + readWriteLength);

    rStatus = CySpiReadWrite (handle, &dataBufferRead, &dataBufferWrite, 5000);
    if (rStatus != CY_SUCCESS){
        CyClose (handle);
        printf ("The Error is %d \n", rStatus);
        printf ("Error in doing SPI data write data Write is %d data read is %d\n" , dataBufferWrite.transferCount,dataBufferRead.transferCount);
        return CY_ERROR_REQUEST_FAILED;    
    }
    printf ("Data Read back is \n");
    printf ("\n---------------------------------------------------------------------\n");
    for (rStatus = 4; rStatus < (readWriteLength + 4); rStatus++){
        printf ("%x ",rbuffer[rStatus]);
        if (rbuffer[rStatus] != wbuffer[rStatus]){
            isVerify = false;
        }
    }
    printf ("\n---------------------------------------------------------------------\n");
    if (isVerify)
        printf ("Data verified successfully \n");
    else
        printf ("Data corruption occured!!\n");

    CyClose (handle);
    return CY_SUCCESS;
}

int i2cVerifyData (int deviceNumber, int interfaceNum)
{
    CY_DATA_BUFFER dataBufferWrite, dataBufferRead;
    CY_HANDLE handle;
    int length = 0;
    bool isVerify = true;
    int loopCount = 100, i;
    CY_RETURN_STATUS rStatus;
    unsigned char bytesPending = 0, address[2], wbuffer[66], rbuffer[66];
    CY_I2C_DATA_CONFIG i2cDataConfig;

    memset (wbuffer, 0, 66);
    memset (rbuffer, 0, 66);

    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x51;

    rStatus = CyOpen (deviceNumber, interfaceNum, &handle);
    if (rStatus != CY_SUCCESS){
        printf("CY_I2C: Open failed \n");
        return rStatus;
   }
    loopCount = 100;
    length = readWriteLength;
    wbuffer[0]= pageAddress;
    wbuffer[1] = 0;
    dataBufferWrite.buffer = wbuffer;
    i2cDataConfig.isStopBit = true;
    dataBufferWrite.length = (length + 2);
    printf ("\n Data that is written on to i2c ...\n");
    printf ("\n-----------------------------------------------------------------\n");
    for (i = 2; i < (length +2); i++){
        wbuffer[i] = rand() % 256;
        printf ("%x ", wbuffer[i]);
    }
    printf ("\n-----------------------------------------------------------------\n");
    rStatus = CyI2cWrite (handle, &i2cDataConfig, &dataBufferWrite, 5000);
    if (rStatus != CY_SUCCESS){
        printf ("Error in doing i2c write \n");
        CyClose (handle);
        return -1;
    }
    //We encountered a error in I2C read repeat the procedure again       
    //Loop here untill Read vendor command passes
    i2cDataConfig.isStopBit = false;
    dataBufferWrite.length = 2;

    do {
        rStatus = CyI2cWrite (handle, &i2cDataConfig, &dataBufferWrite, 5000); 
        loopCount--;
    }while (rStatus != CY_SUCCESS && loopCount != 0);
    
    if (loopCount == 0 && rStatus != CY_SUCCESS){
        printf ("Error in sending read command \n");
        CyClose (handle);
        return -1;
    }

    dataBufferRead.buffer = rbuffer;
    rbuffer[0]= address[0];
    rbuffer[1] = 0;
    i2cDataConfig.isStopBit = true; 
    i2cDataConfig.isNakBit = true;
    dataBufferRead.length = length; 
    dataBufferRead.buffer = rbuffer;

    memset (rbuffer, 0, 64);

    rStatus = CyI2cRead (handle, &i2cDataConfig, &dataBufferRead, 5000); 
    if (rStatus != CY_SUCCESS){
        printf ("Error in doing i2c read ... Error is %d \n", rStatus);
        CyClose (handle);
        return -1;
    }

    printf ("\n Data that is read from i2c ...\n");
    printf ("\n-----------------------------------------------------------------\n");
    for (rStatus = 0; rStatus < length; rStatus++){
        printf ("%x ", rbuffer[rStatus]);
        if (rbuffer[rStatus] != wbuffer[rStatus + 2]){
            isVerify = false;
        }
    }
    printf ("\n-----------------------------------------------------------------\n");
    if (!isVerify)
        printf ("Data corruption occured ..!!!\n");
    else
        printf ("Data verified successfully \n");
    
    CyClose (handle); 
}
bool isCypressDevice (int deviceNum) {
    CY_HANDLE handle;
    unsigned char interfaceNum = 0;
    unsigned char sig[6];
    CY_RETURN_STATUS rStatus;
    rStatus = CyOpen (deviceNum, interfaceNum, &handle);
    if (rStatus == CY_SUCCESS){
        rStatus = CyGetSignature (handle, sig);
        if (rStatus == CY_SUCCESS){
            CyClose (handle);
            return true;
        }
        else {
            CyClose (handle);
            return false;
        }
    }
    else 
        return false;
}
void printListOfDevices (bool isPrint)
{
    int  index_i = 0, index_j = 0, i, j, countOfDevice = 0, devNum;
    int length, index = 0, numInterfaces, interfaceNum;
    bool set1 = false;

    unsigned char deviceID[CY_MAX_DEVICES];
    unsigned char functionality[64];
    CY_DEVICE_INFO deviceInfo;
    CY_DEVICE_CLASS deviceClass[CY_MAX_INTERFACES];
    CY_DEVICE_TYPE  deviceType[CY_MAX_INTERFACES];
    CY_RETURN_STATUS rStatus;

    deviceAddedRemoved = false; 
    CyGetListofDevices (&numDevices);
    //printf ("The number of devices is %d \n", numDevices); 
    for (i = 0; i < numDevices; i++){
        for (j = 0; j< CY_MAX_INTERFACES; j++)
            glDevice[i].interfaceFunctionality[j] = -1;
    }
    if (isPrint){
        printf ("\n\n---------------------------------------------------------------------------------\n");
        printf ("Device Number | VID | PID | INTERFACE NUMBER | FUNCTIONALITY \n");
        printf ("---------------------------------------------------------------------------------\n");
    }
    cyDevices = 0;
    for (devNum = 0; devNum < numDevices; devNum++){
        rStatus = CyGetDeviceInfo (devNum, &deviceInfo);
        interfaceNum = 0;
        if (!rStatus)
        {
            if (!isCypressDevice (devNum)){
                continue;
            }
            strcpy (functionality, "NA");
            numInterfaces = deviceInfo.numInterfaces;
            glDevice[index].numInterface = numInterfaces;
            cyDevices++;
            
            while (numInterfaces){
                if (deviceInfo.deviceClass[interfaceNum] == CY_CLASS_VENDOR)
                {
                    glDevice[index].deviceNumber = devNum;
                    switch (deviceInfo.deviceType[interfaceNum]){
                        case CY_TYPE_I2C:
                            glDevice[index].interfaceFunctionality[interfaceNum] = CY_TYPE_I2C;
                            strcpy (functionality, "VENDOR_I2C");
                            glDevice[index].isI2c = true;
                            break;
                        case CY_TYPE_SPI:
                            glDevice[index].interfaceFunctionality[interfaceNum] = CY_TYPE_SPI;
                            strcpy (functionality, "VENDOR_SPI");
                            glDevice[index].isSpi = true;
                            break;
                        default:
                            strcpy (functionality, "NA");
                            break;    
                    }
                }
                else if (deviceInfo.deviceClass[interfaceNum] == CY_CLASS_CDC){
                    strcpy (functionality, "NA");
                }
                if (isPrint) {
                    printf ("%d             |%x  |%x    | %d     | %s\n", \                
                            index, \
                            deviceInfo.vidPid.vid, \
                            deviceInfo.vidPid.pid,  \
                            interfaceNum, \
                            functionality \
                            );
                }
                interfaceNum++;
                numInterfaces--;
            }
            index++;
        }
    }
    if (isPrint){
        printf ("---------------------------------------------------------------------------------\n\n");
    }
}

