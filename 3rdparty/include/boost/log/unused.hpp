#ifndef __UNUSED_HEADER_INCLUDED
#define __UNUSED_HEADER_INCLUDED

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

#endif /* !defined( __UNUSED_HEADER_INCLUDED ) */
