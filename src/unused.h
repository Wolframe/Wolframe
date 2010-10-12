//
// UNUSED
//

/* mark variables as being unused */

#ifndef __PORT_UNUSED_H
#define __PORT_UNUSED_H

/**
 * @brief Macro to avoid unused parameter messages in functions
 *
 * __attribute__(unused) creates trouble at least on OpeBSD 4.5..
 */
#ifdef __GNUC__
#define SMERP_UNUSED __attribute__((unused))
#else
#define SMERP_UNUSED
#endif

#endif /* ifndef __PORT_UNUSED_H */
