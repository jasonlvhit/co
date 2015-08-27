#include <iostream>

#include "coroutine.h"

using namespace std;
using namespace co;

struct args {
    int n;
};

static void foo(void* ud){
    struct args* arg = (struct args*)ud;
    int start = arg->n;
    
    for(int i = 0; i < 5; i++){
        std::cout << "coroutine " <<  (start+i) << endl;
    }    
}

int main(){
    struct args arg1 = { 10 };
    struct args arg2 = { 100 };
    Coroutine* co1 = new Coroutine(foo, &arg1);
    Coroutine* co2 = new Coroutine(foo, &arg2);
    
    std::cout << "co1 id: " << co1->id() << std::endl;
    std::cout << "co2 id: " << co2->id() << std::endl;
    
    co1->resume();
    co2->resume(); 
    
    return 0;    
}
