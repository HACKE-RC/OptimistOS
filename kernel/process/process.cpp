#include "process.hpp"

uint32_t processMutex = 0;
uint64_t processCount = 0;
uint64_t threadCount = 0;

processInternal* processHead = nullptr;
static processInternal* currentProcessInternal = nullptr;

threadList *threadHead = nullptr;

bool removeProcessFromList(processInternal* process){
    processInternal* pHead = processHead;

    for (uint64_t i = 0; i <= processCount; i++){
        if (pHead->next == process){
            pHead->next = pHead->next->next;
            return true;
        }
        pHead = pHead->next;
    }

    return false;
}

bool removeThreadFromList(thread* threadToRemove){
    threadList* tHead = threadHead;

    for (uint64_t i = 0; i <= threadToRemove->threadID; i++){
        if (tHead->next->threadInfo == threadToRemove){
            tHead->next->threadInfo = tHead->next->next->threadInfo;
            return true;
        }
        tHead = tHead->next;
    }

    return false;
}


extern int getPid(process* process){
    return process->processID;
}

processInternal* initProcesses(){
    processHead = (processInternal*)(toVirtualAddr((void*)allocateFrame(sizeof(processHead))));
//    memoryset((void*)processHead, 0, sizeof(processHead));
    // make: create initial process and add to the head
    return processHead;
}

processInternal* setupProcessInfo(){
    auto* process = (processInternal*)mallocx(sizeof(processInternal));

    process->processID = 7 * (processCount);
    process->threadCount = 1;
    process->next = nullptr;

    return process;
}

void setupThreadContext(thread* thread, uintptr_t entryPoint, bool user, threadState state){
    auto stack = (uintptr_t)toVirtualAddr((void*)allocateFrame(THREAD_STACK_SIZE));

    thread->regs.rip = (uint64_t)entryPoint;
    thread->entryPoint = (uintptr_t)entryPoint;
    thread->stackAddress = stack;
    thread->kernelStack = (uintptr_t)mallocx(THREAD_STACK_SIZE);
    thread->kernelStack += THREAD_STACK_SIZE;
    thread->state = THREAD_READY;
    thread->lock = 0;

    if (user){
        thread->regs = {
                .r15 = 0,
                .r14 = 0,
                .r13 = 0,
                .r12 = 0,
                .r11 = 0,
                .r10 = 0,
                .r9 = 0,
                .r8 = 0,
                .rbp = 0,
                .rdi = 0,
                .rsi = 0,
                .rdx = 0,
                .rcx = 0,
                .rbx = 0,
                .rax = 0,
                .cs = 0x28,
                .ss = 0x30,
        };

        for (uintptr_t i = 0; i < (THREAD_STACK_SIZE); (i += 2 * _1MB)){
            map(toPhysicalAddr((void*)(stack)) + i,
                (void*)(((uintptr_t)THREAD_STACK_ADDR - (uintptr_t)THREAD_STACK_SIZE) + i),
                (pageTableFlag)(ReadWrite | Present | LargerPages), 2 * _1MB);
        }

        thread->regs.rsp = ((uintptr_t)stack + THREAD_STACK_SIZE);
    }
    else{
        thread->regs = {
                .cs = 0x28,
                .rsp = thread->kernelStack,
                .ss = 0x30
        };

        thread->stackAddress = thread->kernelStack;
    }

    thread->regs.eFlags = 0x202;
    threadCount++;
}

void addThreadToList(thread* thread){
    lock(threadMutex);

    if (threadHead == nullptr){
        threadHead = (threadList*)toVirtualAddr((void*)allocateFrame(sizeof(threadList)));
        threadHead->threadInfo = thread;
        threadHead->next = nullptr;
        unlock(threadMutex);
        return;
    }

    threadList *tHead = threadHead;

    while (tHead->next != nullptr) {
        tHead = tHead->next;
    }

    tHead->next = (threadList*) mallocx(sizeof(threadList));
    tHead->next->threadInfo = thread;
    tHead->next->next = nullptr;

    unlock(threadMutex);
}

thread* createThreadInternal(uintptr_t entrypoint, threadPriority priority, uint64_t cpuID, threadState state, bool user){
    thread* thread;

    if (processHead == nullptr){
        processHead = initProcesses();
    }

    cpuInfo* cpu = getCPUInfo(cpuID);
    if (cpu == nullptr){
        haltAndCatchFire(__FILE__, __LINE__, "Invalid CPU Requested!!");
        return nullptr;
    }

    setupThreadContext(thread, entrypoint, user, state);
    thread->priority = priority;
    thread->cpuID = cpuID;
    addThreadToList(thread);

    processCount++;
    return thread;
}

process* createProcessFromRoutine(uintptr_t entryPoint, threadPriority priority, uint64_t cpuID, threadState state, bool user){
    asm volatile("cli");
    lock(processMutex);
    if (processHead == nullptr){
        processHead = initProcesses();
    }

    auto* processInfo = (process*)(toVirtualAddr((void*)allocateFrame(sizeof(process))));
    thread* threadInfo;
    processInternal* process = setupProcessInfo();

    if (processInfo == nullptr){
        return nullptr;
    }
    else if (threadInfo == nullptr){
        return nullptr;
    }

    threadInfo = createThreadInternal(entryPoint, priority, cpuID, state, user);
    threadInfo->threadID = process->threadCount + 1;

    process->PML4 = getPageMap(user);
    process->threads[0] = *threadInfo;

    currentProcessInternal = process;

    processInternalToProcess(process, processInfo);
    threadInfo->parentProcess = processInfo;

    registerProcess(currentProcessInternal);
    unlock(processMutex);

    asm volatile("sti");
    return processInfo;
}

process* getProcessInfo(uint64_t processID){
    processInternal* pHead = processHead;
    process* processOut;

    while (pHead != nullptr){
        if (pHead->processID == processID){
            processOut = (process*)(mallocx(sizeof(process)));
            return processInternalToProcess(pHead, processOut);
        }
        pHead = pHead->next;
    }

    return nullptr;
}

thread* getThreadInfo(uint64_t threadID){
    threadList* tList = threadHead;

    while(tList != nullptr){
        if (tList->threadInfo->threadID == threadID){
            return tList->threadInfo;
        }
    }
    return nullptr;
}

threadList* getThreadList(uint64_t threadID, bool next){
    threadList* tList = threadHead;

    while(tList != nullptr){
        if (tList->threadInfo->threadID == threadID){
            return tList;
        }
        else{
            return tList->next;
        }
    }
    return nullptr;
}


thread* getNextThread(thread* currentThread){
    return getThreadList(currentThread->threadID, true)->threadInfo;
}

inline PageTable* getPageMap(bool user){
    if (!user){
        return kernelPML4;
    }
    return newPML4();
}

process* processInternalToProcess(processInternal* processIn, process* processOut){
    processOut->processID = processIn->processID;
    processOut->threadCount = processIn->threadCount;

    for (uint32_t i = 0; i < processIn->threadCount; i++){
        processOut->threads[i] = processIn->threads[i];
    }

    processOut->PML4 = processIn->PML4;
    return processOut;
}

void registerProcess(processInternal* process){
    if (processCount == 0){
        processHead = process;
        process->next = nullptr;
        processCount++;
        return;
    }
    else{
        processInternal* pHead = processHead;
        while(pHead->next != nullptr){
            pHead = pHead->next;
        }
        pHead->next = process;
        process->next = nullptr;
        processCount++;
        return;
    }
}

void pitInit(uint8_t hertz)
{
    unsigned int divisor = (unsigned int )1193180 / (unsigned int)100;
    asm volatile("cli");
    outb(0x43, 0x34);
    auto l = (uint8_t)(divisor & 0xFF);
    auto h = (uint8_t)((divisor>>8) & 0xFF);

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
    handlers[32] = pitHandler;
    asm volatile("sti");
}


void sleep(int seconds){
    uint64_t startTime = getPITCount();
    uint64_t targetTicks = seconds * 100; // since tick 100 times per second
    e9_printf("timer start!!\n");

    while (getPITCount() - startTime < targetTicks){
        asm("hlt");
    }

    e9_printf("timer end!!\n");
}

void sleepMS(int milliseconds){
    uint64_t startTime = getPITCount();
    uint64_t targetTicks = milliseconds * 10; // since tick 100 times per second

    e9_printf("timer start!!\n");

    while (getPITCount() - startTime < targetTicks){
        // Busy-wait loop
        for (volatile int i = 0; i < 1000; ++i) {}
    }

    e9_printf("timer end!!\n");
}

uint64_t getPITCount(){
    return pitTicks;
}