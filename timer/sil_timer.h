#include <unistd.h>
#if !(_POSIX_VERSION >= 199309L)
#error NEED _POSIX_VERSION >= 199309L
#endif

#if !(_POSIX_C_SOURCE >= 199309L)
#error NEED _POSIX_C_VERSION >= 199309L for struct timespec
#endif

#include <time.h>

struct sil_timer {
        struct timespec last_time;
};

void sil_timer_reset(struct sil_timer *timer);
void sil_timer_elapsed_ms(struct sil_timer *timer, long *ms);
void sil_timer_elapsed_s_ms(struct sil_timer *timer, int *s, long *ms);
