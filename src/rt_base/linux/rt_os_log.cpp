#ifdef OS_LINUX
#include <stdio.h>
#include <stdarg.h>
#include "../rt_os_log.h"

#define LINE_SZ 1024

void rt_os_log(const char* tag, const char* msg, va_list list)
{
    char line[LINE_SZ] = {0};
    snprintf(line, sizeof(line), "%s: %s", tag, msg);
    vfprintf(stdout, line, list);
}

void rt_os_err(const char* tag, const char* msg, va_list list)
{
    char line[LINE_SZ] = {0};
    snprintf(line, sizeof(line), "%s: %s", tag, msg);
    vfprintf(stderr, line, list);
}

#endif
