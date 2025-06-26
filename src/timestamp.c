#include "timestamp.h"

// usando long long pra (tentar) sobreviver aoano  2038
long long timestamp() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec*1000 + tp.tv_usec/1000;
}