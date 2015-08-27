#ifndef _CO_SCHEDULER_H_
#define _CO_SCHEDULER_H_

#include <ucontext.h>
#include <memory>
#include <map>

#include "mutex.h"

namespace co {

class Coroutine;

class Scheduler {
public:
    Scheduler(int sz); // stack size;
    ~Scheduler();

    static Scheduler* getGlobalInstance();

    int submit(Coroutine* coroutine);
    int remove(Coroutine* coroutine);
    void yield();
    int resume(int id);

    static void funcWrapper(uint32_t low32, uint32_t hi32);

private:

    void saveStack(Coroutine* co);

    char* stack_;
    ucontext_t main_;

    int sz_;

    int running_;

    std::map<int, Coroutine*> cos_;

    static Scheduler* g_instance_;
    static jutils::Mutex mutex_;
};


} // namespace co

#endif //_CO_SCHEDULER_H_
