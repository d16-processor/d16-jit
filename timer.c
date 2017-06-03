#include "timer.h"
#include <unistd.h>
#include <sys/time.h>
long long current_milliseconds(){
    struct timeval te;
    gettimeofday(&te,NULL);
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000LL;
    return milliseconds;
}
long long curtime = 0;
long long ms_to_wait = 0;
void timer_init(){
    curtime = 0;
    ms_to_wait = 0;
}
void timer_set(uint16_t val){
    curtime = current_milliseconds();
    ms_to_wait = val;
}
uint16_t timer_get(){
    if(curtime == 0 || ms_to_wait == 0)
        return 0;
    long long ms_remaining = current_milliseconds() - curtime;
    usleep(1000);
    return (ms_remaining < ms_to_wait) ? (ms_to_wait - ms_remaining) : 0;    
}
