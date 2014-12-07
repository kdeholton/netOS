#include "process.h"
#include "machine.h"
#include "debug.h"
#include "pic.h"
#include "pit.h"
#include "semaphore.h"
#include "idle.h"
#include "tss.h"
#include "elf.h"
#include "fs.h"
#include "err.h"
#include "libk.h"

void sysOutLong(uint32_t address, uint32_t value)
{
    *(uint32_t*)address = value;
}

uint32_t sysInLong(uint32_t address)
{
    return *(uint32_t*)address;
}

uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot,
                             uint8_t func, uint8_t offset)
 {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    sysOutLong (0xC000, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t)((sysInLong (0xC004) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
 }

uint32_t printPCIHeader(uint32_t addr){
  return 0;
}

/* global process declarations */
Debug* Process::DEBUG;                       // the debug channel
size_t Process::STACK_LONGS = 1024 * 2;      // default kernel stack size
SimpleQueue<Process*> *Process::readyQueue;  // the ready queue
SimpleQueue<Process*> *Process::reaperQueue; // the reaper queue
Process* Process::current;                   // the current process
Atomic32 Process::nextId;                     // next process ID
Semaphore *Process::traceMutex;
Timer* Process::timers = nullptr;            // pending timers
Process* Process::idleProcess = nullptr;     // the idle process
uint32_t Process::idleJiffies = 0;            // idle jiffies

void Process::init() {
    DEBUG = new Debug("Process");
    readyQueue = new SimpleQueue<Process*>();
    reaperQueue = new SimpleQueue<Process*>();
    traceMutex = new Semaphore(1);
}

void Process::checkReaper() {
    Process *kill;
    do {
        kill = nullptr;
        Process::disable();
        if (!Process::reaperQueue->isEmpty()) {
            kill = Process::reaperQueue->removeHead();
        }
        Process::enable();
        Resource::unref(kill);
    } while (kill != nullptr);
}

void Process::checkKilled() {
    checkReaper();
    Process *me = Process::current;
    if (me) {
        if (me->isKilled) {
            me->onKill();
            Process::exit(me->killCode);
        }
    }
}

void Process::entry() {
    checkKilled();
    long code = run();
    exit(code);
}

extern "C" void runProcess() {
    Process::current->entry();
}

/* A bit of room to detect stack overflow without
   corrupting neighbors */
#define FUDGE 128

Process::Process(const char* name, Table *resources_) : 
    Resource(ResourceType::PROCESS), name(name), resources(resources_)
{
    //Debug::printf("Process::Process %p",this);
    id = nextId.getThenAdd(1);
    iDepth = 0;
    iCount = 0;
    isKilled = false;
    killCode = 0;
    disableCount = 0;
    stack = new long[FUDGE + STACK_LONGS];
    if (stack == 0) {
        Debug::panic("can't allocate stack");
    }
    stack = &stack[FUDGE];
    int idx = STACK_LONGS;
    stack[--idx] = 0;
    stack[--idx] = (long) runProcess;
    stack[--idx] = 0; /* %ebx */
    stack[--idx] = 0; /* %esi */
    stack[--idx] = 0; /* edi */
    stack[--idx] = 0; /* ebp */
    kesp = (long) &stack[idx];

    /* Resource table */
    if (resources == nullptr) {
        resources = new Table(100);
    }
    Resource::ref(resources);

    /* We always start with a refcount of 1 */
    count.set(1);
}

Process::~Process() {
    if (stack) {
        delete[] (stack - FUDGE);
        stack = 0;
    }
    if (resources) {
        Resource::unref(resources);
        resources = nullptr;
    }
}

void Process::start() {
    makeReady();
}

void Process::kill(long code) {

    Process::disable();
    if (!isKilled) {
        isKilled = true;
        killCode = code;
    }
    Process::enable();
    checkKilled();
}

uint32_t strdup(const char* str, char* dest) {
    if (str == nullptr) return 0;

    long len = K::strlen(str);
    //Debug::printf("STRLEN:\t\t%d\n",len);
    /* how long is too long? */

    char* out = dest;
    memcpy(out,str,len);
    out[len] = 0;
    return len + 1;
}

long Process::execv(const char* fileName, SimpleQueue<const char*> *args, long argc) {
    //Debug::printf("Address 0xC000: %x\n",*(int*)0xC000);
    //Debug::printf("PCI Config Read Word: %X\n", pciConfigReadWord(0,3,0,0));
    File *prog = FileSystem::rootfs->rootdir->lookupFile(fileName);
    if (prog == nullptr) {
        return ERR_NOT_FOUND;
    }

    //Debug::printf("In execv\n");
    //Debug::printf("FileName: %s\n",fileName);
    //Debug::printf("*argc: %d\n",argc);
    //Debug::printf("argv:\n");
    /*for(int z = 0; z<argc; z++){
	    const char* s = args->removeHead();
	    Debug::printf("%s\n",s);
	    args->addTail(s);
    }*/


    char** k_argv = new char*[argc];
    int i=0;
    while(!args->isEmpty()){
	    char* s = K::strdup(args->removeHead());
	    k_argv[i] = s;
	    i++;
    }

    /* Prepare address space for exec */
    addressSpace.exec();

    char** argv = (char**)0xffff0000;
    char* dest = (char*)0xfff00000;
    for(i=0; i<argc; i++){
	    uint32_t len = strdup(k_argv[i], dest);
	    argv[i] = dest;
	    dest += len;
    }
    /*for(int z = 0; z<argc; z++){
	    Debug::printf("%s\n",argv[z]);
    }*/

    /* Copy args to user space */
    long userESP = 0xfffffff0;
    *((long*)userESP + 0) = argc;
    *((char***)userESP + 1) = argv;

    /* clear resources */
    resources->closeAll();

    /* read ELF */
    Elf32_Ehdr hdr;

    prog->seek(0);
    prog->readFully(&hdr,sizeof(Elf32_Ehdr));

    uint32_t hoff = hdr.e_phoff;

    for (uint32_t i=0; i<hdr.e_phnum; i++) {
        Elf32_Phdr phdr;
        prog->seek(hoff);
        prog->readFully(&phdr,sizeof(Elf32_Phdr));
        hoff += hdr.e_phentsize;

        if (phdr.p_type == PT_LOAD) {
            char *p = (char*) phdr.p_vaddr;
            //uint32_t memsz = phdr.p_memsz;
            uint32_t filesz = phdr.p_filesz;

            prog->seek(phdr.p_offset);
            prog->readFully(p,filesz);
        }
    }

    switchToUser(hdr.e_entry, userESP,0);

    Debug::shutdown("What?");
    return 0;
}

bool Process::isTerminated() {
    return state == TERMINATED;
}

bool Process::isRunning() {
    return state == RUNNING;
}

bool Process::isBlocked() {
    return state == BLOCKED;
}

bool Process::isReady() {
    return state == READY;
}

void Process::makeReady() {
    disable();
    state = READY;
    if (this != idleProcess) {
        readyQueue->addTail(this);
    }
    enable();
}

/******************/
/* Static methods */
/******************/

void Process::vtrace(const char* msg, va_list ap) {
    Process *me = current;
    if (me != 0) {
        traceMutex->down();
    }

    Debug::printf("[");
    if (me) {
        if (me->name) {
            Debug::printf("%s",me->name);
        }
        Debug::printf("#%d",me->id);
    }
    Debug::printf("] ");
    Debug::vprintf(msg,ap);
    Debug::printf("\n");

    if (me != 0) {
        traceMutex->up();
    }
}

void Process::trace(const char* msg, ...) {
    va_list ap;
    va_start(ap,msg);
    vtrace(msg,ap);
    va_end(ap);
}

void Process::exit(long exitCode) {
    Process* p = current;

    if (p) {
        p->exitCode = exitCode;
        p->resources->closeAll();
        p->onExit();
        p->doneEvent.signal();
        
        Process::disable();
        reaperQueue->addTail(p);
        p->state = TERMINATED;
        current = nullptr;

        yield();
        Debug::panic("should never get here");
        Process::enable();
    }
}

/* switch to the next process */
void Process::dispatch(Process *prev) {
    state = RUNNING;

    uint32_t stackBottom = (uint32_t) &stack[STACK_LONGS];
    uint32_t stackBytes = stackBottom - (uint32_t) kesp;
    uint32_t stackLongs = stackBytes / 4;

    if (stackLongs >= STACK_LONGS) {
        Debug::printf("switching to %s\n",name);
        Debug::printf("switching from %s\n",prev ? prev->name : "?");
        Debug::panic("stack for %s is too big, %d bytes, iDepth:%d iCount:%d",name,stackBytes,iDepth,iCount);
    }

    if (this != prev) {
        addressSpace.activate();
        TSS::esp0((uint32_t) &stack[STACK_LONGS]);
        current = this;
        contextSwitch(
            prev ? &prev->kesp : 0, kesp, (disableCount == 0) ? (1<<9) : 0);
    }
    checkKilled();
}

void Process::yield(Queue<Process*> *q) {
    Process::disable();
    Process* me = current;
    if (me) {
        if (q) {
            /* a queue is specified, I'm blocking on that queue */
            if (me->iDepth != 0) {
                Debug::panic("blocking while iDepth = %d",me->iDepth);
            }
            me->state = BLOCKED;
            q->addTail(me);
        } else {
            /* no queue is specified, put me on the ready queue */
            me->makeReady();
        }
    }

    Process* next;

    if (readyQueue->isEmpty()) {
        if (!idleProcess) {
            idleProcess = new IdleProcess();
            idleProcess->start();
        }
        next = idleProcess;            
    } else {
        next = readyQueue->removeHead();
    }

    next->dispatch(me);
    Process::enable();
}

void Process::yield() {
    yield(nullptr);
}

/*******************/
/* The timer class */
/*******************/

class Timer {
public:
    uint32_t target;
    Timer *next;
    SimpleQueue<Process*> waiting;
};

void Process::sleepUntil(uint32_t millis) {
    Process::disable();

    uint32_t target = millis * Pit::hz / 1000;
    if (target > Pit::jiffies) {
        Timer **pp = &timers;
        Timer* p = timers;
        while (p) {
            if (p->target == target) {
                break;
            } else if (p->target > target) {
                p = nullptr;
                break;
            } else {
                pp = &p->next;
                p = p->next;
            }
        }
        if (!p) {
            p = new Timer();
            p->target = target;
            p->next = *pp;
            *pp = p;
        }
        Process::yield(&p->waiting);
    }

    Process::enable();
}

void Process::sleepFor(uint32_t millis) {
    sleepUntil(Pit::millis() + millis);
}

/* called for every timer tick */
void Process::tick() {
    /* interrupts are already disabled but might as well */
    Process::disable();

    if (current == idleProcess) {
        idleJiffies ++;
    }

    Timer* first = timers;
    if (first) {
        if (Pit::jiffies == first->target) {
            timers = first->next;
            while (!first->waiting.isEmpty()) {
                Process* p = first->waiting.removeHead();
                p->makeReady();
            }
        }
    }

    Process::enable();
}

void Process::disable() {
    Pic::off();
    Process* me = current;
    if (me) {
        me->disableCount ++;
    }
}

void Process::enable() {
    Process* me = current;
    /* It is meaningless to enable interrupts without
       a current process */
    if (me) {
        uint32_t c = me->disableCount;
        if (c == 0) {
            Debug::panic("disable = %d",c);        
        } else if (c == 1) {
            me->disableCount = 0;
            Pic::on();
        } else {
            me->disableCount --;
        }
    }
}    

void Process::startIrq() {
    Process* me = Process::current;
    if (current == 0) {
        Debug::panic("startIrq with no process");
        return;
    }
    me->iCount ++;
    me->iDepth ++;
    if (me->disableCount != 0) {
        Debug::panic("disableCount = %d",me->disableCount);
    }
    me->disableCount = 1;
}

void Process::endIrq() {
    Process* me = Process::current;
    if (me == 0) {
        return;
    }
    me->iDepth --;
    if (me->disableCount != 1) {
        Debug::panic("disableCount = %d",me->disableCount);
    }
    me->disableCount = 0;
}
