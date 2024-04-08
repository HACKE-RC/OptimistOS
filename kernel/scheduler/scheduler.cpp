#include "scheduler.hpp"
uint32_t threadMutex = 0;

threadList* prioritySort(threadList* tList){
    if (tList == nullptr || tList->next == nullptr){
        return tList;
    }

    bool swapped;
    threadList* currentThread;

    do {
        swapped = false;
        currentThread = tList;

        while (currentThread->next->threadInfo != nullptr) {
            if (currentThread->threadInfo->priority < currentThread->next->threadInfo->priority) {
                thread *temp = currentThread->threadInfo;
                currentThread->threadInfo = currentThread->next->threadInfo;
                currentThread->next->threadInfo = temp;
                swapped = true;
            }
            currentThread = currentThread->next;
        }
    }
    while (swapped);

    return tList;
}

void runThreads(){
    lock(threadMutex);
    int processorCount = getProcessorCount();
    thread** execThreadList = (thread**)mallocx(processorCount * sizeof(thread*));
    int threadsScheduled = 0;

    threadList* sortedList = prioritySort(threadHead);
    threadList* sortedList2 = sortedList;
    threadList* sortedList3 = sortedList;

    while(sortedList->threadInfo != nullptr && sortedList->next != nullptr){
        for (int i = 0; i < processorCount && sortedList->threadInfo != nullptr; i++){
            execThreadList[i] = sortedList->threadInfo;
            sortedList = sortedList->next;
        }

        for (int i = 0; i < processorCount; i++){
            if (execThreadList[i] != nullptr && sortedList2->threadInfo != nullptr){
                execute(execThreadList[i], i, getQuanta(execThreadList[i]->priority));
                execThreadList[i]->priority = changePriority(execThreadList[i]->priority);
                sortedList2->threadInfo->priority = execThreadList[i]->priority;
                sortedList2 = sortedList2->next;
                threadsScheduled++;
            }
        }
    }

    freex(execThreadList);
    threadHead = sortedList3;
    asm volatile("hlt");
    unlock(threadMutex);
}

extern "C" void saveContext(cpuRegs* context) {
    asm volatile(
            "mov %%rax, %0;"
            "mov %%rbx, %1;"
            "mov %%rcx, %2;"
            "mov %%rdx, %3;"
            "mov %%rsi, %4;"
            "mov %%rdi, %5;"
            "mov %%rbp, %6;"
            "mov %%rsp, %7;"
            "mov %%r8, %8;"
            "mov %%r9, %9;"
            "mov %%r10, %10;"
            "mov %%r11, %11;"
            "mov %%r12, %12;"
            "mov %%r13, %13;"
            "mov %%r14, %14;"
            "mov %%r15, %15;"
            "pushf; popfq;"
            "call get_rip; get_rip: pop %17;"
            : "=m"(context->rax), "=m"(context->rbx), "=m"(context->rcx), "=m"(context->rdx),
    "=m"(context->rsi), "=m"(context->rdi), "=m"(context->rbp), "=m"(context->rsp),
    "=m"(context->r8), "=m"(context->r9), "=m"(context->r10), "=m"(context->r11),
    "=m"(context->r12), "=m"(context->r13), "=m"(context->r14), "=m"(context->r15),
    "=m"(context->eFlags), "=m"(context->rip)
            :
            : "memory"
            );
}

int getProcessorCount(){
    return cpusStarted;
}

void execute(thread* threadInfo, int processorNo, uint8_t quanta){
    cpuInfo* cpu = getCPUInfo(processorNo);
    smpInfo* cpuInfo = getSMPInfo(processorNo);
    load
    cpuInfo->goto_address = threadInfo->entryPoint;
    saveContext(&threadInfo->regs);
}

void loadContext(cpuRegs* context) {
    asm volatile(
        "mov %0, %%rax;"
        "mov %1, %%rbx;"
        "mov %2, %%rcx;"
        "mov %3, %%rdx;"
        "mov %4, %%rsi;"
        "mov %5, %%rdi;"
        "mov %6, %%rbp;"
        "mov %7, %%rsp;"
        "mov %8, %%r8;"
        "mov %9, %%r9;"
        "mov %10, %%r10;"
        "mov %11, %%r11;"
        "mov %12, %%r12;"
        "mov %13, %%r13;"
        "mov %14, %%r14;"
        "mov %15, %%r15;"
        "push %16; popfq;"
        "jmp *%17;"
        :
        : "m"(context->rax), "m"(context->rbx), "m"(context->rcx), "m"(context->rdx),
          "m"(context->rsi), "m"(context->rdi), "m"(context->rbp), "m"(context->rsp),
          "m"(context->r8), "m"(context->r9), "m"(context->r10), "m"(context->r11),
          "m"(context->r12), "m"(context->r13), "m"(context->r14), "m"(context->r15),
          "m"(context->eFlags), "m"(context->rip)
        : "memory"
    );
}


int getQuanta(threadPriority threadPriority){
    switch (threadPriority) {
        case PRIORITY_HIGH:
            return 8;
        case PRIORITY_MEDIUM:
            return 6;
        case PRIORITY_LOW:
            return 3;
        default:
            return 3;
    }
}

threadPriority changePriority(threadPriority currentPriority){
    switch (currentPriority) {
        case PRIORITY_HIGH:
            return PRIORITY_LOW;
        case PRIORITY_LOW:
            return PRIORITY_MEDIUM;
        case PRIORITY_MEDIUM:
            return PRIORITY_HIGH;
    }
    return PRIORITY_LOW;
}