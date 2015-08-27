#include <utility>
#include <cstdint>

#include "scheduler.h"
#include "mutex.h"
#include "coroutine.h"

using namespace co;
using namespace jutils;
using namespace std;

Scheduler* Scheduler::g_instance_ = NULL;
Mutex Scheduler::mutex_;


void Scheduler::funcWrapper(uint32_t low32, uint32_t hi32){
    uintptr_t ptr = (uintptr_t)low32 | ((uintptr_t)hi32 << 32);
    Scheduler* s = (Scheduler*)ptr;
    int id = s->running_;

    auto iter = s->cos_.find(id);
    Coroutine* co = iter->second;

    co->execute();
    co->setState(COROUTINE_DEAD);

    s->cos_.erase(iter);

    delete co;

    s->running_ = -1;
}

Scheduler* Scheduler::getGlobalInstance(){
    if(g_instance_ == NULL){
        MutexLock lock(&mutex_);
        if(g_instance_ == NULL){
            g_instance_ = new Scheduler((1024 * 1024));
        }
    }
    return g_instance_;
}

Scheduler::~Scheduler(){
    delete[] stack_;
}


Scheduler::Scheduler(int sz)
    : sz_(sz)
{
    stack_ = new char[sz_];
}

int Scheduler::submit(Coroutine* coroutine){
    cos_.insert(pair<int, Coroutine*>(cos_.size(), coroutine));
    return (cos_.size() - 1);
}

int Scheduler::resume(int id){
    auto iter = cos_.find(id);
    if(iter == cos_.end()) return -1;

    Coroutine* co = iter->second;
    COROUTINE_STATE state = co->getState();

    switch (state) {
        case COROUTINE_READY:{
            getcontext(&co->ctx);
            co->ctx.uc_stack.ss_sp = this->stack_;
            co->ctx.uc_stack.ss_size = this->sz_;
            co->ctx.uc_link = &this->main_;

            this->running_ = id;
            co->setState(COROUTINE_RUNNING);
            uintptr_t ptr = (uintptr_t)this;
            makecontext(&co->ctx, (void (*)(void))Scheduler::funcWrapper, 2, (uint32_t)ptr, (uint32_t)(ptr>>32));
            swapcontext(&this->main_, &co->ctx);
            break;
        }
        case COROUTINE_SUSPEND:{
            co->copyStackToMainContext(this->stack_+this->sz_-co->size());
            co->setState(COROUTINE_RUNNING);
            this->running_ = co->id();
            swapcontext(&this->main_, &co->ctx);
            break;
        }
        default:{
            return -1;
        }
    }

    return 0;
}

void Scheduler::yield() {
    int id = this->running_;
    Coroutine* co = cos_.find(id)->second;

    this->saveStack(co);


    co->setState(COROUTINE_SUSPEND);
    this->running_ = -1;

    swapcontext(&co->ctx, &this->main_);
}

void Scheduler::saveStack(Coroutine* co){
    char* top = this->stack_ + sz_;
    char dummy = 0;

    int size = top-&dummy;

    if(co->getCapcity() < size){
        co->setCapcity(size);
    }

    co->copyStackFromMainContext(&dummy, size);
}
