#ifndef _JUTILS_MUTEX_H_
#define _JUTILS_MUTEX_H_

#include <iostream>
#include <pthread.h>

namespace jutils {

class CondVar;

class Mutex {
  public:
    Mutex() {
        if(pthread_mutex_init(&mtx_, NULL) != 0){
            std::cerr << "pthread mutex init failed." << std::endl;
        } 
    }

    ~Mutex() {
        if(pthread_mutex_destroy(&mtx_) != 0){
            std::cerr << "pthread mutex destroy failed." << std::endl;
        } 
    }

    void Lock() {
        if(pthread_mutex_lock(&mtx_) != 0){
            std::cerr << "pthread mutex lock failed." << std::endl;
        }
    }

    void Unlock() {
        if(pthread_mutex_unlock(&mtx_) != 0){
            std::cerr << "pthread mutex unlock failed." << std::endl;
        }
    }
    
  private:
    friend class CondVar;
    pthread_mutex_t mtx_;

    Mutex(const Mutex&);
    void operator=(const Mutex&);
};

class MutexLock {
  public:
    explicit MutexLock(Mutex* mu)
        : mutex_(mu) {
        this->mutex_->Lock();
    }

    ~MutexLock() {
        this->mutex_->Unlock();
    }

  private:

    Mutex *const mutex_;

    // No copying allowed.
    MutexLock(const MutexLock&);
    void operator=(const MutexLock&);

};

} // namespace jutils

#endif // _JUTILS_MUTEX_H_
