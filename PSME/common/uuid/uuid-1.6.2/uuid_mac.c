/*!
 * @brief OSSP UUID Media Access Control (MAC) resolver implementation
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file uuid_mac.c
 */

/*
**  OSSP uuid - Universally Unique Identifier
**  Copyright (c) 2004-2008 Ralf S. Engelschall <rse@engelschall.com>
**  Copyright (c) 2004-2008 The OSSP Project <http://www.ossp.org/>
**
**  This file is part of OSSP uuid, a library for the generation
**  of UUIDs which can found at http://www.ossp.org/pkg/lib/uuid/
**
**  Permission to use, copy, modify, and distribute this software for
**  any purpose with or without fee is hereby granted, provided that
**  the above copyright notice and this permission notice appear in all
**  copies.
**
**  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
**  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
**  IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
**  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
**  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
**  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
**  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
**  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
**  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
**  SUCH DAMAGE.
**
**  uuid_mac.c: Media Access Control (MAC) resolver implementation
*/

/* own headers (part (1/2) */
#include "uuid_ac.h"

/* system headers */
#include <safe-string/safe_mem_lib.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif
#ifdef HAVE_NET_IF_H

#ifdef __clang__
#define __USE_MISC
#endif
#include <net/if.h>
#ifdef __clang__
#undef __USE_MISC
#endif

#endif
#ifdef HAVE_NET_IF_DL_H
#include <net/if_dl.h>
#endif

/* own headers (part (1/2) */
#include "uuid_mac.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (/*lint -save -e506*/ (!FALSE) /*lint -restore*/)
#endif

/* return the Media Access Control (MAC) address of
   the FIRST network interface card (NIC) */
int mac_address(unsigned char *data_ptr, size_t data_len)
{
    /* sanity check arguments */
    if (data_ptr == NULL || data_len < MAC_LEN)
        return FALSE;

#if defined(HAVE_IFADDRS_H) && defined(HAVE_NET_IF_DL_H) && defined(HAVE_GETIFADDRS)
    /* use getifaddrs(3) on BSD class platforms (xxxBSD, MacOS X, etc) */
    {
        struct ifaddrs *ifap;
        struct ifaddrs *ifap_head;
        const struct sockaddr_dl *sdl;
        unsigned char *ucp;
        int i;

        if (getifaddrs(&ifap_head) < 0)
            return FALSE;
        for (ifap = ifap_head; ifap != NULL; ifap = ifap->ifa_next) {
            if (ifap->ifa_addr != NULL && ifap->ifa_addr->sa_family == AF_LINK) {
                sdl = (const struct sockaddr_dl *)(void *)ifap->ifa_addr;
                ucp = (unsigned char *)(sdl->sdl_data + sdl->sdl_nlen);
                if (sdl->sdl_alen > 0) {
                    for (i = 0; i < MAC_LEN && i < sdl->sdl_alen; i++, ucp++)
                        data_ptr[i] = (unsigned char)(*ucp & 0xff);
                    freeifaddrs(ifap_head);
                    return TRUE;
                }
            }
        }
        freeifaddrs(ifap_head);
    }
#endif

#if defined(HAVE_NET_IF_H) && defined(SIOCGIFHWADDR)
    /* use SIOCGIFHWADDR ioctl(2) on Linux class platforms */
    {
        struct ifreq ifr;
        struct sockaddr *sa;
        int s;
        int i;

        if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
            return FALSE;
        strcpy_s(ifr.ifr_name, IFNAMSIZ, "eth0");
        if (ioctl(s, SIOCGIFHWADDR, &ifr) < 0) {
            close(s);
            return FALSE;
        }
        sa = (struct sockaddr *)&ifr.ifr_addr;
        for (i = 0; i < MAC_LEN; i++)
            data_ptr[i] = (unsigned char)(sa->sa_data[i]);
        close(s);
        return TRUE;
    }
#endif

    return FALSE;
}
