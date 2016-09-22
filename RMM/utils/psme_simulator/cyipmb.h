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

#define MY_SLAVE_ADDR            0x08  //7-bit slave address for CY7C65215. must agree with configuration in SCB 1

#define STATUS_SUCCESS           0
#define STATUS_TIMEOUT           1
#define STATUS_BUSY              2
#define STATUS_INIT_ERROR        3
#define STATUS_UNKNOWN_ERROR     4
#define STATUS_INVALID_PARAM     5     //invalid input parameter
#define STATUS_HW_NOT_INIT       6     //attempt to perform read/write before HW initialization
#define STATUS_WRITE_ERROR       7
#define STATUS_CHECKSUM_ERROR    8


#pragma pack(push, 1)
typedef struct
{
   unsigned char  NetFn;         //for slave to parse
   unsigned char  Checksum;
   unsigned char  MasterAddr;    //sender's address
   unsigned char  SeqNum;
   unsigned char  Command;
   unsigned char  Data[1];       //can be the checksum if no data is associated
}  IpmbReq_t;

typedef struct
{
   unsigned char  NetFn;         //from slave
   unsigned char  Checksum;
   unsigned char  SlaveAddr;     //sender's address
   unsigned char  SeqNum;
   unsigned char  Command;
   unsigned char  CompCode;      //CC
   unsigned char  Data[1];
}  IpmbResp_t;
#pragma pack(pop)


//Initialize CY7C65215 hardware for send and receive operations
//Must be called before attempting send/receive
//Return value - status code
extern int CyHardwareInit(void);

//Receive an IPMB request frame. This function blocks until a frame is received of timeout occurs
//Timeout – input. Time to wait before detecting an error.
//DataLen – output. Number of bytes in Data[].
//Req – output. Pointer to caller memory where IPMB request is deposited.
//Return value – status code (TBD).
extern int ReceiveIpmbCmd(unsigned int Timeout, unsigned int *DataLen, IpmbReq_t *Req);

//Send an IPMB response frame
//Timeout – input. Time to wait before detecting an error.
//DestAddr – input. 8-bit destination/master address.
//DataLen – input. Number of bytes in Data[].
//Resp – input. Pointer to caller memory where IPMB response is deposited.
//Return value – status code (TBD).
extern int SendIpmbResp(unsigned int Timeout, unsigned char DestAddr, unsigned int DataLen, IpmbResp_t *Resp);