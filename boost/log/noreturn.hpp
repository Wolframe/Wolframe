#ifndef __NORETURN_H
#define __NORETURN_H

#if defined(__GNUC__)
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif /* defined(__GNUC__) */

#endif /* ifndef __NORETURN_H */
