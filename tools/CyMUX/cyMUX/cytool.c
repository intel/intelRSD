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
 * \file cytool.c
 *
 * \brief command line tool to interface with cyMUX
 *
 */


#include "IPMB.h"
#include "configs.h"
#include "stringify.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <assert.h>


// minimum number of bytes in an IPMB response (without slave address)
#define MIN_RESP_LEN    (sizeof(IpmbResp_t) + sizeof(uint8_t))

// Command line option flags
#define SLAVE_OPT       0x01
#define NETFN_OPT       0x04
#define CMD_OPT         0x08
#define BRIDGE_OPT      0x20
#define CHANNEL_OPT     0x40

// SendMessage NetFn/Cmd
#define NETFN_APP       0x06
#define CMD_SEND_MSG    0x34


/**
 * Verbosity defintion providing mneumonics for levels of verbosity
 */
typedef enum {
    STANDARD = 0,
    INFO,
    PROGRESS,
    PKT_DUMP,
    DEBUG
} verbosity_t;


/**
 * Parameter structure containing configuration passed via the command line
 */
typedef struct {
    uint8_t         OptFlag;    // Bitflag of options set
    uint8_t         TargetAddr; // Target slave address when bridging
    uint8_t         SlaveAddr;  // Target slave address when bridging
    uint8_t         NetFn;      // Net Function to issue
    uint8_t         Command;    // Command associated to NetFn to use
    uint8_t         BridgeChannel;
    size_t          Length;     // Number of bytes to send
    size_t          TimeOut;    // Time out in seconds
    size_t          Retry;      // retries after timeout
    verbosity_t     Verbosity;  // Levels of diagnostics
    uint8_t         Data[MAX_IPMB_PKT_SIZE]; // data for request
    int             Port;       // Server port to use
} params_t;


//forward declaration
static int Ascii2Hex(const char const* Str, uint8_t* Hex);
static uint8_t GetSeqNum(void);
static void usage(int exit_code);
static void process_commandline(int ac, char* av[], params_t* p);
static size_t craft_request(uint8_t* pReq, size_t len, const params_t const* p);
static void print_request(const uint8_t const* pReq, const params_t const* p);
static void print_response(const uint8_t const* pResp, size_t len, const params_t const* p);
static void print_buf(const char const* prefix, const uint8_t const* buf, size_t len, const params_t const* p);
static int attach2MUX(params_t* p);


/*!
 * Main entrypoint for cytool utility
 *
 * \author amao/bjkurlex
 *
 * \param argc Count of command line arguments
 * \param argv array of pointers to the command line parameters
 *
 * \return int program exit code
 *
 */
int main(int argc, char* argv[])
{
    int            server_sock;    // Socket descriptor for new connections
    ssize_t        bytes_sent;
    ssize_t        bytes_read;
    size_t         reqlen;
    size_t         Result      = -1;                // error status
    uint8_t        XmtData[MAX_IPMB_PKT_SIZE+1];    // For outgoing data
    uint8_t        RcvData[MAX_IPMB_PKT_SIZE+1];    // For incoming data
    uint8_t        ChkSum      = 0;

    IpmbReq_t*     Request     = (IpmbReq_t*)(XmtData + 1);
    IpmbResp_t*    Response    = (IpmbResp_t*)(RcvData + 1);
    size_t         retry_count = 0;
    params_t       params;
    fd_set         rfds;
    struct timeval tv;
    int            retval;


    /**
     * Initialize all data structures not already initialized
     */
    bzero(&params, sizeof(params));
    bzero(XmtData, sizeof(XmtData));
    bzero(RcvData, sizeof(RcvData));

    /**
     * Parse the command line for options
     */
    process_commandline(argc, argv, &params);

    /**
     * Craft the request packet based on the command line parameters
     */
    reqlen = craft_request(XmtData, sizeof(XmtData) - 1U, &params);


    /**
     * Establish a connection with cyMUX
     * Will exit if there was any error
     * encountered
     */
    server_sock = attach2MUX(&params);


    do {
        print_request(XmtData, &params);

        //============ Transmit the request =============
        print_buf("XMT", XmtData, reqlen, &params);

        bytes_sent = send(server_sock, XmtData, reqlen, 0);
        if (bytes_sent < 0) {
            fprintf(stderr, "IPMB send failure: %s\n", strerror(errno));
            break;
        }
        assert((size_t)bytes_sent == reqlen);

        if (params.Verbosity >= PROGRESS)
            printf("%ld bytes transmitted successfully\n", bytes_sent);

        tv.tv_sec = params.TimeOut;            // Reset timeout
        tv.tv_usec = 0;

        FD_ZERO(&rfds);                 // Reset socket handle
        FD_SET(server_sock, &rfds);

        retval = select(server_sock + 1, &rfds, NULL, NULL, &tv);
        if (retval < 0) {
            fprintf(stderr, "Response read failure: %s\n", strerror(errno));
            break;
        }
        if (0 == retval) {
            // retry.
            if (params.Retry > 0) {
                retry_count++;
                if (retry_count <= params.Retry) {
                    if (params.Verbosity >= PROGRESS)
                        printf("Retrying(%ld)...\n", retry_count);
                    continue;
                }
                fprintf(stderr, "Response failure (timed out after %ld retried attempts)\n",
                        params.Retry);
                break;
            }
            fprintf(stderr, "Response failure (timed out on read)\n");
            break;
        }

        /**
         * Read the response
         */
        bytes_read = recv(server_sock, RcvData, sizeof(RcvData), 0);
        if (0 == bytes_read) {
            fprintf(stderr, "Server closed connection during receipt of IPMB response\n");
            break;
        }
        else if (bytes_read < 0) {
            fprintf(stderr, "IPMB recv failure: %s\n", strerror(errno));
            break;
        }
        if (params.Verbosity >= PROGRESS)
            printf("%ld bytes received\n", bytes_read);

        print_buf("RCV", RcvData, bytes_read, &params);
        print_response(RcvData, bytes_read, &params);
        // Initialize the Checksum to the response
        ChkSum = Response->SlaveAddr + Response->SeqNum + Response->Command + Response->CompCode;
        Result = Response->CompCode;
        if (Response->CompCode != 0) {
            if (Response->CompCode == 0xd3) {
                if (params.Retry > 0) {
                    retry_count++;
                    if (retry_count <= params.Retry) {
                        if (params.Verbosity >= PROGRESS)
                            printf("Retrying(%ld)...\n", retry_count);
                        usleep((retry_count * 100 + (5 * (getpid() % 4))) * 1000);
                        continue;
                    }
                }
            }
            fprintf(stderr, "cytool: Unable to send raw command (netfn=0x%x lun=0x%x cmd=0x%x rsp=0x%x)\n",
                    Request->NetFn >> 2, Request->NetFn & 3, Request->Command, Response->CompCode);
            break;
        }
        //                       (slave address)   (response header)    (checksum)
        if ((size_t)bytes_read > sizeof(uint8_t) + sizeof(IpmbResp_t) + sizeof(uint8_t)) {
            if ((params.OptFlag & BRIDGE_OPT) &&
                //                     (slave address)   (response header)    (checksum)          (Bridge header)
                (((size_t)bytes_read - sizeof(uint8_t) - sizeof(IpmbResp_t) - sizeof(uint8_t)) >= sizeof(IpmbResp_t))) {

                // Point to Bridged response
                IpmbBridgedReq_t* BridgeReq  = (IpmbBridgedReq_t*)(Request->Data);
                IpmbResp_t*       BridgeResp = (IpmbResp_t*)(Response->Data + sizeof(uint8_t));
                //size_t      DataLength = bytes_read - MIN_RESP_LEN - sizeof(IpmbResp_t) - 1UL;
                size_t            DataLength = bytes_read - sizeof(uint8_t)      // slave address
                                                          - sizeof(IpmbResp_t)   // Main response header
                                                          - sizeof(uint8_t)      // Bridged req addr
                                                          - sizeof(IpmbResp_t)   // Bridged response
                                                          - sizeof(uint8_t);     // Final checksum


                Result = BridgeResp->CompCode;  // Update the CC to the response from the bridge
                if (BridgeResp->CompCode != 0) {
                    fprintf(stderr, "cytool: Unable to send bridged raw command (netfn=0x%x lun=0x%x cmd=0x%x rsp=0x%x)\n",
                            BridgeReq->NetFn >> 2, BridgeReq->NetFn & 3, BridgeReq->Command, BridgeResp->CompCode);
                }

                if (params.Verbosity < INFO && DataLength > 0) {
                    for (size_t Index = 0; Index < DataLength; Index++)
                        printf("%02X ", BridgeResp->Data[Index]);
                    printf("\n");
                }
            }
            else if (params.Verbosity < INFO) {
                // Non-bridged data
                //                                          (slave addr)      (header)             (checksum)
                for (size_t Index = 0; Index < bytes_read - sizeof(uint8_t) - sizeof(IpmbResp_t) - sizeof(uint8_t); Index++)
                    printf("%02X ", Response->Data[Index]);
                printf("\n");
            }

            // Calculate the rest of the check sum for validation
            //                                          (slave addr)      (header)
            for (size_t Index = 0; Index < bytes_read - sizeof(uint8_t) - sizeof(IpmbResp_t); Index++)
                ChkSum += Response->Data[Index];
        }
        else {
            ChkSum += Response->Data[0];
        }

        if (ChkSum) {
            fprintf(stderr, "cytool: Error: Checksum differential was %02X, not 0\n", ChkSum);
            Result = -1;
        }

        break;
    } while (retry_count <= params.Retry);    // IPMB packet retry loop (work around Klockwork complaint)

    close(server_sock);

    return Result;
}


/*!
 * Given an ASCII string, convert it into its expected hex format
 *
 * \author amao
 *
 * \param Str String to convert into hex
 * \param Hex Where to store the Hex conversion
 *
 * \return int success or failure of the conversion
 *
 */
static int Ascii2Hex(const char const* Str, uint8_t* Hex)
{
    int  Val   = 0;
    char Digit;


    if ((Str == NULL) || (Hex == NULL)) return -1;

    while (*Str) {
        if ((*Str >= '0') && (*Str <= '9')) Digit = *Str - '0';
        else if ((*Str >= 'a') && (*Str <= 'f')) Digit = (*Str) - 'a' + 10;
        else if ((*Str >= 'A') && (*Str <= 'F')) Digit = (*Str) - 'A' + 10;
        else {
            fprintf(stderr, "cytool: Error: Do not know hex: %s\n", Str);
            return -1;
        }

        Val = (Val * 16) + Digit;
        Str++;
    }

    *Hex = (unsigned char)Val;
    return 0;
}


/*!
 * Create a pseudo random sequence number to use for the IPMB request
 *
 * \author amao
 *
 * \param void
 *
 * \return uint8_t Generated sequence number
 *
 */
static uint8_t GetSeqNum(void)
{
    unsigned char Rand   = 0;
    int           Result;
    int           Handle;

    Handle = open("/dev/urandom", O_RDONLY);
    if (Handle < 0) {
        perror("cytool: Error: Unable to open random value from /dev/urandom\n");
        return 0;
    }

    //read multiple times to increase the likelyhood of getting a random value.
    Result = read(Handle, &Rand, 1);
    Result += read(Handle, &Rand, 1);
    Result += read(Handle, &Rand, 1);
    Result += read(Handle, &Rand, 1);

    close(Handle);
    return Rand;
}


/*!
 * Display the program usage and then exit using the value passed
 *
 * \author amao/bjkurlex
 *
 * \param exit_code exit value to use when exiting program
 *
 */
static void usage(int exit_code)
{
    printf("cytool: send and receive IPMB command via CY7C65215\n");
    printf("Version: v%s\n", xstr(__CYTOOL_VERSION__));
    printf("Options:\n");
    printf("    -s, --slave-addr=ADDR       mandatory (8-bit hex) slave address.\n");
    printf("    -n, --netfn=NETFN           mandatory (6-bit hex) Net Func.\n");
    printf("    -c, --cmd=CMD               mandatory (hex) command.\n");
    printf("    -d, --data DATA1 DATA2 ...  optional (8-bit hex) data byte(s) to send.\n");
    printf("        option has been deprecated and additional bytes to send should be last\n");
    printf("    -b, --channel=CHANNEL       destination channel for bridge request\n");
    printf("    -t, --target-addr=ADDR      bridge to remote target (8-bit hex) address\n");
    printf("    -r, --retry=COUNT           retry count.\n");
    printf("    -e, --timeout=SECOND        receive timeout (expiry) value (sec).\n");
    printf("    -v, --verbosity=LEVEL       verbosity level. max 2.\n");
    printf("    -p, --port=PORT             cyMUX server port\n");
    printf("   --version                    Version string for cytool\n");
    printf("\n");
    printf("slave addr, netfn, cmd, and data are all presumed and treaded as hex values\n");
    printf("(no 0x prefix is needed)\n");
    exit(exit_code);
}


/*!
 *
 *
 * \author bjkurlex (11/25/15)
 *
 * \param ac
 * \param av
 * \param p
 *
 */
static void process_commandline(int ac, char* av[], params_t* p)
{
    int      c;         // Current command line option to process
    uint8_t* Byte  = NULL;
    static struct option long_options[] = {
        // Name         has_arg             *flag   val
        { "help",       no_argument,        NULL,   'h' },
        { "verbosity",  required_argument,  NULL,   'v' },
        { "version",    no_argument,        NULL,    0  },
        { "timeout",    required_argument,  NULL,   'e' },
        { "slave-addr", required_argument,  NULL,   's' },
        { "target-addr",required_argument,  NULL,   't' },
        { "data",       required_argument,  NULL,   'd' },
        { "retry",      required_argument,  NULL,   'r' },
        { "port",       required_argument,  NULL,   'p' },
        { NULL,         no_argument,        NULL,    0  }     // Termination entry    // 0
    };


    assert(NULL != av);
    assert(NULL != p);
    bzero(p, sizeof(*p));
    Byte = p->Data;

    /**
     * Default non-zero values
     */
    p->TimeOut = 1U;
    p->Port = LISTENER_PORT;

    while (true) {
        int option_index = 0;

        c = getopt_long(ac, av, "p:b:s:e:n:c:d:r:t:v:", long_options, &option_index);

        if (-1 == c)    // End of parameter list?
            break;

        switch (c) {

        case 0:
            switch (option_index) {

            case 2: // --version
                fprintf(stderr, "cytool: v%s\n", xstr(__CYMUX_VERSION__));
                exit(0);

            default:
                fprintf(stderr, "unhandled option %s", long_options[option_index].name);
                if (optarg) fprintf(stderr, " with arg %s", optarg);
                fprintf(stderr, "\n");
                exit(1);
            }
            break;

        case 'h':       // --help
        case '?':
            usage(0);

        case 's':       // --slave-addr
            Ascii2Hex(optarg, &p->SlaveAddr);
            p->OptFlag |= SLAVE_OPT;
            break;

        case 'n':      // --netfn
            Ascii2Hex(optarg, &p->NetFn);
            p->OptFlag |= NETFN_OPT;
            break;

        case 'c':      //command
            Ascii2Hex(optarg, &p->Command);
            p->OptFlag |= CMD_OPT;
            break;

        case 'd':      //data
            fprintf(stderr, "-d has been deprecated, pass additional bytes as last arguments after flag options\n");
            usage(-1);
            break;

        case 'r':      //retry count
            p->Retry = atoi(optarg);
            break;

        case 'b':      //destination channel for bridged request
            Ascii2Hex(optarg, &p->BridgeChannel);
            p->OptFlag |= CHANNEL_OPT;
            break;

        case 't':      //target address (bridging)
            Ascii2Hex(optarg, &p->TargetAddr);
            p->OptFlag |= BRIDGE_OPT;
            break;

        case 'e':      //timeout in sec
            p->TimeOut = atoi(optarg);
            break;

        case 'v':      //verbose level
            p->Verbosity = atoi(optarg);
            break;

        default:
            fprintf(stderr, "cytool: Error: Unknown arg %c\n", c);
            usage(-1);
        }
    }

    if (optind < ac) {
        /* non-parameter arguments found -- treat as extra data bytes */
        for (; optind < ac; optind++) {
            //get data bytes
            if (p->Length >= sizeof(p->Data)) {
                fprintf(stderr, "cytool: Error: -d byte overrun encountered (too many bytes for IPMB packet)\n");
                usage(-1);
            }
            Ascii2Hex(av[optind], Byte);
            //printf("%02X ", *Byte);
            p->Length++;
            Byte++;
        }
    }

    //All mandatory args should be provided
    if ((p->OptFlag & (SLAVE_OPT | NETFN_OPT | CMD_OPT)) != (SLAVE_OPT | NETFN_OPT | CMD_OPT)) {
        fprintf(stderr, "cytool: Error: Missing command line arguments\n");
        usage(-1);
    }


    if (0 != (p->SlaveAddr % 2)) {
        fprintf(stderr, "cytool: Error: Slave address must be even\n");
        usage(-1);
    }

    if (0 != (p->TargetAddr % 2)) {
        fprintf(stderr, "cytool: Error: Target address must be even\n");
        usage(-1);
    }

    if (0 != (p->NetFn % 2)) {
        fprintf(stderr, "cytool: Error: NetFn request must be even\n");
        usage(-1);
    }

    if (p->TimeOut < 1) {
        fprintf(stderr, "cytool: Error: Timeout must be greater than 0\n");
        usage(-1);
    }
}


/*!
 * Given the parameters passed on the command line, craft the IPMB request to be
 * sent.  The message is automatically converted to a SendMessage if a single
 * bridged request is made.
 *
 * \author amao/bjkurlex
 *
 * \param pReq The buffer to place the IPMB request
 * \param len The length of the above buffer
 * \param p the command line parameters
 *
 * \return size_t The length of the buffer used by the new message
 *
 */
static size_t craft_request(uint8_t* pReq, size_t len, const params_t const* p)
{
    IpmbReq_t*        pRequest   = NULL;
    IpmbBridgedReq_t* pBridgeReq = NULL;
    uint8_t           Cksum      = 0;
    size_t            Length;


    assert(NULL != pReq);
    assert(NULL != p);

    bzero(pReq, len);

    *pReq = p->SlaveAddr;

    pRequest   = (IpmbReq_t*)(pReq + 1);
    pBridgeReq = (IpmbBridgedReq_t*)pRequest->Data;


    pRequest->MasterAddr = I2C_ADDR << 1;  //our default 8-bit address

    //Check for bridging
    if ((p->OptFlag & BRIDGE_OPT)) {

        if (sizeof(p->SlaveAddr) + sizeof(IpmbReq_t) + sizeof(IpmbBridgedReq_t) + p->Length + sizeof(Cksum) >= len) {
            fprintf(stderr, "cytool: Error: -d byte overrun encountered (too many bytes for the bridged IPMB packet)\n");
            exit(-1);
        }

        if (p->Length > 0) {
            // Relocate any data to the bridged data location
            memmove(pBridgeReq->Data, p->Data, p->Length);
        }

        // Update the main pReq packet with the SendMessage request info
        pRequest->NetFn = (NETFN_APP << 2);
        pRequest->Command = CMD_SEND_MSG;

        // Need to Repackage things
        //TODO:  Double check bridge/channel for fields
        pBridgeReq->Channel = (p->OptFlag & CHANNEL_OPT) ? p->BridgeChannel : 0x41;
        pBridgeReq->RespAddr = p->TargetAddr;
        pBridgeReq->NetFn = p->NetFn << 2;

        // Calculate the Addy/NetFn check sum
        pBridgeReq->Checksum = pBridgeReq->RespAddr + pBridgeReq->NetFn;
        pBridgeReq->Checksum ^= 0xFF;
        pBridgeReq->Checksum++;
        //pBridgeReq->Checksum = 0;

        pBridgeReq->ReqAddr = p->SlaveAddr;
        pBridgeReq->RqSeq = GetSeqNum();
        pBridgeReq->Command = p->Command;

        // Compute checksum for Bridged packet (pReqor address, sequence number, cmd, and data bytes, if any
        Cksum = pBridgeReq->ReqAddr + pBridgeReq->RqSeq + pBridgeReq->Command;
        for (size_t Index = 0; Index < p->Length; Index++)
            Cksum += pBridgeReq->Data[Index];

        Cksum ^= 0xFF;
        Cksum++;
        pBridgeReq->Data[p->Length] = Cksum;
        Length = sizeof(IpmbBridgedReq_t) + p->Length + sizeof(Cksum);
    }
    else {
        if (sizeof(p->SlaveAddr) + sizeof(IpmbReq_t) + p->Length + sizeof(Cksum) >= len) {
            fprintf(stderr, "cytool: Error: -d byte overrun encountered (too many bytes for the IPMB packet)\n");
            exit(-1);
        }

        if (p->Length > 0) {
            // Relocate any data to the bridged data location
            memmove(pRequest->Data, p->Data, p->Length);
        }

        pRequest->NetFn = p->NetFn << 2;
        pRequest->Command = p->Command;
        Length = p->Length;
    }


    //Compute the checksum for slave address and net func
    pRequest->Checksum = p->SlaveAddr + pRequest->NetFn;
    pRequest->Checksum ^= 0xFF;
    pRequest->Checksum++;

    //Set the sequence number using /dev/urandom
    pRequest->SeqNum = GetSeqNum();


    //Compute checksum for master address, sequence number, cmd, and data bytes, if any
    Cksum = pRequest->MasterAddr + pRequest->SeqNum + pRequest->Command;
    for (size_t Index = 0; Index < Length; Index++)
        Cksum += pRequest->Data[Index];

    Cksum ^= 0xFF;
    Cksum++;
    pRequest->Data[Length] = Cksum;

    return sizeof(p->SlaveAddr) + sizeof(IpmbReq_t) + Length + sizeof(Cksum);
}


/*!
 * Given the buffer, print out as an IPMB request packet
 *
 * \author amao/bjkurlex
 *
 * \param pReq buffer containing the IPMB request packet
 * \param p command line parameters parsed
 *
 */
static void print_request(const uint8_t const* pReq, const params_t const* p)
{
    IpmbReq_t*        pRequest   = NULL;
    IpmbBridgedReq_t* pBridgeReq = NULL;
    size_t            Length     = 0;


    assert(NULL != pReq);
    assert(NULL != p);

    pRequest = (IpmbReq_t*)(pReq + 1U);
    pBridgeReq = (IpmbBridgedReq_t*)(pRequest->Data);

    if (p->Verbosity >= DEBUG) {
        printf("SlaveAddr:  %02X\n", p->SlaveAddr);
        printf("NetFn/Lun:  %02X (%02X/%02X)\n", pRequest->NetFn, (pRequest->NetFn >> 2), pRequest->NetFn & 3);
        printf("Checksum:   %02X\n", pRequest->Checksum);
        printf("MasterAddr: %02X\n", pRequest->MasterAddr);
        printf("SeqNum:     %02X\n", pRequest->SeqNum);
        printf("Command:    %02X\n", pRequest->Command);
        if ((p->OptFlag & BRIDGE_OPT)) {
            printf("Data:      ");
            printf("\n");
            printf("   (Bridged packet):\n");
            printf("       Channel:   %02X (%d)\n", pBridgeReq->Channel, pBridgeReq->Channel & 3);
            printf("       RespAddr:  %02X\n", pBridgeReq->RespAddr);
            printf("       NetFn/Lun: %02X (%02X/%02X)\n", pBridgeReq->NetFn, (pBridgeReq->NetFn >> 2), pBridgeReq->NetFn & 3);
            printf("       Checksum:  %02X\n", pBridgeReq->Checksum);
            printf("       ReqAddr:   %02X\n", pBridgeReq->ReqAddr);
            printf("       SeqNum:    %02X\n", pBridgeReq->RqSeq);
            printf("       Command:   %02X\n", pBridgeReq->Command);

            if (p->Length > 0) {
                printf("       Data:     ");
                for (size_t Index = 0; Index < p->Length; Index++) {
                    printf(" %02X", pBridgeReq->Data[Index]);
                }
                printf("\n");
            }

            printf("       Checksum:  %02X\n", pBridgeReq->Data[p->Length]);
            Length = sizeof(IpmbBridgedReq_t) + p->Length + sizeof(pBridgeReq->Data[0]);
        }
        else {
            if (p->Length > 0) {
                printf("Data:      ");
                for (size_t Index = 0; Index < p->Length; Index++) {
                    printf(" %02X", pRequest->Data[Index]);
                }
                printf("\n");
            }
            Length = p->Length;
        }
        printf("Checksum:   %02X\n", pRequest->Data[Length]);
    }
}


/*!
 * Given the buffer, print out the corresponding packet as a response packet
 *
 * \author amao/bjkurlex
 *
 * \param pResp buffer containing the IPMB response packet
 * \param len length of the response packet
 * \param p command line parameters parsed
 *
 */
static void print_response(const uint8_t const* pResp, size_t len, const params_t const* p)
{
    IpmbResp_t* Response;


    assert(NULL != pResp);
    assert(NULL != p);

    if (p->Verbosity >= INFO) {
        // Skip the Slave Address
        Response = (IpmbResp_t*)(pResp + 1U);
        len--;

        if (len >= sizeof(IpmbResp_t)) {
            if (p->Verbosity >= DEBUG) {
                printf("NetFn/Lun: %02X (%02X/%02X)\n", Response->NetFn, (Response->NetFn >> 2), Response->NetFn & 3);
                printf("Checksum:  %02X\n", Response->Checksum);
                printf("SlaveAddr: %02X\n", Response->SlaveAddr);
                printf("SeqNum:    %02X\n", Response->SeqNum);
                printf("Command:   %02X\n", Response->Command);
            }
            printf("CC:        %02X\n", Response->CompCode);

            if (len > sizeof(IpmbResp_t) + sizeof(uint8_t)) {
                printf("Data:      ");

                if ((p->OptFlag & BRIDGE_OPT) && 0 == Response->CompCode && (len - sizeof(IpmbResp_t)) >= (sizeof(IpmbResp_t))) {

                    // Point to Bridged response
                    IpmbResp_t* BridgeResp = (IpmbResp_t*)(Response->Data + 1);
                    size_t      DataLength = len - sizeof(IpmbResp_t)   // Main response header
                                                 - sizeof(uint8_t)      // Bridged req addr
                                                 - sizeof(IpmbResp_t)   // Bridged response
                                                 - sizeof(uint8_t);     // Final checksum

                    printf("\n");
                    printf("   (Bridged packet):\n");
                    printf("       ReqAddr:   %02X\n", Response->Data[0]);
                    printf("       NetFn/Lun: %02X (%02X/%02X)\n", BridgeResp->NetFn, (BridgeResp->NetFn >> 2), BridgeResp->NetFn & 3);
                    printf("       Checksum:  %02X\n", BridgeResp->Checksum);
                    printf("       SlaveAddr: %02X\n", BridgeResp->SlaveAddr);
                    printf("       SeqNum:    %02X\n", BridgeResp->SeqNum);
                    printf("       Command:   %02X\n", BridgeResp->Command);
                    printf("       CompCode:  %02X\n", BridgeResp->CompCode);

                    if (DataLength > 0) {
                        printf("       Data:      ");
                        for (size_t Index = 0; Index < DataLength; Index++)
                            printf("%02X ", BridgeResp->Data[Index]);
                        printf("\n");
                    }
                }
                else {
                    // Non-bridged data
                    for (size_t Index = 0; Index < (len - sizeof(IpmbResp_t) - sizeof(uint8_t)); Index++)
                        printf("%02X ", Response->Data[Index]);
                    printf("\n");
                }

                if (p->Verbosity >= DEBUG) printf("Checksum:  %02X\n", Response->Data[len - sizeof(IpmbResp_t) - sizeof(uint8_t)]);
            }
            else {
                if (p->Verbosity >= DEBUG) printf("Checksum:  %02X\n", Response->Data[0]);
            }
        }
    }
}


/*!
 * Dump buffer in hex by 16 bytes with leading annotation
 *
 * \author bjkurlex (11/25/15)
 *
 * \param prefix annotation to lead dump with
 * \param buf buffer to dump
 * \param len length of the buffer to dump
 * \param p command line parameters parsed
 *
 */
static void print_buf(const char const* prefix, const uint8_t const* buf, size_t len, const params_t const* p)
{
    assert(NULL != prefix);
    assert(NULL != buf);
    assert(len > 0);

    if (p->Verbosity >= PKT_DUMP) {
        printf("%s (%ld bytes):", prefix, len);
        for (size_t Index = 0; Index < len; Index++) {
            if (0 == (Index % 16)) printf("\n%02lx:", Index);
            printf(" %02X", buf[Index]);
        }
        printf("\n");
    }
}


/*!
 * Connect to and negotiate with the server as a client
 * Will close/exit on any failure (including negotiation failure)
 *
 * \author bjkurlex (11/25/15)
 *
 * \param p command line parameters
 *
 * \return int socket on successful connection/negotiation
 *
 */
static int attach2MUX(params_t* p)
{
    int                sflags;
    int                port;
    ssize_t            sent;
    ssize_t            received;
    fd_set             rfds;
    struct timeval     tv;
    int                retval;
    struct sockaddr_in server;         // listener/registration defintion for socket
    uint8_t            buf[MAX_IPMB_PKT_SIZE+1]; // receive buffer
    int                sock     = socket(AF_INET, SOCK_STREAM, 0);

    if (-1 == sock) {   // Klockwork wants to see -1 check, first or it throws an error
        fprintf(stderr, "Unable to connect to server: %s\n", strerror(errno));
        exit(1);
    }

    assert(NULL != p);

    if (p->Verbosity >= PROGRESS)
        printf("Connecting to server\n");

    port = p->Port > 0 ? p->Port : LISTENER_PORT;


    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    // Only services on the localhost can connect to the MUX
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    /**
     * Attempt connection to the server
     */
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        fprintf(stderr, "Unable to bind to server port %d (is cyMUX running?): %s\n",
                port, strerror(errno));
        close(sock);
        exit(1);
    }

    /**
     * Reconfigure the connection as non-blocking
     */
    sflags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, sflags | O_NONBLOCK);

    if (p->Verbosity >= PROGRESS)
        printf("Negotiating with server\n");

    /**
     * Perform negotiation with the server requesting to be client
     */
    sent = send(sock, "C", 1, 0);
    if (sent < 0) {
        fprintf(stderr, "Negotiation send failure: %s\n", strerror(errno));
        close(sock);
        exit(1);
    }
    bzero(buf, sizeof(buf));

    tv.tv_sec = p->TimeOut;
    tv.tv_usec = 0;

    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);

    retval = select(sock + 1, &rfds, NULL, NULL, &tv);
    if (retval < 0) {
        fprintf(stderr, "Negotiation failure (waiting on select to read): %s\n", strerror(errno));
        close(sock);
        exit(1);
    }
    if (0 == retval) {
        fprintf(stderr, "Negotiation failure (timed out on read)\n");
        close(sock);
        exit(1);
    }

    assert(FD_ISSET(sock, &rfds));

    /**
     * Look for the server's response to the negotiation request
     */
    received = recv(sock, buf, sizeof(buf), 0);
    if (0 == received) {
        fprintf(stderr, "Server closed connection during negotiation\n");
        close(sock);
        exit(1);
    }
    else if (received < 0) {
        fprintf(stderr, "Negotiation recv failure: %s\n", strerror(errno));
        close(sock);
        exit(1);
    }

    if (0 != memcmp(buf, "OK", 2)) {
        if (0 == memcmp(buf, "NOK", 3)) {
            fprintf(stderr, "Server rejected our negotiation request as a client\n");
        }
        else {
            fprintf(stderr, "Server provided unexpected results during negotiation request:");
            for (ssize_t Index = 0; Index < received; Index++) {
                if (0 == (Index % 16))
                    fprintf(stderr, "\n%02lx:", Index);
                fprintf(stderr, " %02x", buf[Index]);
            }
            fprintf(stderr, "\n");
        }
        close(sock);
        exit(1);
    }
    if (p->Verbosity >= PROGRESS) printf("Negotiating successful\n");

    /**
     * Successful negotiation, return the open socket
     */
    return sock;
}
