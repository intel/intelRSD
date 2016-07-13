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


#include "mux_usage.h"
#include "debug.h"
#include "stringify.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>


int gTimeout = 2;                   // Send/Receive timeout to use
int gSendDelay = 0;                 // Default to best (sunny day) case


/*!
 * Usage of the program.   Displays the valid arguments that may passed to the
 * program.
 *
 * \author bjkurlex (11/2/15)
 *
 * \param rc Program exit status to use
 */
void usage(int rc)
{
    printf("Usage: cyMUX [OPTION]...\n\n");
    printf("Options:\n");
    printf("   -t, --timeout=SECONDS  receive/send timeout (expiry) value (SECONDS).\n");
    printf("   -v, --verbosity=LEVEL  verbosity/debug LEVEL\n");
    printf("   -f, --foreground       stay in the foreground.\n");
    printf("   -h, --help             this usage\n");
    printf("   -w, --delay=MS         How long to delay in milliseconds after i2c write\n");
    printf("   --version              Version string for cyMUX\n");
    printf("\n");
    exit(rc);
}


/*!
 * Process the command line arguments.  Sets up the environment that the daemon
 * will use.   Will dispatch the usage if requested.
 *
 * \author bjkurlex (11/2/15)
 *
 * \param ac Number of arguments passed to program
 * \param av Array containing the arguments passed to the program
 * \return boolean whether to daemonize(false) or leave in the foreground(true)
 *
 */
bool process_commandline(int ac, char *av[])
{
    int     c;                      // Current command line option to process
    bool    foreground = false;     // Whether to daemonize or not
    static struct option long_options[] = {
        // Name         has_arg             *flag   val
        { "help",       no_argument,        NULL,   'h' },    // 0
        { "verbosity",  required_argument,  NULL,   'v' },    // 1
        { "foreground", no_argument,        NULL,   'f' },    // 2
        { "timeout",    required_argument,  NULL,   't' },    // 3
        { "delay",      required_argument,  NULL,   'w' },    // 4
        { "wait",       required_argument,  NULL,   'w' },    // 5
        { "version",    no_argument,        NULL,   0   },    // 6
        { NULL,         no_argument,        NULL,   0   }     // Termination entry
    };

    assert(NULL != av);

    while (true) {
        int option_index = 0;

        c = getopt_long(ac, av, "w:t:v:h?f", long_options, &option_index);

        if (-1 == c)    // End of parameter list?
            break;

        switch (c) {

        case 0:
            switch (option_index) {

            case 6: // --version
                fprintf(stderr, "cyMUX: v%s\n", xstr(__CYMUX_VERSION__));
                exit(0);

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
            gTimeout = atoi(optarg);
            if (gTimeout < 1) {
                fprintf(stderr, "cyMUX: Error: Timeout must be greater than 0\n");
            }
            break;

        case 'f':
            foreground = true;
            break;

        case 'v':   // --verbosity
            gVerbosity = atoi(optarg);
            break;

        case 'w':   // --delay | --wait
            gSendDelay = atoi(optarg);
            break;

        default:
            fprintf(stderr, "\n");
            usage(-1);
        }
    }

    if (optind < ac) {
        /* non-parameter arguments found */
        fprintf(stderr, "cyMUX: Extra unknown arguments ");
        for ( ; optind < ac; optind++)
            fprintf(stderr, "\"%s\" ", av[optind]);
        fprintf(stderr, "\n\n");
        usage(-1);
    }

    debug(LOG_DEBUG, "verbosity=%d\n", gVerbosity);
    debug(LOG_DEBUG, "foreground=%s\n", (foreground ? "true" : "false"));
    debug(LOG_DEBUG, "timeout=%d\n", gTimeout);
    debug(LOG_DEBUG, "delay=%d\n", gSendDelay);

    return foreground;
}
