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
 * \file usage.c
 *
 * \brief Handle command line parameters and displaying usage of the same
 *
 */


#include "test_client_usage.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdarg.h>


int     gPlanned = 0;
bool    gHang_connection = false;
int     gConnection_holdtime = 0;
bool    gServer = false;
bool    gNo_server = false;
bool    gClose_prematurely = false;
char    gClient_type[MAX_IPMB_PKT_SIZE*2];
size_t  gClient_type_len = 1;


static int sgPlanned = 0;
static int sgFailed = 0;
static int sgPassed = 0;
static int sgCount = 1;



/*!
 * Usage of the program.   Displays the valid arguments that may passed to the
 * program.
 *
 * \author bjkurlex (11/4/15)
 *
 * \param retval Program exit status to use
 */
void usage(int retval)
{
    printf("Usage: test_client [OPTION]...\n\n");
    printf("Options:\n");
    printf("   -h, --help             this usage\n");
    printf("   -t, --test TEST        Predefined test to execute\n");
    printf("   --connect TIME         How long to wait after making a connection\n");
    printf("                          before continuing test\n");
    printf("   --type NEGTYPE         Negotiate type (defaults to C)\n");
    printf("   --neg_len LENGTH       Creates a random packet to LENGTH for the\n");
    printf("                          Negotiation submitted\n");
    printf("\nTESTS:\n");
    printf("   HANGCONN       Open connection and do nothing for --connect TIME\n");
    printf("   SERVER         Attempted connection to server succeeds\n");
    printf("   NOSERVER       Attempted connection to server fails\n");
    printf("   PREMATURE      Attempt connection, then close\n");
    printf("   CLIENT         Connect in client mode            --type C\n");
    printf("   RESPONDER      Connect in responder mode         --type R\n");
    printf("   BADNEGOTIATION Connect with garbage/illegal mode --type <x>\n");
    printf("\n");
    exit(retval);
}


/*!
 * Process the command line arguments.  Sets up the environment that the daemon
 * will use.   Will dispatch the usage if requested.
 *
 * \author bjkurlex (11/4/15)
 *
 * \param ac Number of arguments passed to program
 * \param av Array containing the arguments passed to the program
 * \return void
 *
 */
void process_commandline(int ac, char *av[])
{
    int     c;                      // Current command line option to process
    static struct option long_options[] = {
        // Name         has_arg             *flag   val
        { "help",       no_argument,        NULL,   'h' },  // 0
        { "test",       required_argument,  NULL,   't' },  // 1
        { "connect",    required_argument,  NULL,   0   },  // 2
        { "plan",       required_argument,  NULL,   'p' },  // 3
        { "plan_lazy",  no_argument,        NULL,   'l' },  // 4
        { "type",       required_argument,  NULL,   0   },  // 5
        { "neg_len",    required_argument,  NULL,   0   },  // 6
        { NULL,         no_argument,        NULL,   0   }   // Termination entry
    };

    assert(NULL != av);

    while (true) {
        int option_index = 0;

        c = getopt_long(ac, av, "t:c:p:lh?", long_options, &option_index);

        if (-1 == c)    // End of parameter list?
            break;

        switch (c) {

        case 0:
            switch (option_index) {
            case 2:
                gConnection_holdtime = atoi(optarg);
                break;

            case 5:
                *gClient_type = *optarg;
                break;

            case 6:
                gClient_type_len = atol(optarg);
                if (gClient_type_len > sizeof(gClient_type)) {
                    fprintf(stderr, "%s can not exceed compiled storage limitaion of %ld bytes\n",
                            long_options[option_index].name, sizeof(gClient_type));
                    exit(1);
                }
                {
                    for (size_t i = 1; i < gClient_type_len; i++)
                        gClient_type[i] = (char)rand();
                }
                break;

            default:
                fprintf(stderr, "unhandled option %s", long_options[option_index].name);
                if (optarg) fprintf(stderr, " with arg %s", optarg);
                fprintf(stderr, "\n");
                exit(1);
            }
            break;

        case 'h':
        case '?':
            usage(0);

        case 't':
            if (0 == strcmp(optarg, "HANGCONN"))
                gHang_connection = true;
            else if (0 == strcmp(optarg, "SERVER"))
                gServer = true;
            else if (0 == strcmp(optarg, "NOSERVER"))
                gNo_server = true;
            else if (0 == strcmp(optarg, "PREMATURE"))
                gClose_prematurely = true;
            else if (0 == strcmp(optarg, "CLIENT"))
                *gClient_type = 'C';
            else if (0 == strcmp(optarg, "RESPONDER"))
                *gClient_type = 'R';
            else if (0 == strcmp(optarg, "BADNEGOTIATION"))
                *gClient_type = (char)249;
            else {
                fprintf(stderr, "Unknown or unimplimented test %s\n\n", optarg);
                usage(1);
            }
            break;

        case 'p':
            gPlanned = atoi(optarg);
            break;

        case 'l':
            plan_lazy();
            break;

        default:
            fprintf(stderr, "\n");
            usage(-1);
        }
    }

    if (optind < ac) {
        /* non-parameter arguments found */
        fprintf(stderr, "test_client: Extra unknown arguments ");
        for ( ; optind < ac; optind++)
            fprintf(stderr, "\"%s\" ", av[optind]);
        fprintf(stderr, "\n\n");
        usage(-1);
    }

    diag("Tests to be performed:");
    if (gServer) diag("SERVER");
    if (gNo_server) diag("NOSERVER");
    if (gHang_connection) diag("HANGCONN (%ds)", gConnection_holdtime);
    if (gClose_prematurely) diag("PREMATURE");
    switch (*gClient_type) {
    case 0:
        *gClient_type = 'C';
    case 'C':
    case 'c':
        diag("CLIENT");
        break;

    case 'r':
    case 'R':
        diag("RESPONDER");
        break;

    default:
        diag("BADNEGOTIATION");
        break;
    }
    diag("Negotiation packet length: %d", gClient_type_len);

}


int diag(const char* fmt, ...)
{
    char    newfmt[160];    // two lines, but no more for now
    char    output[160];    // Hush Klockwork
    va_list ap;
    va_start(ap, fmt);

    assert(NULL != fmt);

    snprintf(newfmt,sizeof(newfmt)-1,"# %s\n", fmt);
    vsnprintf(output, sizeof(output)-1, newfmt, ap);    // Hush Klockwork
    return printf("%s", output);
}


bool pass(const char* desc)
{
    assert(NULL != desc);

    printf("ok %d - %s\n", sgCount, desc);
    sgCount++;
    sgPassed++;
    return true;
}

bool fail(const char* desc)
{
    assert(NULL != desc);

    printf("not ok %d - %s\n", sgCount, desc);
    sgCount++;
    sgFailed++;
    return true;
}


void plan(int planned)
{
    sgPlanned = planned;
    sgCount = 1;
    sgPassed = 0;
    sgFailed = 0;

    printf("1..%d\n", planned);
}

void plan_lazy(void)
{
    sgPlanned = 0;
    sgCount = 1;
    sgPassed = 0;
    sgFailed = 0;
}
