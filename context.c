#include "context.h"

// References:
// https://graphitemaster.github.io/fibers
// https://github.com/hnes/libaco

asm(".text\n\t"
    ".global _get_context\n\t"
    "_get_context:\n\t"

    "movq  (%rsp), %rdx\n\t" // RIP
    "leaq 8(%rsp), %rcx\n\t" // RSP
    "movq %r12, 8*0(%rdi)\n\t"
    "movq %r13, 8*1(%rdi)\n\t"
    "movq %r14, 8*2(%rdi)\n\t"
    "movq %r15, 8*3(%rdi)\n\t"
    "movq %rdx, 8*4(%rdi)\n\t" // RIP
    "movq %rcx, 8*5(%rdi)\n\t" // RSP
    "movq %rbx, 8*6(%rdi)\n\t"
    "movq %rbp, 8*7(%rdi)\n\t"

    "ret\n\t");

asm(".text\n\t"
    ".global _set_context\n\t"
    "_set_context:\n\t"

    "movq 8*0(%rdi), %r12\n\t"
    "movq 8*1(%rdi), %r13\n\t"
    "movq 8*2(%rdi), %r14\n\t"
    "movq 8*3(%rdi), %r15\n\t"
    "movq 8*4(%rdi), %rax\n\t" // RIP
    "movq 8*5(%rdi), %rcx\n\t" // RSP
    "movq 8*6(%rdi), %rbx\n\t"
    "movq 8*7(%rdi), %rbp\n\t"

    "movq %rcx, %rsp\n\t"
    "pushq %rax\n\t"
    "ret\n\t");

asm(".text\n\t"
    ".global _swap_context\n\t"
    "_swap_context:\n\t"

    // Save current context
    "movq  (%rsp), %rdx\n\t" // RIP
    "leaq 8(%rsp), %rcx\n\t" // RSP
    "movq %r12, 8*0(%rdi)\n\t"
    "movq %r13, 8*1(%rdi)\n\t"
    "movq %r14, 8*2(%rdi)\n\t"
    "movq %r15, 8*3(%rdi)\n\t"
    "movq %rdx, 8*4(%rdi)\n\t" // RIP
    "movq %rcx, 8*5(%rdi)\n\t" // RSP
    "movq %rbx, 8*6(%rdi)\n\t"
    "movq %rbp, 8*7(%rdi)\n\t"

    // Set the new context
    "movq 8*0(%rsi), %r12\n\t"
    "movq 8*1(%rsi), %r13\n\t"
    "movq 8*2(%rsi), %r14\n\t"
    "movq 8*3(%rsi), %r15\n\t"
    "movq 8*4(%rsi), %rax\n\t" // RIP
    "movq 8*5(%rsi), %rcx\n\t" // RSP
    "movq 8*6(%rsi), %rbx\n\t"
    "movq 8*7(%rsi), %rbp\n\t"

    "movq %rcx, %rsp\n\t"
    "pushq %rax\n\t"
    "ret\n\t");
