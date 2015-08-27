#ifndef _CO_COROUTINE_H_
#define _CO_COROUTINE_H_

#include <cstddef>
#include <ucontext.h>

#include "scheduler.h"


namespace co {

enum COROUTINE_STATE {
    COROUTINE_DEAD = 0,
    COROUTINE_READY = 1,
    COROUTINE_RUNNING = 2,
    COROUTINE_SUSPEND = 3
};

class Coroutine {
public:
    typedef void (*coroutine_func)(void* ud);

    Coroutine(coroutine_func func, void* ud, int cap=(1024*1024), 
            Scheduler* scheduler=NULL);

    int resume();
    int yield();

    void execute();
    void destroyStack();

    COROUTINE_STATE getState() { return this->state_; }
    void setState(COROUTINE_STATE state){ this->state_ = state; }

    int id() { return id_; }
    void setId(int id) { this->id_ = id; }

    void setSize(int sz){ this->sz_ = sz; } 
    int size(){ return this->sz_; }

    int getCapcity(){ return cap_; }
    void setCapcity(int cap);

    void copyStackFromMainContext(const char* src, int sz);
    void copyStackToMainContext(char* dst);


    ucontext_t ctx;

private:
    COROUTINE_STATE state_;
    coroutine_func func_;
    void* ud_;

    //ucontext_t ctx;
    
    Scheduler* scheduler_;
    int cap_;
    
    int sz_;

    char* stack_;

    int id_;

};

} // namespace co


#endif // _CO_COROUTINE_H_
