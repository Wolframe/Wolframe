//
// unused.h
//

/* mark variables as being unused */

#ifndef __WOLFRAME_UNUSED_H
#define __WOLFRAME_UNUSED_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Macro to avoid unused parameter messages in functions
 *
 * __attribute__(unused) creates trouble at least on OpeBSD 4.5..
 */
#ifdef __GNUC__
#define WOLFRAME_UNUSED __attribute__((unused))
#else
#define WOLFRAME_UNUSED
#endif

#ifdef __cplusplus
}
#endif

#endif /* ifndef __WOLFRAME_UNUSED_H */
