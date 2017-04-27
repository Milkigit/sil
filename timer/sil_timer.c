#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <time.h>
#include <unistd.h>  /* _POSIX_VERSION */

#if !(_POSIX_VERSION >= 199309L)
#error NEED _POSIX_VERSION >= 199309L
#endif

#if !(_POSIX_TIMERS > 0)
#error NEED _POSIX_TIMERS > 0
#endif

#ifndef _POSIX_MONOTONIC_CLOCK
#error NEED _POSIX_MONOTONIC_CLOCK support
#endif

#include "sil_timer.h"

static void sil_timer_elapsed_timespec(struct sil_timer *timer, struct timespec *out)
{
        struct timespec ts;
        int r;

        r = clock_gettime(CLOCK_MONOTONIC, &ts);
        assert(r == 0); (void) r;

        ts.tv_sec -= timer->last_time.tv_sec;
        ts.tv_nsec -= timer->last_time.tv_nsec;
        if (ts.tv_nsec < 0) {
                ts.tv_sec -= 1;
                ts.tv_nsec += 1000000000L;
        }

        *out = ts;
}

void sil_timer_reset(struct sil_timer *timer)
{
        int r;

        r = clock_gettime(CLOCK_MONOTONIC, &timer->last_time);
        assert(r == 0); (void) r;
}

void sil_timer_elapsed_ms(struct sil_timer *timer, long *outms)
{
        struct timespec elapsed;
        long ms;

        sil_timer_elapsed_timespec(timer, &elapsed);

        ms = elapsed.tv_nsec / 1000000L;
        ms += elapsed.tv_sec * 1000L;

        *outms = ms;
}

void sil_timer_elapsed_s_ms(struct sil_timer *timer, int *outs, int *outms)
{
        struct timespec elapsed;

        sil_timer_elapsed_timespec(timer, &elapsed);

        *outms = elapsed.tv_nsec / 1000000L;
        *outs = elapsed.tv_sec;
}
