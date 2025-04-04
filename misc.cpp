#include <iostream>
#include <chrono>
#include "chess.h"

#ifdef _WIN32
#include "windows.h"
#else
#include "time.h"
#endif

unsigned long getTimeInMilliseconds()
{
    //TODO: Use chrono to get time in nanoseconds...Highest Precision
#ifdef _WIN32
    return GetTickCount();
#else
    struct timeval tv;
    mingw_gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
#endif

}
using namespace std::chrono;
unsigned long long getTimeInNanoseconds()
{
    return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
}