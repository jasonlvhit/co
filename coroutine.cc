#include <iostream>
#include <memory>
#include <string.h>

#include "coroutine.h"

using namespace co;
using namespace std;


Coroutine::Coroutine(coroutine_func func, void* ud, int cap, Scheduler* scheduler)
   : state_(COROUTINE_READY),
     func_(func),
     ud_(ud),
     scheduler_(scheduler),
     cap_(cap),
     sz_(0)
{
    if(scheduler_ == NULL){
        scheduler_ = Scheduler::getGlobalInstance();
    }

    int r = scheduler_->submit(this);
    if(r < 0){
        std::cout << "coroutine init failed." << std::endl;
        return;
    }

    this->id_ = r;
    this->stack_ = new char[this->cap_];
}


Coroutine::~Coroutine(){
    delete[] this->stack_;
}


// positive for success.
int Coroutine::resume(){
    return scheduler_->resume(this->id_);
}


void Coroutine::setCapcity(int cap){
    delete[] this->stack_;
    this->cap_ = cap;

    this->stack_ = new char[cap];
}

void Coroutine::copyStackFromMainContext(const char* src, int sz){
    memcpy(this->stack_, (void*)src, sz);
    this->sz_ = sz;
}

void Coroutine::copyStackToMainContext(char* dst){
    memcpy(dst, this->stack_, this->sz_);
}

void Coroutine::execute(){
    this->func_(this->ud_);
}
