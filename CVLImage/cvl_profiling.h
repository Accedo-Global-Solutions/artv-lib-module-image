#ifndef CVL_PROFILING_H
#define CVL_PROFILING_H


#define CVL_TICN_CHOOSER(A,B,FUNC, ...)  FUNC
/**
 * Profiling macro capable of executing code block several times and calculating average execution
 * time.
 *
 * Marks start of profiled code block. Accepts one or two arguments.
 * Compared with CVL_TIC macro this macro implementation wraps profiled code with compound statement
 * (block). This makes macro potentially incompatible with CVL_TIC macro.
 *
 * Usage.
 * Single argument, execute code once:
 * CVL_TICN(tag);
 * ...code...
 * CVL_TOCN(tag)
 *
 * Two arguments, execute code count times and calculate average execution time.
 * CVL_TICN(tag, count);
 * ...code...
 * CVL_TOCN(tag)
 */
#define CVL_TICN(...) CVL_TICN_CHOOSER(__VA_ARGS__,\
                          CVL_TICN_BATCH(__VA_ARGS__),\
                          CVL_TICN_ONCE(__VA_ARGS__)\
                      )
/**
 * \def CVL_TOCN(tag)
 * Marks end of profiled code block.
 *
 * Companion macro for CVL_TICN(tag[, count])
 */

/**
 * \def CVL_TIC(tag)
 *
 * Measure code execution time.
 *
 * Marks start of profiled code block.
 *
 * Usage.
 * CVL_TIC(tag)
 * ...code...
 * CVL_TOC(tag)
 */

/**
 * \def CVL_TOC(tag)
 * Marks end of profiled code block.
 *
 * Companion macro for CVL_TIC(tag)
 */

#if CVL_PROFILING

#include <time.h>

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32)) && !defined(__CYGWIN__)

// Define timespec structure if not defined (MinGW has this structure).
#ifndef __MINGW32__
#if !defined(HAVE_STRUCT_TIMESPEC)
#define HAVE_STRUCT_TIMESPEC
#if !defined(_TIMESPEC_DEFINED)
#define _TIMESPEC_DEFINED
struct timespec {
        time_t tv_sec;
        long tv_nsec;
};
#endif // _TIMESPEC_DEFINED
#endif // HAVE_STRUCT_TIMESPEC
#endif // __MINGW32__

// We need only basic WINAPI without crypto functions and min/max macro.
#ifndef WINDOWS_H
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

/**
 * Windows version of clock_gettime routine.
 *
 * @param tv - pointer to resulting timespec structure
 * @return 0
 */
static int cvl_windows_clock_get_time(struct timespec *tv) {
    LARGE_INTEGER t;
    FILETIME      f;

    double               microseconds;
    static LARGE_INTEGER offset;
    static double        freq_to_microseconds;
    static int           initialized = 0;
    static BOOL          use_performance_counter = 0;

    // Initialize freq_to_microseconds.
    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        use_performance_counter = QueryPerformanceFrequency(&performanceFrequency);

        // If hardware supports a high-resolution performance counter.
        if (use_performance_counter) {
            QueryPerformanceCounter(&offset);
            freq_to_microseconds = performanceFrequency.QuadPart / 1000000.0;
        } else {
            SYSTEMTIME s;
            FILETIME f;

            // UNIX epoch
            s.wYear = 1970;
            s.wMonth = 1;
            s.wDay = 1;
            s.wHour = 0;
            s.wMinute = 0;
            s.wSecond = 0;
            s.wMilliseconds = 0;

            SystemTimeToFileTime(&s, &f);
            offset.QuadPart = f.dwHighDateTime;
            offset.QuadPart <<= 32;
            offset.QuadPart |= f.dwLowDateTime;

            freq_to_microseconds = 10.0;
        }
    }

    if (use_performance_counter) {
        QueryPerformanceCounter(&t);
    }
    else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    microseconds = t.QuadPart / freq_to_microseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_nsec = t.QuadPart % 1000000 * 1000;

    return 0;
}

#else

#include <sys/time.h>
#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#endif // WIN32

#ifdef __ANDROID__
#include <android/log.h>
#define LOGP(...) __android_log_print(ANDROID_LOG_DEBUG, "PROFILE", __VA_ARGS__)
#else
#include <stdio.h>
#define LOGP(...) printf(__VA_ARGS__), fflush(stdout)
#endif

static inline void current_utc_time(struct timespec *ts) {
#ifdef __APPLE__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32)) && !defined(__CYGWIN__)
    cvl_windows_clock_get_time(ts);
#else
    clock_gettime(CLOCK_REALTIME, ts);
#endif
}

#define CVL_TICN_ONCE(tag) \
struct timespec tag##_start_ts;\
const unsigned int tag##_batch_size = 1;\
current_utc_time(&tag##_start_ts);\
{

#define CVL_TICN_BATCH(tag, count) \
struct timespec tag##_start_ts;\
const unsigned int tag##_batch_size = count;\
current_utc_time(&tag##_start_ts);\
unsigned int tag##_batch_iteration;\
for (tag##_batch_iteration = 0; tag##_batch_iteration < tag##_batch_size; tag##_batch_iteration+=1)\
{

#define CVL_TOCN(tag) \
}\
struct timespec tag##_finish_ts;\
current_utc_time(&tag##_finish_ts);\
double tag##_elapsed = (tag##_finish_ts.tv_sec - tag##_start_ts.tv_sec);\
tag##_elapsed += (tag##_finish_ts.tv_nsec - tag##_start_ts.tv_nsec) / 1000000000.0;\
if (tag##_batch_size > 1) {\
    LOGP(#tag": %f sec. (over %d iterations)\n", tag##_elapsed / tag##_batch_size , tag##_batch_size);\
}\
else {\
    LOGP(#tag": %f sec.\n", tag##_elapsed);\
}


#define CVL_TIC(tag) \
struct timespec tag##_start_ts;\
current_utc_time(&tag##_start_ts);

#define CVL_TOC(tag) \
struct timespec tag##_finish_ts;\
current_utc_time(&tag##_finish_ts);\
double tag##_elapsed = (tag##_finish_ts.tv_sec - tag##_start_ts.tv_sec);\
tag##_elapsed += (tag##_finish_ts.tv_nsec - tag##_start_ts.tv_nsec) / 1000000000.0;\
LOGP(#tag": %f sec.\n", tag##_elapsed);

#else

#define CVL_TIC(tag)
#define CVL_TOC(tag)
#define CVL_TICN_BATCH(tag, count)
#define CVL_TICN_ONCE(tag)
#define CVL_TOCN(tag)
#endif


#endif //CVL_PROFILING_H

