//
// Created by SuNNjek on 10.07.20.
//

#pragma once

#include <cstdlib>
#include <gc.h>

class AllocatePolicyStdNew
{
public:
    static void* allocate(size_t size) { return malloc(size); }
    static void mfree(void *t) { free(t); }

protected:
    ~AllocatePolicyStdNew() { } // to prohibit destruction by client
};

class AllocatePolicyStdGC
{
public:
    static void* allocate(size_t size) { return GC_MALLOC_ATOMIC(size); }
    static void mfree(void *t) { }
protected:
    ~AllocatePolicyStdGC() { } // to prohibit destruction by client
};

