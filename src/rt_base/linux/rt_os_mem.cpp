#ifdef OS_LINUX
#include <stdlib.h>
#include "../rt_os_mem.h"

int rt_os_malloc(void **memptr, size_t alignment, size_t size)
{
    return posix_memalign(memptr, alignment, size);
}

int rt_os_realloc(void *src, void **dst, size_t alignment, size_t size)
{
    (void)alignment;
    *dst = realloc(src, size);
    return (*dst) ? (0) : (-1);
}

void rt_os_free(void *ptr)
{
    free(ptr);
}

#endif
