#include "process.hpp"
static uint32_t processMutex = 0;

uint64_t processCount = 0;
static processInternal* processHead = nullptr;
static processInternal* currentProcessInternal = nullptr;


bool removeProcessFromList(processInternal* process){
    processInternal* pHead = processHead;

    for (int i = 0; i <= processCount; i++){
        if (pHead->next == process){
            pHead->next = pHead->next->next;
            return true;
        }
        pHead = pHead->next;
    }

    return false;
}

extern int getPid(process* process){
    return process->processID;
}

processInternal* initProcesses(){
    processHead = (processInternal*)(mallocx(sizeof(processInternal)));
    memoryset((void*)processHead, 0, sizeof(processHead));
    // make: create initial process and add to the head
    return processHead;
}

processInternal* setupProcessInfo(){
    processInternal* process = (processInternal*)mallocx(sizeof(processInternal));

    process->processID = 7 * (processCount);
    process->threadCount = 1;
    process->next = nullptr;

    return process;
}

void setupThreadContext(thread* thread, void (*entryPoint)(), bool user, threadState state){
    uintptr_t stack = (uintptr_t)toVirtualAddr((void*)allocateFrame(THREAD_STACK_SIZE));

    thread->regs.rip = (uint64_t)entryPoint;
    thread->entryPoint = entryPoint;
    thread->stackAddress = stack;
    thread->kernelStack = (uintptr_t)mallocx(THREAD_STACK_SIZE);
    thread->kernelStack += THREAD_STACK_SIZE;

    if (user){
        thread->regs = {
                .cs = 0x23,
                .ss = 0x1b
        };

        for (uintptr_t i = 0; i < (THREAD_STACK_SIZE); (i += 2 * _1MB)){
            map(toPhysicalAddr((void*)(stack)) + i, (void*)(((uintptr_t)THREAD_STACK_ADDR - (uintptr_t)THREAD_STACK_SIZE) + i), (pageTableFlag)(ReadWrite | Present | LargerPages), 2 * _1MB);
        }

        thread->regs.rsp = THREAD_STACK_ADDR;
    }
    else{
        thread->regs = {
                .cs = 0x08,
                .rsp = thread->kernelStack,
                .ss = 0x10
        };

        thread->stackAddress = thread->kernelStack;
    }

    thread->regs.eFlags = 0x202;
}

void addThreadToList(thread* thread){
    if (threadHead == nullptr){
        threadHead = (threadList*)mallocx(sizeof(threadList));
        threadHead->threadInfo = thread;
        threadHead->next = nullptr;
    }
    threadList *tHead = threadHead->next;

    while (tHead->next != nullptr) {
        tHead = tHead->next;
    }

    tHead->next = (threadList*) mallocx(sizeof(threadList));
    tHead->next->threadInfo = thread;
    tHead->next->next = nullptr;
}

thread* createThreadInternal(void (*entrypoint)(), threadPriority priority, uint64_t cpuID, threadState state, bool user){
    thread* thread;

    if (processHead == nullptr){
        processHead = initProcesses();
    }


    cpuInfo* cpu = getCPU(cpuID);
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

process* createProcessFromRoutine(void (*entryPoint)(), threadPriority priority, uint64_t cpuID, threadState state, bool user){
    lock(processMutex);

    if (processHead == nullptr){
        processHead = initProcesses();
    }

    process* processInfo = (process*)mallocx(sizeof(process));
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
    registerProcess(currentProcessInternal);
    unlock(processMutex);

    return processInfo;
}

extern process* getProcessInfo(uint64_t processID){
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
    unsigned int divisor = (unsigned int )1193180 / (unsigned int)1000;
//    outb(0x21, 0xFA);
//    outb(0xA1, 0xFF);
//    uint8_t mask = inb(PIC1 + 1) & 0xFE;
//    outb(PIC1 + 1, mask);
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0x00ff));
//    outb(0x40, (uint8_t)((divisor & 0xff00) >> 8));
//    setIDTGate(0x20, (uintptr_t)pitHandler);
//    idtInit();
}

void sleep(int seconds){
    int startTime = getPITCount();
    while (getPITCount() < startTime + seconds){
        asm("hlt");
    }
}

int getPITCount(){
    return pitTicks;
}