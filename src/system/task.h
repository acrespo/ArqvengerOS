#ifndef __SYSTEM_TASK__
#define __SYSTEM_TASK__

#include "system/process/process.h"

struct TaskState {
    unsigned short previousTask;
    unsigned short res0;

    unsigned int esp0;
    unsigned short ss0;
    unsigned short res1;

    unsigned int esp1;
    unsigned short ss1;
    unsigned short res2;

    unsigned int esp2;
    unsigned short ss2;
    unsigned short res3;

    unsigned int cr3;
    unsigned int eip;
    unsigned int eflags;
    unsigned int eax;
    unsigned int ecx;
    unsigned int edx;
    unsigned int ebx;
    unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;

    unsigned short es;
    unsigned short res4;

    unsigned short cs;
    unsigned short res5;

    unsigned short ss;
    unsigned short res6;

    unsigned short ds;
    unsigned short res7;

    unsigned short fs;
    unsigned short res8;

    unsigned short gs;
    unsigned short res9;

    unsigned short ldtr;
    unsigned short res10;

    unsigned short res11;
    unsigned short iopb;

} __attribute__((packed));


struct TaskState* task_create_tss(void);

void task_load(struct Process* p);

#endif
