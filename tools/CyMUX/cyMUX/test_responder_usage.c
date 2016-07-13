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
 * \file test_responder_usage.c
 *
 * \brief Handle command line parameters and displaying usage of the same
 *
 */


#include "test_responder_usage.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <assert.h>


/*!
 * Usage of the program.   Displays the valid arguments that may passed to the
 * program.
 *
 * \author bjkurlex (11/13/15)
 *
 * \param retval Program exit status to use
 *
 */
void usage(int retval)
{
    printf("Usage: test_responder [OPTION]...\n\n");
    printf("Options:\n");
    printf("   -h, --help             this usage\n");
    printf("   -v, --verbosity=LEVEL  verbosity/debug LEVEL\n");
    printf("\n");
    exit(retval);
}


/*!
 * Process the command line arguments.  Sets up the environment that the daemon
 * will use.   Will dispatch the usage if requested.
 *
 * \author bjkurlex (11/13/15)
 *
 * \param ac Number of arguments passed to program
 * \param av Array containing the arguments passed to the program
 *
 * \return void
 *
 */
void process_commandline(int ac, char* av[])
{
    int                  c;         // Current command line option to process
    static struct option long_options[] = {
        // Name         has_arg             *flag   val
        { "help",       no_argument,        NULL,   'h' },
        { "verbosity",  required_argument,  NULL,   'v' },    // 1
        { NULL,         no_argument,        NULL,   0   }   // Termination entry  // 0
    };

    assert(NULL != av);

    while (true) {
        int option_index = 0;

        c = getopt_long(ac, av, "v:h?", long_options, &option_index);

        if (-1 == c)    // End of parameter list?
            break;

        switch (c) {

        case 0:
            switch (option_index) {
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

        case 'v':   // --verbosity
            gVerbosity = atoi(optarg);
            break;

        default:
            fprintf(stderr, "\n");
            usage(-1);
        }
    }

    if (optind < ac) {
        /* non-parameter arguments found */
        fprintf(stderr, "test_responder: Extra unknown arguments ");
        for (; optind < ac; optind++) fprintf(stderr, "\"%s\" ", av[optind]);
        fprintf(stderr, "\n\n");
        usage(-1);
    }
}
