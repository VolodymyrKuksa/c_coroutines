#include "context.h"

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct
{
    Context caller;
    Context callee;
    uint8_t* callee_stack;
    bool finished;
} Coroutine;

// TODO: Make this thread local
static volatile Coroutine* g_thread_co = NULL;


Coroutine co_make(void (*callee)())
{
    Coroutine co;
    bzero(&co, sizeof(co));
    co.finished = false;

    const uint32_t mem_size = 4096;
    co.callee_stack = (uint8_t*)malloc(sizeof(uint8_t) * mem_size);
    assert(co.callee_stack != NULL);

    uint8_t* sp = co.callee_stack + mem_size; // Point to the top as the stack grows downwards
    sp = (uint8_t*)((uintptr_t)sp & ~ (uintptr_t)0x0F); // Allign to 16 bytes
    sp -= 128; // Leave room for the red zone

    sp -= 8; // When calling a function it's return adress is pushed onto the stack
             // As we never called into foo we have to offset the stack by the size of the return adress

    // Setup the context
    co.callee.rsp = (void*)sp; // Set the stack
    co.callee.rip = (void*)callee; // Set the instruction

    return co;
}

void co_release(Coroutine* co)
{
    free(co->callee_stack);
    bzero(co, sizeof(*co));
}

bool co_continue(Coroutine* co)
{
    assert(!co->finished);
    volatile uint32_t guard = 0;
    volatile Coroutine* cache = g_thread_co;
    get_context(&co->caller);
    if (guard++ == 0)
    {
        g_thread_co = co;
        // This set context never retuns
        // We'll jump back to get_context above
        set_context(&co->callee);
    }
    g_thread_co = cache;
    return co->finished;
}

void co_return()
{
    g_thread_co->finished = true;
    set_context(&g_thread_co->caller);
}

void co_yield()
{
    volatile uint32_t guard = 0;
    get_context(&g_thread_co->callee);
    if (guard++ == 0)
    {
        // This set context never retuns
        // We'll jump back to get_context above
        set_context(&g_thread_co->caller);
    }
}


void foo()
{
    printf("Hello from foo!\n");
    co_return();
}

void count_to_five()
{
    uint32_t i = 1;
    while (i <=5)
    {
        printf("Nested coroutine: %d/5\n", i++);
        co_yield();
    }
    co_return();
}

void counter()
{
    float i = 0.f;
    while(1)
    {
        Coroutine nested = co_make(count_to_five);
        while (!co_continue(&nested)) {}
        co_release(&nested);

        printf("counter: %f\n", i);
        i += 0.5f;
        co_yield();
    }
    // never returns
}


int main()
{
    { // Call a simple function as a coroutine
        Coroutine co = co_make(foo);
        co_continue(&co);
        co_release(&co);
    }

    { // Infinite loop as coroutine + nested coroutines
        Coroutine counter_co = co_make(counter);
        uint32_t c = 0;
        while (c++ < 3)
        {
            co_continue(&counter_co);
        }
        co_release(&counter_co);
    }

    { // The code below prints "Hello From Main Context" twice
        volatile uint32_t hit_count = 0;
        Context main_context;
        get_context(&main_context);
        printf("Hello From Main Context\n");
        while (hit_count++ < 1)
        {
            set_context(&main_context);
        }
    }

    return 0;
}
