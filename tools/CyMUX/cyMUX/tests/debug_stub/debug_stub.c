#include "tap/basic.h"
#include "debug.h"
#include <stdarg.h>
#include <stdio.h>


void message_dump(const char* __restrict fmt, Message_t const* const m, ...)
{
    size_t  i;
    va_list ap;


    if (NULL == fmt) bail("message_dump fmt == NULL");
    if (NULL == m) bail("message_dump m == NULL");

    printf("# ");
    va_start(ap, m);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
    printf("# Resource: %d\n", m->client_resource);
    printf("# DstAddr: %02x\n", m->DstAddr);
    printf("# Length: %lu", m->Length);
    for (i = 0; i < m->Length; i++) {
        if (0 == (i % 16)) printf("\n# %04ld:", i);
        printf(" %02x", m->Data[i]);
    }
    printf("\n");
}


void buffer_dump(const char* __restrict fmt, uint8_t const* const b, ssize_t l, ...)
{
    va_list ap;

    if (NULL == fmt) bail("buffer_dump fmt == NULL");
    if (NULL == b) bail("buffer_dump b == NULL");

    printf("# ");
    va_start(ap, l);
    vprintf(fmt, ap);
    va_end(ap);
    for (ssize_t i = 0; i < l; i++) {
        if (0 == (i % 16)) printf("\n# %04ld:", i);
        printf(" %02x", b[i]);
    }
    printf("\n");
}
//void debug(int  __attribute__((__unused__))verbosity, const char  __attribute__((__unused__))* __restrict fmt, ...)
void debug(int __attribute__((__unused__))verbosity, const char* __restrict fmt, ...)
{
    va_list args;

    fflush(stderr);
    printf("# ");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
void init_diag(void)
{
}
void cleanup_diag(void)
{
}
