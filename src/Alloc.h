//
// Created by SuNNjek on 10.07.20.
//

#pragma once

#include <cstdlib>
#if !WIN32
    #include <gc.h>
#endif

class AllocatePolicyStdNew
{
public:
    static void* allocate(size_t size) { return malloc(size); }
    static void mfree(void *t) { free(t); }

protected:
    ~AllocatePolicyStdNew() { } // to prohibit destruction by client
};

#if !WIN32 // Boehm GC doesn't exist on Windows appearently but it doesn't get used so it doesn't matter anyway
class AllocatePolicyStdGC
{
public:
    static void* allocate(size_t size) { return GC_MALLOC_ATOMIC(size); }
    static void mfree(void *t) { }
protected:
    ~AllocatePolicyStdGC() { } // to prohibit destruction by client
};
#endif
