#include "timestamp.h"

long long timestamp() {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return tp.tv_sec*1000 + tp.tv_usec/1000;
}