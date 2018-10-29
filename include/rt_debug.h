

#ifndef __RT_DEBUG_H__
#define __RT_DEBUG_H__

#include <assert.h>

#ifdef RT_DEBUG
    #define RtASSERT(cond)            static_cast<void>(0)
    #define RtASSERTF(cond, fmt, ...) static_cast<void>(0)
    #define RtDEBUGFAIL(messge)
    #define RtAssertResult(cond)      if (cond) {} do {} while(false)
#else
    #define RT_ASSERT(cond)            static_cast<void>(0)
    #define RT_ASSERT_IF(cond, fmt, ...) static_cast<void>(0)
    #define RT_DEBUGFAIL(message)
    #define RT_ASSERT_RESULT(cond)      if (cond) {} do {} while(false)
#endif

#endif
