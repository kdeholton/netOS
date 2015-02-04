#include "syscall.h"
#include "machine.h"
#include "idt.h"
#include "process.h"
#include "child.h"
#include "fs.h"
#include "err.h"
#include "u8250.h"
#include "libk.h"
#include "console.h"

void Syscall::init(void) {
  IDT::addTrapHandler(100,(uint32_t)syscallTrap,3);
}

extern "C" long syscallHandler(uint32_t* context, long num, long a0, long a1) {

  switch (num) {
    case 0: /* exit */
      Process::exit(a0);
      return -1;
    case 1: /* putchar */
      //if((a0 >= 0x20 && a0 <= 0x7e) || a0 == 0xA){
      return Console::me->put(a0);
      //}
      return 0;
    case 2: /* fork */
      {
        uint32_t userPC = context[8];
        uint32_t userESP = context[11];
        Child *child = new Child(Process::current);
        child->pc = userPC;
        child->esp = userESP;
        child->eax = 0;
        long id = Process::current->resources->open(child);
        child->start();

        return id;
      }
    case 3: /* semaphore */
      {
        Semaphore *s = new Semaphore(a0);
        return Process::current->resources->open(s);
      }
    case 4: /* down */
      {
        Semaphore* s = (Semaphore*) Process::current->resources->get(
            a0,ResourceType::SEMAPHORE);
        if (s == nullptr) return ERR_INVALID_ID;
        s->down();
        return 0;
      }
    case 5 : /* up */
      {
        Semaphore* s = (Semaphore*) Process::current->resources->get(a0,
            ResourceType::SEMAPHORE);
        if (s == nullptr) return ERR_INVALID_ID;
        s->up();
        return 0;
      }
    case 6 : /* join */
      {
        Process *proc = (Process*) Process::current->resources->get(a0,
            ResourceType::PROCESS);
        if (proc == nullptr) return ERR_INVALID_ID;
        proc->doneEvent.wait();
        long code = proc->exitCode;
        Process::current->resources->close(a0);
        return code;
      }
    case 7 : /* shutdown */
      {
        Debug::shutdown("");
        return 0;
      }
    case 8 : /* open */
      {
        File* f = FileSystem::rootfs->rootdir->lookupFile((char*) a0);
        if (f == nullptr) return ERR_NOT_FOUND;
        else return Process::current->resources->open(f);
      }
    case 9 : /* getlen */
      {
        File* f = (File*) Process::current->resources->get(a0,ResourceType::FILE);
        if (f == nullptr) {
          return ERR_INVALID_ID;
        }
        return f->getLength();
      }
    case 10: /* read */
      {
        long *args = (long*) a0;
        File* f = (File*) Process::current->resources->get(args[0],ResourceType::FILE);
        if (f == nullptr) {
          return ERR_INVALID_ID;
        }
        void* buf = (void*) args[1];
        long len = (long) args[2];
        return f->read(buf,len);
      }
    case 11 : /* seek */
      {
        File* f = (File*) Process::current->resources->get(a0,ResourceType::FILE);
        if (f == nullptr) {
          return ERR_INVALID_ID;
        }
        f->seek(a1);
        return 0;
      }
    case 12 : /* close */
      {
        return Process::current->resources->close(a0);
      }
    case 13: /* execv */
      {
        /* find the security exposures in this code */
        int argc = a0;
        char** userArgs = (char**) a1;
        char* name = *userArgs;

        SimpleQueue<const char*> args;

        int i = 0;


        for(; i<argc; i++){
          if(userArgs[i] == 0)
            break;
          char* s = K::strdup(userArgs[i]);
          if (s == 0) break;
          args.addTail(s);
        }

        long rc = Process::current->execv(name,&args,i);

        /* execv failed, cleanup */
        while (!args.isEmpty()) {
          const char* s = args.removeHead();
          delete[] s;
        }
        return rc;
      }
    case 14: /* getchar */
      {
        return Console::me->get();
      }
    case 15: /* sleepfor */
      {
        Process::current->sleepFor(a0);
        return 0;
      }
    case 16: /* clear */
      {
        Console::me->clear();
        return 0;
      }
    case 17: /* write */
      {
        long* args = (long*)a0;
        File* f = (File*) Process::current->resources->get(args[0],ResourceType::FILE);
        uint16_t* buf = (uint16_t*)args[1];
        long len = args[2];
        if(f == nullptr){
          return ERR_INVALID_ID;
        }

        f->write(buf, len);
        return 0;
      }
    case 18: /*putcolor*/
      {
        long* args = (long*)a0;
        Console::me->putcolor(args[0], args[1], args[2]);
        return 0;
      }
    case 19: /*move down and to top*/
      {
        Console::me->moveToZero();
        return 0;
      }
    case 20: /* get row of the cursor*/
      {
        return Console::me->getRow();
      }
    case 21: /* get col of the cursor*/
      {
        return Console::me->getColumn();
      }
    case 22: /* set row and col of the cursor*/
      {
        Console::me->setCursor(a0, a1);
        return 0;
      }
    case 23: /* decr internal row */
      {
        Console::me->decrementOnlyRow();
        return 0;
      }
    default:
      Process::trace("syscall(%d,%d,%d)",num,a0,a1);
      return -1;
  }
}
