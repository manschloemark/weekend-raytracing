#ifndef TIMER_H
#define TIMER_H

#include <time.h>

class timer {
    public: 
    timer(){}

    void start(){ t0 = clock(); }
    void stop() { t1 = clock(); }

    double duration_s() { return (double)(t1 - t0) / CLOCKS_PER_SEC; }
    double duration_ms() { return (t1 - t0) / (CLOCKS_PER_SEC / 1000.0); }

    public:
    clock_t t0, t1;
};

#endif