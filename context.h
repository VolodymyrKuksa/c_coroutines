typedef struct
{
    void *r12, *r13, *r14, *r15, *rip, *rsp, *rbx, *rbp;
} Context;

void get_context(volatile Context* c);
void set_context(volatile Context* c);
