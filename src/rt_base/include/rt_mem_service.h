#ifndef __RT_MEM_SERVICE_H__
#define __RT_MEM_SERVICE_H__

#include "rt_header.h"

typedef struct _mem_node {
    INT32       size;
    void        *ptr;
    const char  *caller;
} MemNode;

class rt_mem_service {
public:
    rt_mem_service();
    ~rt_mem_service();
    void add_node(const char *caller, void* ptr, UINT32 size);
    void remove_node(void* ptr, UINT32 *size);
    void reset();
    void dump();
    INT32 find_node(const char *caller, void* ptr, UINT32 *size);
public:
    MemNode *mem_nodes;
    UINT32  nodes_max;
    UINT32  nodes_cnt;
    UINT32  total_size;
};

#endif // __RT_MEM_SERVICE_H__
