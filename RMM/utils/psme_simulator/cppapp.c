#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "cyipmb.h"

#include<openssl/md5.h>
#include<string.h>



typedef char                    int8;                                               // 1 byte
typedef short                   int16;                                              // 2 bytes
typedef int                     int32;                                              // 4 bytes
typedef long long               int64;                                              // 8 bytes
typedef unsigned char           bool;
typedef unsigned char           uint8;                                              // 1 byte
typedef unsigned short          uint16;                                             // 2 bytes
typedef unsigned int            uint32;                                             // 4 bytes
typedef unsigned long long      uint64;                                             // 8 bytes


#define WAIT_FOREVER    0

#define DRAWER_IP_CMD           0x90
#define SET_ID_FIELD_CMD        0x06
#define GET_ID_FIELD_CMD        0x07

#define GET_AGGREGATED_THERMAL_SENSOR	0xC1
#define GET_AGGREGATED_HEALTH_SENSOR	0xC2
#define GET_TRAY_POWER					0xC3

#define DRAWER_SET_CERT		0xC4
#define DRAWER_GET_CERT		0xC5
#define DRAWER_GET_CERT_HASH	0xC6

#define DRAWER_CERT_TYPE_PODM	0x00
#define DRAWER_CERT_TYPE_RMM	0x01

#define FIELD_TYPE_DEFAULT      0x00

#define GET_LANCFG_CMD           0x02
#define GET_LANCFG_DATA0         0x01
#define GET_LANCFG_DATA1         0x03
#define GET_LANCFG_DATA2         0x00
#define GET_LANCFG_DATA3         0x00
#define GET_LANCFG_DATA_LEN      0x04
#define GET_LANCFG_IPSRC_DATA1			0x04
#define GET_LANCFG_SUBNET_MASK_DATA1	0x06


#define INSTANCE_RACK_PUID      0x01
#define INSTANCE_RACK_BPID      0x02
#define INSTANCE_DRAWER_RUID    0x03

#define MAX_CERT_LEN	8000
#define CERT_HASH_LEN	16

/* getAggregatedThermalSensor */
#define SLED_TYPE_HSW 	0
#define SLED_TYPE_BDX	1


typedef struct cert_vars {
	int16    cert_len;
	int16    cur_pos;
	int8     cur_chunk_num;
} cert_vars_t;

cert_vars_t RMMCert_vars;
uint8        RMMCert[MAX_CERT_LEN+1];
uint8		RMMCertHash[CERT_HASH_LEN];
static char RMMCertPath[]="./rmm_cert";
	
cert_vars_t PODMCert_vars;
uint8        PODMCert[MAX_CERT_LEN+1];
uint8		PODMCertHash[CERT_HASH_LEN];
static char PODMCertPath[]="./podm_cert";

static uint64 loop_cnt = 0;

#if 0
int generate_checksum(char *cert, char *hash){
    FILE *fstream=NULL;
	char command[64] = {};
    char buff[1024] = {};

	sprintf(command, "md5sum %s", cert);
    memset(buff,0,sizeof(buff));
    if(NULL==(fstream=popen(command,"r")))
    {
        fprintf(stderr,"execute command failed: %s",strerror(errno));
        return -1;
    }
    if(NULL!=fgets(buff, sizeof(buff), fstream))
    {
		buff[CERT_HASH_LEN] = '\0';
        printf("md5sum output is:%s",buff);
    }
    else
    {
		printf("md5sum command fail.\n");
        pclose(fstream);
        return -1;
    }
    pclose(fstream);
    return 0;
}
#endif


void print_data(uint8 *data, uint8 len)
{
	char buf[33]={'\0'};
	char tmp[3]={'\0'};
	int i;

	for( i=0; i<len; i++ ){
		sprintf(tmp,"%02X",data[i]);
		sprintf(tmp, "%2.2x", data[i]);
		strcat(buf,tmp);
	}

	printf("%s\n",buf);
}

int generate_checksum( unsigned char *data, int data_len, unsigned char *md)
{
	MD5_CTX ctx;
	//char *data="19870617";
	//unsigned char md[16];
	//char buf[33]={'\0'};
	//char tmp[3]={'\0'};
	//int i;

	MD5_Init(&ctx);
	MD5_Update(&ctx, data, data_len);
	MD5_Final(md,&ctx);

	//for( i=0; i<16; i++ ){
	//	sprintf(tmp,"%02X",md[i]);
	//	sprintf(tmp, "%2.2x", md[i]);
	//	strcat(buf,tmp);
	//}
	printf("MD5 SUM is:");
	print_data(md, 16);
	
	return 0;
}



//Sample application for sending and receiving IPMB frames
int main(int argc, char *argv[])
{
	int            Status;
	unsigned int   Index, Length;
	unsigned char  ReqBuff[64], Respbuff[64];     //buffers large enough for any IPMB frame
	IpmbReq_t      *Req;
	IpmbResp_t     *Resp;
	int i = 0 ;
	int rsp_data_len = 0 ; 

	cert_vars_t *ptr_cert_vars;
	uint8		*ptr_cert;
	uint8		*ptr_certHash;
	char		*cert_path;

	Resp = (IpmbResp_t  *)Respbuff;

	//Must call this API one time before sending and receiving
	Status = CyHardwareInit();
	if(STATUS_SUCCESS != Status)
	{
		printf("HW init failed\n");
		return -1;
	}

	printf("Ready to receive...\n");

	//Loop here a few times as an example of a task which sends canned responses to the master
	//Count = 200;
	while(1)
	{
		rsp_data_len = 0;
		Length = 0;

		loop_cnt++;
		
		Status = ReceiveIpmbCmd(WAIT_FOREVER, &Length, (IpmbReq_t *)ReqBuff);
		if(STATUS_SUCCESS != Status)
		{
			printf("Receive failed: %d\n", Status);
			return -1;
		}

		//Display the request
		Req = (IpmbReq_t *)ReqBuff;
		printf("CJ Incoming IPMB frame:\n");
		printf("NetFn =      %02X\n", Req->NetFn);
		printf("Master =     %02X\n", Req->MasterAddr);
		printf("Sequence =   %02X\n", Req->SeqNum);
		printf("Command =    %02X\n", Req->Command);
		printf("Length = %02x\n", Length);

		//Here is the data bytes associated with the request
		if(Length)
		{
			printf("CJ Data = ");
			for(Index = 0; Index < Length; Index++)
			printf("%02X ", Req->Data[Index]);
			printf("\n");
		}

		//Prepare a dummy response and send it
		Resp->NetFn       = Req->NetFn+4;                    //Set your NetFunc here
		Resp->Checksum    = 0;                       //API will calculate checksum
		Resp->SlaveAddr   = MY_SLAVE_ADDR << 1;      //Cypress device is programmed with this (8-bit) address
		Resp->SeqNum      = Req->SeqNum;                    //Caller must provide a sequence number
		Resp->Command     = Req->Command;                    //Set your command here
		//Resp->CompCode    = 0;                       //And the completion code

		if (Req->NetFn != 0x38<<2 && Req->NetFn != 0xc2<<2 && Req->NetFn != 0x06<<2) {
			Resp->CompCode = 0xC1;
			goto send_rsp;
		}

		/* handle warm reset command */
		if (Req->NetFn == 0x06<<2) {
			if (Req->Command!=0x03) {
				Resp->CompCode = 0xC1;
				goto send_rsp;
			}

			rsp_data_len = 0;
			goto send_rsp;
		}

		/* handle Get IP Configuration command */
		if (Req->NetFn == 0xc2<<2) {
			if (Req->Command!=0x02 || Req->Data[0]!=GET_LANCFG_DATA0 || Req->Data[2]!=GET_LANCFG_DATA2 || Req->Data[3]!=GET_LANCFG_DATA3) {
				Resp->CompCode = 0xC1;
				goto send_rsp;
			}

			switch(Req->Data[1]) {
				case GET_LANCFG_DATA1:
					rsp_data_len = 5;		/* get ip */
					
					Resp->Data[0] = 0x11;

					Resp->Data[1] = 192;
					Resp->Data[2] = 168;
					Resp->Data[3] = 0;
					Resp->Data[4] = 100;
					break;
				case GET_LANCFG_IPSRC_DATA1:
					rsp_data_len = 2;		/* get ip */
					
					Resp->Data[0] = 0x11;
					
					Resp->Data[1] = 2;	/* DHCP */
					break;
				case GET_LANCFG_SUBNET_MASK_DATA1:
					rsp_data_len = 5;

					Resp->Data[0] = 0x11;

					Resp->Data[1] = 255;
					Resp->Data[2] = 255;
					Resp->Data[3] = 255;
					Resp->Data[4] = 0;
					break;
			}

			goto send_rsp;
		}

		/* handle NetFn 0x38 commands */
		switch(Req->Command) {
			case DRAWER_SET_CERT:
				{
					if (Req->Data[0] == DRAWER_CERT_TYPE_PODM) {
						ptr_cert_vars = &PODMCert_vars;
						ptr_cert = PODMCert;
						ptr_certHash = PODMCertHash;
						cert_path = PODMCertPath;
					} else if (Req->Data[0] == DRAWER_CERT_TYPE_RMM) {
						ptr_cert_vars = &RMMCert_vars;
						ptr_cert = RMMCert;
						ptr_certHash = RMMCertHash;
						cert_path = RMMCertPath;
					} else {
						printf("Unsupported Cert Type %d.\n", Req->Data[0]);
						Resp->CompCode = 0xC9;                       //And the completion code
						goto send_rsp;
					}

					/* receive chunk 0, clean variables */
					if (Req->Data[3] == 0) {
						ptr_cert_vars->cert_len = 0;
						ptr_cert_vars->cur_chunk_num = 0;
						ptr_cert_vars->cur_pos = 0;
					}
					
					/* store cert len */
					if (ptr_cert_vars->cur_chunk_num == 0) {
						ptr_cert_vars->cert_len = Req->Data[1] | Req->Data[2]<<8;
						printf("cert len is %d.", ptr_cert_vars->cert_len);
					}

					/* check chunk num */
					if (ptr_cert_vars->cur_chunk_num != Req->Data[3]) {
						printf("Cert Chunk Num received is %d, should be %d.\n", Req->Data[3], ptr_cert_vars->cur_chunk_num);
						Resp->CompCode = 0xC9;                       //And the completion code
						goto send_rsp;
					} else
						ptr_cert_vars->cur_chunk_num = Req->Data[3]+1;

					/* store cert */
					{
						int cnt;

						for (cnt=0; cnt<Req->Data[4]; cnt++)
							ptr_cert[ptr_cert_vars->cur_pos+cnt] = Req->Data[5+cnt];
						ptr_cert_vars->cur_pos+=Req->Data[4];
					}

					/* check cert len */
					if (ptr_cert_vars->cur_pos > ptr_cert_vars->cert_len) {
						printf("cur pos %d exceed cert len %d.\n", ptr_cert_vars->cur_pos, ptr_cert_vars->cert_len);
						Resp->CompCode = 0xC9;                       //And the completion code
						goto send_rsp;
					}

					/* generate checksum */
					if (ptr_cert_vars->cur_pos == ptr_cert_vars->cert_len) {
						/* store cert */
                    	int rc;
                    	FILE *pCertFile = NULL;
                    	pCertFile = fopen(cert_path, "w+");
                    	if (pCertFile == NULL)
                    	{
	                    	printf("Open file failed! The program exist!\n");
	                    	Resp->CompCode = 0xCB;                     //And the completion code
	                    	fclose(pCertFile);
	                    	goto send_rsp;
                    	}

                    	rc = fwrite((const void *)ptr_cert, 1, ptr_cert_vars->cert_len, pCertFile);
                    	if (rc != ptr_cert_vars->cert_len) {
                        	printf("write cert failed! rc is %d, should be %d.\n", rc, ptr_cert_vars->cert_len);
                        	Resp->CompCode = 0xCB;                       //And the completion code
                       		fclose(pCertFile);
                      	 	goto send_rsp;
                    	}

                    	fclose(pCertFile);
						generate_checksum(ptr_cert, ptr_cert_vars->cert_len, ptr_certHash);

						/* clean */
						ptr_cert_vars->cert_len = 0;
						ptr_cert_vars->cur_chunk_num = 0;
						ptr_cert_vars->cur_pos = 0;
					}
				}
				
				break;
			case DRAWER_GET_CERT_HASH:
				{
					uint8 * md;

					if (Req->Data[0] == DRAWER_CERT_TYPE_PODM)
						md = PODMCertHash;
					else if (Req->Data[0] == DRAWER_CERT_TYPE_RMM)
						md = RMMCertHash;
					else {
						printf("cert type %d invalid.\n", Req->Data[0]);
						Resp->CompCode = 0xCB;  
						goto send_rsp;
					}
						
					rsp_data_len =16;
					for(i=0 ; i < rsp_data_len ; i++)
						Resp->Data[i] = md[i];
				}
				break;
			case SET_ID_FIELD_CMD:
				{
					char *ID_type;
					uint8 len;
					
					switch(Req->Data[1]) {
						case 0:
							ID_type = "Reserved";
							break;
						case 1:
							ID_type = "RackPUID";
							break;
						case 2:
							ID_type = "RackBPID";
							break;
						case 3:
							ID_type = "DrawerRUID";
							break;
						case 4:
							ID_type = "DrawerUUID";
							break;
						default:
							printf("id type %d invalid.\n", Req->Data[1]);
							Resp->CompCode = 0x80;
							goto send_rsp;
					}

					len = Req->Data[2];

					printf("SET_ID_FIELD_CMD %s, len %d ID data is:", ID_type, len);
					print_data(Req->Data+3, len);

					rsp_data_len = 1;
					Resp->Data[0] = 0x20; /* max id length */
				}
				break;
			case GET_AGGREGATED_THERMAL_SENSOR:
				rsp_data_len = 1+3*3;
				
				//if (loop_cnt>300)
					//Resp->Data[0] = 0x0; /* 111, first, second, and third sled present */
				//else
				Resp->Data[0] = 0x7;

				Resp->Data[1+0] = SLED_TYPE_HSW;	/* sled type */
				Resp->Data[1+1] = 0x11+loop_cnt%10;	/* desired pwm */
				Resp->Data[1+2] = 0x12+loop_cnt%10;	/* Inlet temperature */
				
				Resp->Data[4+0] = SLED_TYPE_BDX;	/* sled type */
				Resp->Data[4+1] = 0x13+loop_cnt%10;	/* desired pwm */
				Resp->Data[4+2] = 0x14+loop_cnt%10;	/* Inlet temperature */

				Resp->Data[7+0] = SLED_TYPE_HSW;	/* sled type */
				Resp->Data[7+1] = 0x15+loop_cnt%10;	/* desired pwm */
				Resp->Data[7+2] = 0x16+loop_cnt%10;	/* Inlet temperature */
				break;
			case GET_TRAY_POWER:
				rsp_data_len = 1+3*2;

				//if (loop_cnt>300)
					//Resp->Data[0] = 0x0; /* 111, first, second, and third sled present */
				//else
				Resp->Data[0] = 0x7;

				Resp->Data[1+0] = 0x33+loop_cnt%10;
				Resp->Data[1+1] = 0;
				
				Resp->Data[3+0] = 0x44+loop_cnt%10;
				Resp->Data[3+1] = 0x1;

				Resp->Data[5+0] = 0x55+loop_cnt%10;
				Resp->Data[5+1] = 0x1;
				break;
			case GET_AGGREGATED_HEALTH_SENSOR:
				rsp_data_len = 1;

				Resp->Data[0] = 0;	/* 0:OK, 0x80:Failure*/
				break;
			default:
				Resp->CompCode = 0xc1;
				goto send_rsp;
		}

#if 0
		rsp_data_len = 199;
		for(i=0 ; i < rsp_data_len ; i++)
			Resp->Data[i]     = i;                    //Caller sets zero or more data bytes


		Resp->CompCode    = 0x0;
#endif

		Resp->CompCode    = 0x0;
send_rsp:
		printf("Send RSP, rsp code[0x%x], data len[0x%x]\n\n", Resp->CompCode, rsp_data_len);
		Status = SendIpmbResp(10000,                 //Timeout value = 10 seconds
			//                            0x80,                  //8-bit (master) address to which this response is sent
			Req->MasterAddr ,
			rsp_data_len+1,                     //Number of data bytes in Data[] - see above
			Resp);

		if(STATUS_SUCCESS != Status)
		{
			printf("Send failed: %d\n", Status);
			return -1;
		}
	}  

return 0;
}
