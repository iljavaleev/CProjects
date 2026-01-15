#ifndef PTHREADM_H
#define PTHREADM_H
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define ERROR(msg) { printf("%s\n", msg); exit(1); }
#define LOCK(mutex) { if(pthread_mutex_lock(&mutex) != 0) ERROR("error lock"); }
#define UNLOCK(mutex) {   if(pthread_mutex_unlock(&mutex) != 0) \
    ERROR("error unlock"); }
#define WAIT(cond, mutex) { if(pthread_cond_wait(&cond, &mutex) != 0) \
    ERROR("error wait"); }
#define SEND_SIGNAL(cond) { if(pthread_cond_signal(&cond) != 0) \
    ERROR("error send signal"); }
#define SEND_BROAD_SIGNAL(cond) { if(pthread_cond_broadcast(&cond) != 0) \
    ERROR("error send signal"); }
#define JOIN(tid) { if(pthread_join(tid, NULL) != 0) ERROR("error join"); }
#define JOIN_WITH_RES(tid, res) { if(pthread_join(tid, res) != 0) \
    ERROR("error join"); }
#define CREATE(tid, func, arg){ \
    if(pthread_create(&tid, NULL, func, (void*) arg) != 0) \
    ERROR("error pthread_create");}

#endif