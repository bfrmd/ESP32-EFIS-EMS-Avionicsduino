#ifndef RDAMTIMERS_H
#define RDAMTIMERS_H

#include "RDAMglobals.h"

void timersInit();
static void oneshot_timer_5ms_callback(void *arg);
static void oneshot_timer_50ms_callback(void *arg);
static void oneshot_timer_200ms_callback(void *arg);
static void oneshot_timer_250ms_callback(void *arg);
static void oneshot_timer_500ms_callback(void *arg);
static void oneshot_timer_5000ms_callback(void *arg);


#endif