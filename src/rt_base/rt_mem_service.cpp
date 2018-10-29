#include "rt_mem_service.h"
#include "rt_os_mem.h"
#include "rt_mem.h"
#include "rt_log.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "rt_mem_service"

#define MEM_NODE_MAX            (1024)

rt_mem_service::rt_mem_service() {
    nodes_max = MEM_NODE_MAX;
    INT32 size = sizeof(MemNode)*nodes_max;
    rt_os_malloc((void **)&mem_nodes, MEM_ALIGN, size);
    RT_ASSERT(RT_NULL != mem_nodes);

    rt_memset(mem_nodes, 0, sizeof(MemNode)*nodes_max);
    add_node(__FUNCTION__, mem_nodes, size);

    add_node(__FUNCTION__, this, sizeof(rt_mem_service));
}

rt_mem_service::~rt_mem_service(){
}

void rt_mem_service::add_node(const char *caller, void* ptr, UINT32 size) {
    RT_ASSERT(RT_NULL != mem_nodes);

    MemNode *node = mem_nodes;
    for (UINT32 i = 0; i < nodes_max; i++, node++) {
        if(node->size == 0) {
            node->caller = caller;
            node->ptr    = ptr;
            node->size   = size;
            nodes_cnt++;
            total_size += size;
            break;
        }
    }
}

void rt_mem_service::remove_node(void* ptr, size_t *size) {
    RT_ASSERT(RT_NULL != mem_nodes);

    MemNode *node = mem_nodes;
    for (UINT32 i = 0; i < nodes_max; i++, node++) {
        if((node->size > 0)&&(node->ptr == ptr)) {
            *size = node->size;
            nodes_cnt--;
            total_size -= node->size;
            node->size = 0;
            break ;
        }
    }
}

void rt_mem_service::reset(){
    RT_ASSERT(RT_NULL != mem_nodes);

    MemNode *node = mem_nodes;
    for (UINT32 i = 0; i < nodes_max; i++, node++) {
       node->size   = 0;
       node->ptr    = RT_NULL;
       node->caller = RT_NULL;
    }
    nodes_cnt  = 0;
    total_size = 0;
}

void rt_mem_service::dump(){
    RT_ASSERT(RT_NULL != mem_nodes);

    RT_LOGE("======= Rockit Memory Summary =======");
    MemNode *node = mem_nodes;
    RT_LOGE("Memory Tatal:%dk, nodes:%d/%d", total_size/1024, nodes_cnt, nodes_max);
    for (UINT32 i = 0; i < nodes_max; i++, node++) {
        if( node->size > 0) {
            RT_LOGE("Memory Node:Ptr:%p; Size=%04d; caller=%s",
                     node->ptr, node->size, node->caller);
        }
    }
}
