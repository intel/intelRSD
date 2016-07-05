/******************************************************************************
*
*              INTEL CORPORATION PROPRIETARY INFORMATION
*  This software is supplied under the terms of a license agreement or
*  nondisclosure agreement with Intel Corporation and may not be copied
*  or disclosed except in accordance with the terms of that agreement.
*
*  Copyright (c) 2015 Intel Corporation. All Rights Reserved.
*
********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "CyUSBSerial.h"
#include "cyipmb.h"

#define I2C_MASTER         0        //SCB 0 = master. this must match with the configuration in CY7C65215
#define I2C_SLAVE          1        //SCB 1 = slave
#define READ_MODULE        0
#define WRITE_MODULE       1
#define BUFF_LEN           64
#define BUS_COUNT          2        //CY7C65215 has 2 I2C busses

static CY_HANDLE  SlaveHandle = 0;
static CY_HANDLE  MasterHandle = 0;

typedef struct
{
   int Device;
   int Interface;
} I2cInfo_t;


//Receive an IPMB request frame. This function blocks until a frame is received of timeout occurs.
//Timeout – input. Time (in milliseconds) to wait before detecting an error. Use zero for unlimited timeout.
//DataLen – output. Number of bytes in Data[].
//Req – output. Pointer to caller memory where IPMB request is deposited. The memory buffer must support maximum IPMB message length.
//Return value – status code.
int ReceiveIpmbCmd(unsigned int Timeout, unsigned int *DataLen, IpmbReq_t *Req)
{
   CY_RETURN_STATUS        Status;
   CY_I2C_DATA_CONFIG      DataConfig;
   CY_DATA_BUFFER          ReadBuff;
   unsigned char           RcvData[BUFF_LEN];          //For incoming data
   unsigned char           ChkSum;
   int                     Len, Index, Result;
   
   //sanity check
   if((DataLen == 0) || (Req == 0))
      return STATUS_INVALID_PARAM;
      
   if(SlaveHandle == 0)
      return STATUS_HW_NOT_INIT;
   
   Result = STATUS_SUCCESS;
   DataConfig.slaveAddress = 0;     //slave address not used in slave mode
   DataConfig.isStopBit = true;
   DataConfig.isNakBit = false;

   memset(RcvData, 0, BUFF_LEN);
   ReadBuff.buffer = RcvData;
   ReadBuff.length = BUFF_LEN;
   ReadBuff.transferCount = 0;
   
   Status = CyI2cRead(SlaveHandle, &DataConfig, &ReadBuff, Timeout);
   
   switch(Status)
   {
      case CY_ERROR_I2C_STOP_BIT_SET:
        	//send a message
        	*DataLen = (unsigned int)ReadBuff.transferCount - sizeof(IpmbReq_t);
        	//printf("Received %d bytes\n", ReadBuff.transferCount);
        	memcpy(Req, RcvData, ReadBuff.transferCount);
         break;
         
      case CY_ERROR_IO_TIMEOUT:
         CyI2cReset(SlaveHandle, READ_MODULE);
         Result = STATUS_TIMEOUT;
         break;
         
      case CY_ERROR_I2C_DEVICE_BUSY:
         CyI2cReset(SlaveHandle, READ_MODULE);
         Result = STATUS_BUSY;
         break;
         
      default:
         Result = STATUS_UNKNOWN_ERROR;
         break;
   }
   
   //verify the header checksum
   ChkSum = (MY_SLAVE_ADDR << 1) + Req->NetFn + Req->Checksum;
   if(ChkSum)
      return STATUS_CHECKSUM_ERROR;
   
   //verify the payload checksum
   Len = ReadBuff.transferCount - sizeof(IpmbReq_t);
   ChkSum = Req->MasterAddr + Req->SeqNum + Req->Command;
   for(Index = 0; Index < Len; Index++)
      ChkSum += Req->Data[Index];

   ChkSum += Req->Data[Index];
   if(ChkSum)
      return STATUS_CHECKSUM_ERROR;
   
   return Result;
}

//Send an IPMB response frame
//Timeout – input. Time to wait before detecting an error.
//DestAddr – input. 8-bit destination/master address.
//DataLen – input. Number of bytes in Data[].
//Resp – input. Pointer to caller memory where IPMB response is deposited.
//Return value – status code (TBD).
int SendIpmbResp(unsigned int Timeout, unsigned char DestAddr, unsigned int DataLen, IpmbResp_t *Resp)
{
   CY_RETURN_STATUS        Status;
   CY_I2C_DATA_CONFIG      DataConfig;
   CY_DATA_BUFFER          WriteBuff;
   unsigned char           ChkSum;
   int                     Index, Result;
 
    //sanity check
   if(Resp == 0)
      return STATUS_INVALID_PARAM;
      
   if(MasterHandle == 0)
      return STATUS_HW_NOT_INIT;  
   
   Result = STATUS_SUCCESS;
      
   DataConfig.slaveAddress = DestAddr >> 1;           //Cypress part takes 7-bit address
   DataConfig.isStopBit = true;
   DataConfig.isNakBit = false;
   
   //set up dummy payload
   WriteBuff.length = DataLen + sizeof(IpmbResp_t);
   WriteBuff.transferCount = 0;
   WriteBuff.buffer = (unsigned char*)Resp;
   
   //Compute herder checksum
   Resp->Checksum = DestAddr + Resp->NetFn;
   Resp->Checksum ^= 0xFF;
   Resp->Checksum++;
   
   //Compute the payload checksum
   ChkSum = Resp->SlaveAddr + Resp->SeqNum + Resp->Command + Resp->CompCode;
   for(Index = 0; Index < DataLen; Index++)
      ChkSum += Resp->Data[Index];
      
   ChkSum ^= 0xFF;
   ChkSum++;
   Resp->Data[Index] = ChkSum;

   //printf("slave = %02X, length = %d\n", DataConfig.slaveAddress, WriteBuff.length);
   Status = CyI2cWrite(MasterHandle, &DataConfig, &WriteBuff, Timeout);
   if(Status != CY_SUCCESS)
   {
      printf("Write failed. Status = %d\n", Status);
      CyI2cReset(MasterHandle, WRITE_MODULE);
      Result = STATUS_WRITE_ERROR;
   }
      
   return Result;
}


//Initialize CY7C65215 hardware for send and receive operations
//Must be called before attempting send/receive
//Return value - status code
int CyHardwareInit(void)
{
   CY_DEVICE_INFO DeviceInfo;
   CY_RETURN_STATUS Status;
   CY_HANDLE Handle;
   I2cInfo_t Bus[5];    //any number larger than 2 is fine
   unsigned char NumDevices = 0;
   int Device, Interface, BusCount;
   
   Status = CyLibraryInit();
   if(Status != CY_SUCCESS) 
   {
     printf("Error initializing library. Error NO:<%d> \n", Status);
     CyLibraryExit();
     return STATUS_INIT_ERROR;
   }
   
   Status = CyGetListofDevices(&NumDevices);
   if(Status != CY_SUCCESS) 
   {
     printf("Error in getting number of devices. Error NO:<%d> \n", Status);
     CyLibraryExit();
     return STATUS_INIT_ERROR;
   }
   
   //printf ("%d devices detected\n", NumDevices);      
   BusCount = 0;
   
   for(Device = 0; Device < NumDevices; Device++)
   {
      //probe each device to see if it is made by Cypress
      Status = CyGetDeviceInfo(Device, &DeviceInfo);
      if(Status != CY_SUCCESS)
         continue;
         
      //It is a Cypress part if CyOpen returns success
      Status = CyOpen(Device, 0, &Handle);
      if(Status != CY_SUCCESS)
         continue;
         
      //printf("Found CY at device %d\n", Device);
      //printf("Number of interfaces: %d\n", DeviceInfo.numInterfaces);
      for(Interface = 0; Interface < DeviceInfo.numInterfaces; Interface++)
      {
         if(DeviceInfo.deviceClass[Interface] != CY_CLASS_VENDOR)
            continue;
            
         if(DeviceInfo.deviceType[Interface] == CY_TYPE_I2C)
         {
            //printf("I2C Interface at %d\n", Interface);
            Bus[BusCount].Device = Device;
            Bus[BusCount].Interface = Interface;
            
            BusCount++;
         }
      }
      
      CyClose(Handle);
   }
   
   if(BusCount != BUS_COUNT)
   {
     printf("Expected I2C port count: %d, actual: %d\n", BUS_COUNT, BusCount);
     CyLibraryExit();
     return STATUS_INIT_ERROR;
   }
   
   Status = CyOpen(Bus[I2C_MASTER].Device, Bus[I2C_MASTER].Interface, &MasterHandle);
   if(Status != CY_SUCCESS)
   {
      printf("Error opening master port\n");
      CyLibraryExit();
      return STATUS_INIT_ERROR;
   }
   
   Status = CyOpen(Bus[I2C_MASTER].Device, Bus[I2C_SLAVE].Interface, &SlaveHandle);
   if(Status != CY_SUCCESS)
   {
      printf("Error opening slave port\n");
      CyLibraryExit();
      return STATUS_INIT_ERROR;
   }
   
   //clear out errors, in case there is any
   CyI2cReset(MasterHandle, WRITE_MODULE);
   CyI2cReset(SlaveHandle, READ_MODULE);

   return STATUS_SUCCESS;
}


