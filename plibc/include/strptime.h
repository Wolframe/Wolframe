#ifndef PLIBC_STRPTIME_H_INCLUDED
#define PLIBC_STRPTIME_H_INCLUDED
#include <time.h>

extern "C"
{
char* plibc_strptime (const char *buf, const char *format, struct tm *tm);
}
#endif
